#ifndef _INVOCATION_PARAMS_H_
#define _INVOCATION_PARAMS_H_

#include <string>
#include <vector>
#include "process.h"

namespace runexe {
    class InvocationParams {
    public:
        static const int INFINITE_LIMIT = 2000000000;
        static const long long INFINITE_LIMIT_INT64 = 1000000000000000000LL;

        InvocationParams(const std::vector<std::string> &cmdLineParams);

        InvocationParams();

        long long getTimeLimit() const;

        void setTimeLimit(long long timeLimit);

        long long getMemoryLimit() const;

        void setMemoryLimit(long long memoryLimit);

        std::string getRedirectInput() const;
        
        void setRedirectInput(const std::string &inputFilePath);

        std::string getRedirectOutput() const;

        void setRedirectOutput(const std::string &outputFilePath);

        std::string getRedirectError() const;

        void setRedirectError(const std::string &errorFilePath);

        std::string getHomeDirectory() const;

        void setHomeDirectory(const std::string &homeDirectory);

        std::string getCommandLine() const;

        void setCommandLine(const std::string &commandLine);

        std::string getUserName() const;

        void setUserName(const std::string &userName);

        std::string getDomain() const;

        void setDomain(const std::string &domain);

        std::string getPassword() const;

        void setPassword(const std::string &password);

        std::string getInjectDll() const;

        void setInjectDll(const std::string &injectDll);

        std::string getProgramName() const;

        void setProgramName(const std::string &programName);

        bool isTrustedProcess() const;

        void setTrustedProcess(bool isTrustedProcess);

        bool isIdlenessChecking() const;

        void setIdlenessChecking(bool checkIdleness);

        ProcessParams asProcessParams() const;

    private:
        long long timeLimit;
        long long memoryLimit;
        std::string redirectInput;
        std::string redirectOutput;
        std::string redirectError;
        std::string homeDirectory;
        std::string commandLine;
        std::string userName;
        std::string domain;
        std::string password;
        std::string injectDll;
        std::string programName;
        bool trustedProcess;
        bool idlenessChecking;

    private:
        std::string buildCommandLine(const std::vector<std::string> &params,
                                     size_t fromIndex);

        long long parseTimeLimit(const std::string &s);

        long long parseMemoryLimit(const std::string &s);

        void setDefaults();
    };
}

#endif
