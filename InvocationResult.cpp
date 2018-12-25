#include "InvocationResult.h"
#include "InvocationVerdict.h"
#include "Configuration.h"
#include "Run.h"

#include <string>

using namespace runexe;
using namespace std;

InvocationResult::InvocationResult()
{
    setDefaults();
}

InvocationResult::InvocationResult(const InvocationVerdict& invocationVerdict,
                                   const string& comment)
{
    if (invocationVerdict != CRASH && invocationVerdict != FAIL)
        fail("Crash/Fail InvocationResult constructor invoked for verdict " +
                invocationVerdictToString(invocationVerdict));

    setDefaults();

    verdict = invocationVerdict;
    this->comment = comment;
}

InvocationVerdict InvocationResult::getInvocationVerdict() const
{
    return verdict;
}

int InvocationResult::getExitCode() const
{
    return exitCode;
}

int InvocationResult::getUserTime() const
{
    return userTime;    
}

int InvocationResult::getKernelTime() const
{
    return kernelTime;
}

long long InvocationResult::getMemory() const
{
    return memory;
}

string InvocationResult::getComment() const
{
    return comment;
}

int InvocationResult::getPassedTime() const
{
    return passedTime;
}

void InvocationResult::setDefaults()
{
    verdict = FAIL;
    exitCode = RUN_EXIT_FAILURE;
    userTime = 0;
    kernelTime = 0;
    memory = 0;
    passedTime = 0;
    comment = "";
}

void InvocationResult::setInvocationVerdict(const InvocationVerdict& verdict)
{
    this->verdict = verdict;
}

void InvocationResult::setExitCode(int exitCode)
{
    this->exitCode = exitCode;
}

void InvocationResult::setUserTime(int userTime)
{
    this->userTime = userTime;
}

void InvocationResult::setKernelTime(int kernelTime)
{
    this->kernelTime = kernelTime;
}

void InvocationResult::setMemory(long long memory)
{
    this->memory = memory;
}

void InvocationResult::setPassedTime(int passedTime)
{
    this->passedTime = passedTime;
}

void InvocationResult::setComment(std::string comment)
{
    this->comment = comment;
}
