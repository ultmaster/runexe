#ifndef __process_h_
#define __process_h_

#include <string>

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
    std::string input_file;
    std::string output_file;
    std::string error_file;
    std::string directory;
    bool check_idleness;
};

ProcessOutcome run(const std::string &command_line,
                   const ProcessParams &params);

#endif

