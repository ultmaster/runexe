#include <cstdio>
#include <unistd.h>
#include <signal.h>

#include "Interaction.h"
#include "Run.h"
#include "Configuration.h"

namespace interaction {

    bool failed = false;
    std::string failMessage = "";
    pthread_mutex_t mutex_lock;
    int pipeFds[8];

    void error(const std::string &message) {
        pthread_mutex_lock(&mutex_lock);
        failed = true;
        failMessage = message;
        pthread_mutex_unlock(&mutex_lock);
    }

    StreamProxyConfiguration::StreamProxyConfiguration(const std::string &firstProcessName,
                                                       const std::string &secondProcessName,
                                                       const std::string &recordFileName, int inputStreamFd,
                                                       int outputStreamFd) : firstProcessName(firstProcessName),
                                                                             secondProcessName(secondProcessName),
                                                                             recordFileName(recordFileName),
                                                                             inputStreamFd(inputStreamFd),
                                                                             outputStreamFd(outputStreamFd) {}

    void *streamProxyRun(void *arg) {
        StreamProxyConfiguration *configuration = (StreamProxyConfiguration *) arg;
        FILE* recordFile = NULL;
        if (!configuration->recordFileName.empty())
            recordFile = fopen(configuration->recordFileName.c_str(), "w");

        const int bufferSize = 65536;
        char buffer[bufferSize];
        while (true) {
            ssize_t len;
            len = read(configuration->inputStreamFd, buffer, bufferSize);
            if (len == -1) {
                error("Unexpected exception while reading from the output stream of " +
                      configuration->firstProcessName);
                break;
            }

            if (len == 0) break;

            if (recordFile != NULL) {
                if (fwrite(buffer, sizeof(char), len, recordFile) != len) {
                    error("Unexpected exception while recording the output stream to file from " +
                          configuration->firstProcessName);
                }
            }

            if (write(configuration->outputStreamFd, buffer, len) != len) {
                error("Unexpected exception while writing to the input stream of " + configuration->secondProcessName);
                break;
            }
        }

        fclose(recordFile);
        close(configuration->inputStreamFd);
        close(configuration->outputStreamFd);
        return NULL;
    }

    void pipeHelper(int &writeEnd, int &readEnd) {
        int fd[2];
        if (pipe(fd) == -1) {
            runexe::crash("Unexpected exception: pipe failed");
        } else {
            readEnd = fd[0];
            writeEnd = fd[1];
        }
    }

    struct InvocationThreadStruct {
        runexe::InvocationParams params;
        runexe::InvocationResult result;

        InvocationThreadStruct(const runexe::InvocationParams &params) : params(params) {}
    };

    void *runProcessHelper(void *arg) {
        using namespace runexe;
        InvocationThreadStruct *threadStruct = (InvocationThreadStruct *) arg;
        InvocationParams *invocationParams = &(threadStruct->params);
        ProcessParams params = invocationParams->asProcessParams();
        int readFd = pipeFds[invocationParams->getProgramName() == "program" ? 7 : 3];
        int writeFd = pipeFds[invocationParams->getProgramName() == "program" ? 0 : 4];
        for (int i = 0; i < 8; ++i)
            if (pipeFds[i] != readFd && pipeFds[i] != writeFd)
                params.closing_fds.push_back(pipeFds[i]);

        params.input_fd = readFd;
        params.output_fd = writeFd;

        ProcessOutcome outcome = run(params);
        threadStruct->result = InvocationResult(outcome);

        return NULL;
    }

    std::vector<runexe::InvocationResult> interact(runexe::InvocationParams paramProgram,
                                                   runexe::InvocationParams paramInteractor) {
        pthread_mutex_init(&mutex_lock, NULL);

        /* https://stackoverflow.com/questions/19020857/how-to-check-if-the-pipe-is-opend-before-writing */
        if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
            runexe::crash("Failed to ignore SIGPIPE");

        pipeHelper(pipeFds[0], pipeFds[1]);     // program process -> proxy
        pipeHelper(pipeFds[2], pipeFds[3]);     // proxy -> interactor process
        pipeHelper(pipeFds[4], pipeFds[5]);     // interactor process -> proxy
        pipeHelper(pipeFds[6], pipeFds[7]);     // proxy -> program process

        runexe::Configuration& configuration = runexe::Configuration::getConfiguration();

        StreamProxyConfiguration firstProxy = StreamProxyConfiguration("program", "interactor",
                                                                       configuration.getInteractorRecordOutput(),
                                                                       pipeFds[1], pipeFds[2]);
        StreamProxyConfiguration secondProxy = StreamProxyConfiguration("interactor", "program",
                                                                        configuration.getInteractorRecordInput(),
                                                                        pipeFds[5], pipeFds[6]);

        InvocationThreadStruct programStruct(paramProgram), interactorStruct(paramInteractor);

        pthread_t proxy1, proxy2, local1, local2;

        if (pthread_create(&local1, NULL, runProcessHelper, &programStruct) != 0) {
            runexe::crash("Thread local 1 failed to create");
        }

        if (pthread_create(&local2, NULL, runProcessHelper, &interactorStruct) != 0) {
            runexe::crash("Thread local 2 failed to create");
        }

        if (pthread_create(&proxy1, NULL, streamProxyRun, &firstProxy) != 0) {
            runexe::crash("Thread proxy 1 failed to create");
        }

        if (pthread_create(&proxy2, NULL, streamProxyRun, &secondProxy) != 0) {
            runexe::crash("Thread proxy 2 failed to create");
        }

        if (pthread_join(local1, NULL) != 0) {
            runexe::crash("Thread local 1 failed to join");
        }

        if (pthread_join(local2, NULL) != 0) {
            runexe::crash("Thread local 2 failed to join");
        }

        if (pthread_join(proxy1, NULL) != 0) {
            runexe::crash("Thread proxy 1 failed to join");
        }

        if (pthread_join(proxy2, NULL) != 0) {
            runexe::crash("Thread proxy 2 failed to join");
        }

        pthread_mutex_destroy(&mutex_lock);

        interactorStruct.result.setProgramName("interactor");

        std::vector<runexe::InvocationResult> result;
        result.push_back(programStruct.result);
        result.push_back(interactorStruct.result);

        if (failed) {
            runexe::crash(failMessage);
        }

        return result;
    }
}
