#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <cstdarg>
#include <iostream>
#include <vector>
#include <csignal>
#include <cstdlib>
#include <unistd.h>
#include <climits>
#include <errno.h>

#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>

#include <unistd.h>

#include "Process.h"
#include "Strings.h"

using namespace std;

struct Process {
    /* Directory, where to run the process. */
    string directory;
    /* Command to be executed, without arguments. */
    string commandLine;
    /* Command arguments. */
    vector<string> args;

    /* Memory limit in bytes. */
    long long memoryLimit;
    /* Time limit in milliseconds. */
    long long timeLimit;

    /* Redirection file for stdin, empty if not used. */
    string redirectStdinFile;
    /* Redirection file for stdout, empty if not used. */
    string redirectStdoutFile;
    /* Redirection file for stderr, empty if not used. */
    string redirectStderrFile;

    /* File descriptors that will be closed in child process */
    vector<int> closingFds;

    /* Redirect descriptor for input, this will preempt redirectStdinFile if it's >=0 */
    int redirectInputDescriptor;

    /* Redirect descriptor for output */
    int redirectOutputDescriptor;

    /* Is true iff process completed execution (completed, interrupted, signalled and so on). */
    bool completed;
    /* Process pid. */
    int pid;
    /* Process exit code or -1 if execution failed because of any reason. */
    int exitCode;
    /* state. */
    ProcessState state;
    /* Consumed time. */
    long long consumedTime;
    /* Consumed memory. */
    long long consumedMemory;

    /* Human-readable comment. */
    string comment;

    /* Check idleness? */
    bool checkIdleness;
};

static string formatByAp(const char *format, va_list ap) {
    // TODO: Possible buffer overflow!
    static int MAX_FORMAT_LENGTH = 1024 * 1024;

    char *msg = new char[MAX_FORMAT_LENGTH];
    vsprintf(msg, format, ap);
    string result = msg;
    delete[] msg;

    return result;
}

static string format(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    string result = formatByAp(fmt, ap);
    va_end(ap);
    return result;
}

static void fail(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    string result = format(fmt, ap);
    va_end(ap);
    fprintf(stderr, "FAIL: %s\n", result.c_str());
    exit(EXIT_FAILURE);
}

static void redirectFd(int fd, const char *const fileName, const char *const mode) {
    FILE *fp = fopen(fileName != NULL && strlen(fileName) ? fileName : "/dev/null", mode);

    if (!fp)
        fail("Can't setup redirection from/to \"%s\"", fileName);

    if (dup2(fileno(fp), fd) == -1)
        fail("Can't setup descriptor for redirection from/to \"%s\"", fileName);
}

static void redirectFd(int fd, int fileno) {
    if (dup2(fileno, fd) == -1)
        fail("Can't setup descriptor for redirection from/to \"%d\"", fileno);
}

void rlim(int resource, rlim_t limit) {
    rlimit rl;
    rl.rlim_cur = limit;
    rl.rlim_max = limit;
    if (setrlimit(resource, &rl) == -1)
        fail("Can't setup rlimit (%d,%d)", resource, int(limit));
}

static void execute(Process &p) {
    p.state = BEFORE;
    p.comment = "";

    // Setup directory.
    if (!p.directory.empty() && chdir(p.directory.c_str()))
        exit(EXIT_FAILURE);

    // Setup redirection.
    if (p.redirectInputDescriptor != -1)
        redirectFd(0, p.redirectInputDescriptor);
    else
        redirectFd(0, p.redirectStdinFile.c_str(), "r");

    if (p.redirectOutputDescriptor != -1)
        redirectFd(1, p.redirectOutputDescriptor);
    else
        redirectFd(1, p.redirectStdoutFile.c_str(), "w");

    redirectFd(2, p.redirectStderrFile.c_str(), "w");

    // Close fds that will be longer used
    for (size_t i = 0; i < p.closingFds.size(); ++i)
        close(p.closingFds[i]);

    // Setup limits.

    if (p.timeLimit > 0)
        rlim(RLIMIT_CPU, (rlim_t) (p.timeLimit / 1000 + 1)); // Given + 1 seconds.
    if (p.memoryLimit > 0) {
        rlim_t memory_limit = (rlim_t) min(p.memoryLimit + 1024 * 1024 * 32, (long long) INT_MAX); // Given + 32 megabytes.
#ifndef __APPLE__
        rlim(RLIMIT_STACK, memory_limit);  // does not work, I don't know why
        // TODO: check this works on linux
#endif
        rlim(RLIMIT_AS, memory_limit);
    }

    char *commandLine = strdup(p.commandLine.c_str());
    char **argv = new char *[p.args.size() + 2];
    argv[0] = strdup(p.commandLine.c_str());
    for (size_t i = 0; i < p.args.size(); i++)
        argv[i + 1] = strdup(p.args[i].c_str());
    argv[p.args.size() + 1] = NULL;

    char **envp = new char *[2];
    envp[0] = strdup((string("PATH=") + getenv("PATH")).c_str());
    envp[1] = NULL;

    p.state = RUNNING;
    execvp(commandLine, argv);

    p.completed = true;
    p.state = FAILED;
    p.exitCode = -1;

    if (errno == ENOENT)
        p.comment = "No such file";

    if (errno == E2BIG)
        p.comment = "Too large arguments";
}

