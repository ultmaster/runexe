#include <iostream>
#include "../include/limiter.h"
#include "../include/invocation_verdict.h"

int main() {
    InvocationParams params;
    params.setCommandLine("./while1.o");
    params.setTimeLimit(2000);

    std::vector<InvocationResult> verdict = Limiter::invoke(params);

    for (auto &result: verdict) {
        std::cout << "time: " << result.getPassedTime() << 
                     "\tmemory: " << result.getMemory() << 
                     "\tinvocationVerdict" << runexe::invocationVerdictToString(result.getInvocationVerdict()) << std::endl;
    }
}
