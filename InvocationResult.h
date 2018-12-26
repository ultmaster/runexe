#ifndef _INVOCATION_RESULT_H_
#define _INVOCATION_RESULT_H_

#include "InvocationVerdict.h"
#include "Process.h"

#include <string>

namespace runexe {
    class InvocationResult {
    public:
        InvocationResult();

        InvocationResult(const InvocationVerdict &invocationVerdict,
                         const std::string &comment);

        InvocationResult(const ProcessOutcome& outcome);

        InvocationVerdict getInvocationVerdict() const;

        int getExitCode() const;

        long long getUserTime() const;

        long long getKernelTime() const;

        long long getMemory() const;

        long long getPassedTime() const;

        std::string getComment() const;

        std::string getProgramName() const;

        void setInvocationVerdict(const InvocationVerdict &verdict);

        void setExitCode(int exitCode);

        void setUserTime(long long userTime);

        void setKernelTime(long long kernelTime);

        void setMemory(long long memory);

        void setPassedTime(long long passedTime);

        void setComment(std::string comment);

        void setProgramName(std::string programName);

    private:
        InvocationVerdict verdict;
        int exitCode;
        long long userTime;
        long long kernelTime;
        long long memory;
        long long passedTime;
        std::string comment;
        std::string programName;

        void setDefaults();
    };
}

#endif