static void updateByRusage(Process &p, struct rusage &usage) {
    p.consumedTime = max(p.consumedTime, (long long) (usage.ru_utime.tv_sec * 1000LL
                                                      + usage.ru_utime.tv_usec / 1000LL
                                                      + usage.ru_stime.tv_sec * 1000LL
                                                      + usage.ru_stime.tv_usec / 1000LL));
#ifdef __APPLE__
    p.consumedMemory = max(p.consumedMemory, (long long) usage.ru_maxrss);
#else
    p.consumedMemory = max(p.consumedMemory, usage.ru_maxrss * 1024LL);
#endif
}

static bool isIdle(const Process &p, long long passedTimeMillis) {
    return p.checkIdleness && (p.timeLimit > 0 && passedTimeMillis > 2 * p.timeLimit);
}

static void waitFor(Process &p) {
    int iter = 0;
    long long passedTimeMillis = 0;

    // Main loop.
    while (!p.completed) {
        iter++;
        int status;
        pid_t wres;
        struct rusage usage;
        wres = wait4(p.pid, &status, WUNTRACED | WCONTINUED | WNOHANG, &usage);

        // Is running?
        if (wres == 0) {
            if (isIdle(p, passedTimeMillis)) {
                p.completed = true;
                p.exitCode = -1;
                p.state = IDLENESS_EXCEEDED;
                p.comment = "Process hangs or something like it";
                kill(p.pid, SIGKILL);
                return;
            }
            int sleep_milliseconds = min(iter, 50);
            passedTimeMillis += sleep_milliseconds;
            usleep(sleep_milliseconds * 1000);
        } else {
            updateByRusage(p, usage);
            if (p.consumedTime > p.timeLimit && p.timeLimit > 0) {
                p.completed = true;
                p.exitCode = -1;
                p.state = TIME_EXCEEDED;
                p.comment = format("Process has been time limited [timeLimit=%lld ms]", p.timeLimit);
                return;
            }
            if (p.consumedMemory > p.memoryLimit && p.memoryLimit > 0) {
                p.completed = true;
                p.exitCode = -1;
                p.state = MEMORY_EXCEEDED;
                p.comment = format("Process has been memory limited [memoryLimit=%lld kb]", p.memoryLimit / 1024);
                return;
            }
            if (wres == -1 && errno == EINTR) {
                p.completed = true;
                p.exitCode = -1;
                p.state = FAILED;
                p.comment = "Process has been killed [errno == EINTR]";
                return;
            } else if (wres == p.pid && WIFSTOPPED(status)) {
                p.completed = true;
                p.exitCode = -1;
                p.state = FAILED;
                p.comment = "Process has been killed [WIFSTOPPED]";
                return;
            } else if (wres == p.pid && WIFSIGNALED(status)) {
                p.completed = true;
                p.exitCode = -1;
                printf("signal = %d, exit code = %d\n", WIFSIGNALED(status), WIFEXITED(status));
                p.comment = "Process has been killed [WIFSIGNALED]";
                p.state = FAILED;
                return;
            } else if (wres == p.pid && WIFEXITED(status)) {
                p.completed = true;
                p.exitCode = WEXITSTATUS(status);
                p.comment = "";
                p.state = EXITED;
                return;
            } else {
                p.completed = true;
                p.exitCode = -1;
                p.comment = format("Unexpected process state: wres=%d, status=%d.", int(wres), status);
                p.state = FAILED;
                return;
            }
        }
    }
}

static long long time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000LL + tv.tv_usec / 1000LL;
}

static ProcessOutcome run(Process &p) {
    if (p.redirectStdoutFile.empty() && p.redirectOutputDescriptor == -1) {
        char name[] = "/tmp/process_output_XXXXXX";
        close(mkstemp(name));
        p.redirectStdoutFile = name;
    }

    if (p.redirectStderrFile.empty()) {
        char name[] = "/tmp/process_error_XXXXXX";
        close(mkstemp(name));
        p.redirectStderrFile = name;
    }

    ProcessOutcome result;
    p.consumedMemory = p.consumedTime = 0LL;
    p.state = FAILED;
    p.exitCode = -1;
    p.completed = false;
    p.comment = "Can't fork().";
    long long start = time_ms();

    if (!(p.pid = fork())) {
        execute(p);
        exit(EXIT_FAILURE);
    } else {
        // closing file descriptors that will no longer be used
        if (p.redirectInputDescriptor != -1)
            close(p.redirectInputDescriptor);
        if (p.redirectOutputDescriptor != -1)
            close(p.redirectOutputDescriptor);

        if (p.pid > 0)
            waitFor(p);
        kill(p.pid, SIGKILL);
        result.state = p.state;
        result.exit_code = p.exitCode;
        result.time = p.consumedTime;
        result.memory = p.consumedMemory;
        result.output_file = p.redirectStdoutFile;
        result.error_file = p.redirectStderrFile;
        result.comment = p.comment;
        result.passed_time = time_ms() - start;
    }

    return result;
}

ProcessOutcome run(const ProcessParams &params) {
    Process p;
    p.directory = params.directory;
    p.timeLimit = params.time_limit;
    p.memoryLimit = params.memory_limit;
    p.redirectStdinFile = params.input_file;
    p.redirectStdoutFile = params.output_file;
    p.redirectStderrFile = params.error_file;
    p.checkIdleness = params.check_idleness;
    p.closingFds = params.closing_fds;
    p.redirectInputDescriptor = params.input_fd;
    p.redirectOutputDescriptor = params.output_fd;

    vector<string> args = runexe::Strings::tokenizeCommandLine(params.command_line);
    p.commandLine = args[0];
    for (size_t i = 1; i < args.size(); i++)
        p.args.push_back(args[i]);
    return run(p);
}
