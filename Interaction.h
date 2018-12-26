#ifndef RUNEXE_INTERACTION_H
#define RUNEXE_INTERACTION_H

#include <string>
#include <vector>
#include <pthread.h>

#include "InvocationParams.h"
#include "InvocationResult.h"

namespace interaction {

    struct StreamProxyConfiguration {
        std::string firstProcessName;
        std::string secondProcessName;
        std::string recordFileName;
        int inputStreamFd;
        int outputStreamFd;

        StreamProxyConfiguration(const std::string &firstProcessName, const std::string &secondProcessName,
                                 const std::string &recordFileName, int inputStreamFd, int outputStreamFd);
    };

    void *streamProxyRun(void *arg);

    std::vector<runexe::InvocationResult> interact(runexe::InvocationParams paramProgram,
                                                   runexe::InvocationParams paramInteractor);
}

#endif //RUNEXE_INTERACTION_H
