#include "Run.h"
#include "Strings.h"

#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <iostream>

using namespace runexe;
using namespace std;

void runexe::quit(unsigned int exitCode) {
    exit(exitCode);
}

void runexe::showInfo() {
    cout << endl
         << "RunExe for *NIX, Version " << version << endl
         << "Copyright(c) Saratov State U Codecenter Development Team, "
         << copyrightYears << endl
         << "Based on runlib by Paul P. Komkoff Jr." << endl
         << "Written by Dmitry Levshunov, Mike Mirzayanov" << endl
         << "Modified by ultmaster, 2019" << endl
         << "Use \"runexe -h\" to get help information" << endl;
}

void runexe::showHelp() {
    showInfo();

    cout << endl
         << "This program runs other program for given period of time with specified" << endl
         << "  memory restrictions" << endl << endl
         << "See readme.md for help instructions" << endl;
}

void runexe::fail(const string &comment) {
    Configuration &configuration = Configuration::getConfiguration();

    if (configuration.isScreenOutput()) {
        cout << "Invocation failed" << endl
             << "Comment: " << comment << endl << endl
             << "Use \"runexe -h\" to get help information" << endl;
    }

    InvocationResult invocationResult = InvocationResult(FAIL, comment);

    if (configuration.isXmlOutput())
        printXmlInvocationResult(invocationResult, configuration.getXmlFileName());

    quit();
}

void runexe::crash(const string &comment) {
    Configuration &configuration = Configuration::getConfiguration();

    if (configuration.isScreenOutput()) {
        cout << "Invocation crashed" << endl
             << "Comment: " << comment << endl << endl
             << "Use \"runexe -h\" to get help information" << endl;
    }

    InvocationResult invocationResult = InvocationResult(CRASH, comment);

    if (configuration.isXmlOutput())
        printXmlInvocationResult(invocationResult, configuration.getXmlFileName());

    if (configuration.isReturnExitCode())
        quit();
    else
        quit(0);
}

InvocationParams runexe::processParams(const vector<string> &params) {
    if (params.size() > 1 && params[1] == "-h") {
        showHelp();
        quit();
    }

    return InvocationParams(params);
}

InvocationParams runexe::processCommandLine(int argc, char *argv[]) {
    if (argc <= 1) {
        showInfo();
        quit();
    }

    vector<string> params(argc);
    for (int i = 0; i < argc; ++i)
        params[i] = argv[i];

    return processParams(params);
}

void runexe::printTimes(double userTime, double kernelTime,
                        double timeLimit, bool isTimeLimitExceeded) {
    if (Configuration::getConfiguration().isShowKernelModeTime()) {
        if (isTimeLimitExceeded)
            cout << "  time consumed:" << endl
                 << "    user mode:   " << userTime << " of " << timeLimit << " sec" << endl
                 << "    kernel mode: " << kernelTime << " sec" << endl;
        else
            cout << "  time consumed:" << endl
                 << "    user mode:   " << userTime << " sec" << endl
                 << "    kernel mode: " << kernelTime << " sec" << endl;
    } else {
        if (isTimeLimitExceeded)
            cout << "  time consumed: " << userTime << " of " << timeLimit << " sec" << endl;
        else
            cout << "  time consumed: " << userTime << " sec" << endl;
    }
}

