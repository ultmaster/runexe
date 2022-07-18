#ifndef _RUN_H_
#define _RUN_H_

#include "configuration.h"
#include "invocation_params.h"
#include "invocation_result.h"

#include <string>
#include <vector>

namespace runexe {
    InvocationParams processCommandLine(int argc, char *argv[]);

    InvocationParams processParams(const std::vector<std::string> &params);

    void quit(unsigned int exitCode = RUN_EXIT_FAILURE);

    void showInfo();

    void showHelp();

    void crash(const std::string &comment);

    void printTimes(double userTime, double kernelTime, double timeLimit = 0.0,
                    bool isTimeLimitExceeded = false);

    void fail(const std::string &comment);

    void printInvocationResult(const InvocationParams &invocationParams,
                               const InvocationResult &invocationResult);

    void printXmlInvocationResult(const InvocationResult &invocationResult, const std::string &fileName);

    void printXmlInvocationResult(const std::vector<InvocationResult> &invocationResults,
                                  const std::string &fileName);
}

#endif
