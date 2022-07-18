#include <cstdio>
#include <cstdlib>

#include "../include/run.h"
#include "../include/interaction.h"
#include "../include/process.h"
#include "../include/strings.h"

using namespace runexe;
using namespace std;


int main(int argc, char *argv[]) {
    InvocationParams invocationParams = processCommandLine(argc, argv);

    Configuration &configuration = Configuration::getConfiguration();
    const string &interactorCommandLine = configuration.getInteractor();
    vector<InvocationResult> results;

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

    if (configuration.isScreenOutput()) {
        for (size_t i = 0; i < results.size(); ++i)
            printInvocationResult(invocationParams, results[i]);
    }

    if (configuration.isXmlOutput())
        printXmlInvocationResult(results, configuration.getXmlFileName());

    return EXIT_SUCCESS;
}