void runexe::printInvocationResult(const InvocationParams &invocationParams,
                                   const InvocationResult &invocationResult) {
    InvocationVerdict invocationVerdict = invocationResult.getInvocationVerdict();

    int exitCode = invocationResult.getExitCode();
    double userTime = (double) invocationResult.getUserTime() / 1000.0;
    double kernelTime = (double) invocationResult.getKernelTime() / 1000.0;
    double passedTime = (double) invocationResult.getPassedTime() / 1000.0;
    long long memory = invocationResult.getMemory();

    double timeLimit = (double) invocationParams.getTimeLimit() / 1000.0;
    long long memoryLimit = invocationParams.getMemoryLimit();

    cout.precision(2);
    cout.setf(ios::fixed);

    switch (invocationVerdict) {
        case SUCCESS :
            cout << "Program successfully terminated" << endl
                 << "  exit code:     " << exitCode << endl,
                    printTimes(userTime, kernelTime),
                    cout << "  time passed:   " << passedTime << " sec" << endl
                         << "  peak memory:   " << memory << " bytes" << endl;
            break;

        case FAIL :
            fail(invocationResult.getComment());
            break;

        case CRASH :
            crash(invocationResult.getComment());
            break;

        case TIME_LIMIT_EXCEEDED :
            cout << "Time limit exceeded" << endl
                 << "Program failed to terminate within " << timeLimit << " sec" << endl,
                    printTimes(userTime, kernelTime, timeLimit, true),
                    cout << "  time passed:   " << passedTime << " sec" << endl
                         << "  peak memory:   " << memory << " bytes" << endl;
            break;

        case MEMORY_LIMIT_EXCEEDED :
            cout << "Memory limit exceeded" << endl
                 << "Program tried to allocate more than " << memoryLimit << " bytes" << endl,
                    printTimes(userTime, kernelTime),
                    cout << "  time passed:   " << passedTime << " sec" << endl
                         << "  peak memory:   " << memory << " of " << memoryLimit << " bytes" << endl;
            break;

        case IDLENESS_LIMIT_EXCEEDED :
            cout << "Idleness limit exceeded" << endl
                 << "Detected program idle " << endl,
                    printTimes(userTime, kernelTime),
                    cout << "  time passed:   " << passedTime << " sec" << endl
                         << "  peak memory:   " << memory << " bytes" << endl;
            break;

        case SECURITY_VIOLATION :
            cout << "Security violation" << endl
                 << "Program tried to do some forbidden action" << endl,
                    printTimes(userTime, kernelTime),
                    cout << "  time passed:   " << passedTime << " sec" << endl
                         << "  peak memory:   " << memory << " bytes" << endl;
            break;

        default :
            fail("unknown invocation verdict");
    }
}

void runexe::printXmlInvocationResult(const InvocationResult &invocationResult,
                                      const string &fileName) {
    static bool firstCall = true;

    if (!firstCall)
        return;
    else
        firstCall = false;

    vector<string> result;
    result.push_back("<?xml version = \"1.1\" encoding = \"UTF-8\"?>");
    result.push_back("");
    result.push_back("<invocationResult>");
    result.push_back("    <invocationVerdict>" +
                     invocationVerdictToString(invocationResult.getInvocationVerdict()) +
                     "</invocationVerdict>");
    result.push_back("    <exitCode>" +
                     Strings::integerToString(invocationResult.getExitCode()) +
                     "</exitCode>");
    result.push_back("    <processorUserModeTime>" +
                     Strings::integerToString(invocationResult.getUserTime()) +
                     "</processorUserModeTime>");
    result.push_back("    <processorKernelModeTime>" +
                     Strings::integerToString(invocationResult.getKernelTime()) +
                     "</processorKernelModeTime>");
    result.push_back("    <passedTime>" +
                     Strings::integerToString(invocationResult.getPassedTime()) +
                     "</passedTime>");
    result.push_back("    <consumedMemory>" +
                     Strings::integerToString(invocationResult.getMemory()) +
                     "</consumedMemory>");
    result.push_back("    <comment>" + invocationResult.getComment() +
                     "</comment>");
    result.push_back("</invocationResult>");

    int resultSize = (int) result.size();

    if (!fileName.empty()) {

        FILE *file = fopen(fileName.c_str(), "wt");

        if (NULL != file) {
            for (int i = 0; i < resultSize; ++i)
                fprintf(file, "%s\n", result[i].c_str());

            fclose(file);
        } else {
            fail("can't open file '" + fileName + "'");
        }
    } else {
        for (int i = 0; i < resultSize; ++i)
            printf("%s\n", result[i].c_str());
    }
}
