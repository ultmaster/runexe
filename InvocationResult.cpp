#include "InvocationResult.h"
#include "InvocationVerdict.h"
#include "Configuration.h"
#include "Run.h"

#include <string>

using namespace runexe;
using namespace std;

InvocationVerdict verdictByState(const ProcessState &state) {
    if (state == BEFORE || state == RUNNING || state == FAILED)
        return FAIL;

    if (state == EXITED)
        return SUCCESS;

    if (state == TIME_EXCEEDED)
        return TIME_LIMIT_EXCEEDED;

    if (state == MEMORY_EXCEEDED)
        return MEMORY_LIMIT_EXCEEDED;

    if (state == IDLENESS_EXCEEDED)
        return IDLENESS_LIMIT_EXCEEDED;

    fail("Unexpected state");
    return FAIL;
}

InvocationResult::InvocationResult() {
    setDefaults();
}

InvocationResult::InvocationResult(const InvocationVerdict &invocationVerdict,
                                   const string &comment) {
    if (invocationVerdict != CRASH && invocationVerdict != FAIL)
        fail("Crash/Fail InvocationResult constructor invoked for verdict " +
             invocationVerdictToString(invocationVerdict));

    setDefaults();

    verdict = invocationVerdict;
    this->comment = comment;
}

InvocationResult::InvocationResult(const ProcessOutcome &outcome) {
    setDefaults();
    exitCode = outcome.exit_code;
    comment = outcome.comment;
    verdict = verdictByState(outcome.state);
    userTime = outcome.time;
    memory = outcome.memory;
    passedTime = outcome.passed_time;
}

InvocationVerdict InvocationResult::getInvocationVerdict() const {
    return verdict;
}

int InvocationResult::getExitCode() const {
    return exitCode;
}

long long InvocationResult::getUserTime() const {
    return userTime;
}

long long InvocationResult::getKernelTime() const {
    return kernelTime;
}

long long InvocationResult::getMemory() const {
    return memory;
}

string InvocationResult::getComment() const {
    return comment;
}

long long InvocationResult::getPassedTime() const {
    return passedTime;
}

std::string InvocationResult::getProgramName() const {
    return programName;
}

void InvocationResult::setDefaults() {
    verdict = FAIL;
    exitCode = RUN_EXIT_FAILURE;
    userTime = 0;
    kernelTime = 0;
    memory = 0;
    passedTime = 0;
    comment = "";
    programName = "program";
}

void InvocationResult::setInvocationVerdict(const InvocationVerdict &verdict) {
    this->verdict = verdict;
}

void InvocationResult::setExitCode(int exitCode) {
    this->exitCode = exitCode;
}

void InvocationResult::setUserTime(long long userTime) {
    this->userTime = userTime;
}

void InvocationResult::setKernelTime(long long kernelTime) {
    this->kernelTime = kernelTime;
}

void InvocationResult::setMemory(long long memory) {
    this->memory = memory;
}

void InvocationResult::setPassedTime(long long passedTime) {
    this->passedTime = passedTime;
}

void InvocationResult::setComment(std::string comment) {
    this->comment = comment;
}

void InvocationResult::setProgramName(std::string programName) {
    this->programName = programName;
}
