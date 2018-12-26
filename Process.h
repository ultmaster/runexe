#ifndef __process_h_
#define __process_h_

#include <string>
#include <vector>

enum ProcessState {
    BEFORE = 0,
    RUNNING,
    EXITED,
    FAILED,
    TIME_EXCEEDED,
    MEMORY_EXCEEDED,
    IDLENESS_EXCEEDED
};

struct ProcessOutcome {
    ProcessState state;

    int exit_code;

    /* Memory peak consumed, in bytes. */
    long long memory;

    /* Time consumed, in milliseconds. */
    long long time;

    /** Time passed (not CPU). */
    long long passed_time;

    /* Absolute path to the file, containing stdout. */
    std::string output_file;

    /* Absolute path to the file, containing stderr. */
    std::string error_file;

    /* Invocation human-readable comment. */
    std::string comment;
};

struct ProcessParams {
    long long time_limit;
    long long memory_limit;
    std::string command_line;
    std::string input_file;
    std::string output_file;
    std::string error_file;
    std::string directory;
    bool check_idleness;

    std::vector<int> closing_fds;
    int input_fd;
    int output_fd;

    ProcessParams() : time_limit(0), memory_limit(0), input_fd(-1), output_fd(-1), check_idleness(false) {

    }
};

ProcessOutcome run(const ProcessParams &params);

#endif

