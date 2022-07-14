#include "Limiter.h"
#include <vector>

using namespace runexe;

std::vector<InvocationResult> Limiter::invoke(const InvocationParams &invocationParams) {

    Configuration &configuration = Configuration::getConfiguration();
    const std::string &interactorCommandLine = configuration.getInteractor();
    std::vector<InvocationResult> results;

    if (!interactorCommandLine.empty()) {
        // interactive mode

        InvocationParams interactorParams = InvocationParams(Strings::tokenizeCommandLine(interactorCommandLine));
        interactorParams.setProgramName("interactor");
        results = interaction::interact(invocationParams, interactorParams);

    } else {
        // normal mode

        ProcessParams params = invocationParams.asProcessParams();
        ProcessOutcome outcome = run(params);
        results.push_back(InvocationResult(outcome));

        if (invocationParams.getRedirectOutput().empty())
            remove(outcome.output_file.c_str());

        if (invocationParams.getRedirectError().empty())
            remove(outcome.error_file.c_str());
    }

    return results;

}
