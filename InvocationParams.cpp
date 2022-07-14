#include "InvocationParams.h"
#include "Configuration.h"
#include "Run.h"
#include "Strings.h"
#include <string>

using namespace runexe;
using namespace std;

void InvocationParams::setDefaults() {
    timeLimit = INFINITE_LIMIT_INT64;
    memoryLimit = INFINITE_LIMIT_INT64;
    redirectInput = "";
    redirectOutput = "";
    redirectError = "";
    homeDirectory = "";
    commandLine = "";
    userName = "";
    domain = "";
    password = "";
    injectDll = "";
    trustedProcess = false;
    idlenessChecking = true;
    programName = "program";
}

InvocationParams::InvocationParams(const vector<string> &cmdLineParams) {
    setDefaults();

    size_t tokensCount = cmdLineParams.size();

    Configuration &configuration = Configuration::getConfiguration();

    for (size_t currentTokenNumber = 0; currentTokenNumber < tokensCount; ++currentTokenNumber) {
        string currentToken = cmdLineParams[currentTokenNumber];

        if (currentToken == "-t") {
            if (currentTokenNumber == tokensCount - 1)
                crash("expected time limit value after \"-t\"");
            currentToken = cmdLineParams[++currentTokenNumber];
            timeLimit = parseTimeLimit(currentToken);
            continue;
        }

        if (currentToken == "-m") {
            if (currentTokenNumber == tokensCount - 1)
                crash("expected memory limit value after \"-m\"");
            currentToken = cmdLineParams[++currentTokenNumber];
            memoryLimit = parseMemoryLimit(currentToken);
            continue;
        }

        if (currentToken == "-d") {
            if (currentTokenNumber == tokensCount - 1)
                crash("expected directory after \"-d\"");
            currentToken = cmdLineParams[++currentTokenNumber];
            homeDirectory = currentToken;
            continue;
        }

        if (currentToken == "-i") {
            if (currentTokenNumber == tokensCount - 1)
                crash("expected file name after \"-i\"");
            currentToken = cmdLineParams[++currentTokenNumber];
            redirectInput = currentToken;
            continue;
        }

        if (currentToken == "-o") {
            if (currentTokenNumber == tokensCount - 1)
                crash("expected file name after \"-o\"");
            currentToken = cmdLineParams[++currentTokenNumber];
            redirectOutput = currentToken;
            continue;
        }

        if (currentToken == "-e") {
            if (currentTokenNumber == tokensCount - 1)
                crash("expected file name after \"-e\"");
            currentToken = cmdLineParams[++currentTokenNumber];
            redirectError = currentToken;
            continue;
        }

        if (currentToken == "-xml" || currentToken == "--xml") {
            configuration.setScreenOutput(false);
            configuration.setXmlOutput(true);
            continue;
        }
        
        if (currentToken == "-xmltof" || currentToken == "--xml-to-file") {
            configuration.setScreenOutput(false);
            configuration.setXmlOutput(true);
            if (currentTokenNumber == tokensCount - 1)
                crash("expected file name after \"" + currentToken + "\"");
            currentToken = cmdLineParams[++currentTokenNumber];
            configuration.setXmlFileName(currentToken);
            continue;
        }

        if (currentToken.find("--interactor=") == 0 || currentToken.find("-interactor=") == 0) {
            configuration.setInteractor(currentToken.substr(currentToken.find('=') + 1));
            continue;
        }

        if (currentToken.find("--ri=") == 0 || currentToken.find("-ri=") == 0) {
            configuration.setInteractorRecordInput(currentToken.substr(currentToken.find('=') + 1));
            continue;
        }

        if (currentToken.find("--ro=") == 0 || currentToken.find("-ro=") == 0) {
            configuration.setInteractorRecordOutput(currentToken.substr(currentToken.find('=') + 1));
            continue;
        }

        commandLine = buildCommandLine(cmdLineParams, currentTokenNumber);

        return;
    }

    crash("unexpected end of parameters");
}

string InvocationParams::buildCommandLine(const vector<string> &params, size_t fromIndex) {
    size_t paramsSize = params.size();
    string result = "";

    for (size_t currentTokenNumber = fromIndex; currentTokenNumber < paramsSize; ++currentTokenNumber) {
        string currentToken = params[currentTokenNumber];

        if (currentTokenNumber > fromIndex)
            result += " ";

        for (size_t i = 0; i < currentToken.length(); i++) {
            if (currentToken[i] <= ' ') {
                currentToken = "\"" + currentToken + "\"";
                break;
            }
        }

        result += currentToken;
    }

    return result;
}

long long InvocationParams::parseTimeLimit(const string &_s) {
    string s(_s);

    long long timeLimit;

    // In milliseconds (suffix "ms").
    if (s.length() > 2 && s.find("ms") == s.length() - 2) {
        timeLimit = Strings::parseInt64(s.substr(0, s.length() - 2));

        if (timeLimit < 0 || timeLimit >= INFINITE_LIMIT / 1000)
            crash("invalid time limit");
    } else {
        // In seconds (suffix "s").
        if (s.length() > 1 && s[s.length() - 1] == 's')
            s = s.substr(0, s.length() - 1);

        // In seconds (no suffix).
        timeLimit = Strings::parseInt64(s);

        if (timeLimit < 0 || timeLimit >= INFINITE_LIMIT)
            crash("invalid time limit");

        timeLimit *= 1000;
    }

    return timeLimit;
}

long long InvocationParams::parseMemoryLimit(const string &s) {
    long long memoryLimit;

    // In kibibytes (suffix "K").
    if (s.find("K") == s.length() - 1 || s.find("k") == s.length() - 1) {
        memoryLimit = Strings::parseInt64(s.substr(0, s.length() - 1));

        if (memoryLimit < 0 || memoryLimit >= INFINITE_LIMIT_INT64 / 1024)
            crash("invalid memory limit");

        memoryLimit *= 1024;
    }

    // In mebibytes (suffix "M").
    else if (s.find("M") == s.length() - 1 || s.find("m") == s.length() - 1) {
        memoryLimit = Strings::parseInt64(s.substr(0, s.length() - 1));

        if (memoryLimit < 0 || memoryLimit >= INFINITE_LIMIT_INT64 / 1024 / 1024)
            crash("invalid memory limit");

        memoryLimit *= 1024 * 1024;
    }

    // In gigabytes (suffix "G").
    else if (s.find("G") == s.length() - 1 || s.find("g") == s.length() - 1) {
        memoryLimit = Strings::parseInt64(s.substr(0, s.length() - 1));

        if (memoryLimit < 0 || memoryLimit >= INFINITE_LIMIT_INT64 / 1024 / 1024 / 1024)
            crash("invalid memory limit");

        memoryLimit *= 1024 * 1024 * 1024;
    }

    // In bytes (no suffix).
    else {
        memoryLimit = Strings::parseInt64(s);

        if (memoryLimit < 0 || memoryLimit >= INFINITE_LIMIT_INT64)
            crash("invalid memory limit");
    }

    return memoryLimit;
}

void InvocationParams::setTimeLimit(long long timeLimit) {
    this->timeLimit = timeLimit;
}


void InvocationParams::setMemoryLimit(long long memoryLimit) {
    this->memoryLimit = memoryLimit;
}

void InvocationParams::setRedirectInput(const std::string &inputFilePath) {
    this->redirectInput = inputFilePath;
}


void InvocationParams::setRedirectOutput(const std::string &outputFilePath) {
    this->redirectOutput = outputFilePath;
}

void InvocationParams::setRedirectError(const std::string &errorFilePath) {
    this->redirectError = errorFilePath;
}


void InvocationParams::setHomeDirectory(const std::string &homeDirectory) {
    this->homeDirectory = homeDirectory;
}

long long InvocationParams::getTimeLimit() const {
    return timeLimit;
}

long long InvocationParams::getMemoryLimit() const {
    return memoryLimit;
}

string InvocationParams::getRedirectInput() const {
    return redirectInput;
}

string InvocationParams::getRedirectOutput() const {
    return redirectOutput;
}

string InvocationParams::getRedirectError() const {
    return redirectError;
}

string InvocationParams::getHomeDirectory() const {
    return homeDirectory;
}

string InvocationParams::getCommandLine() const {
    return commandLine;
}

bool InvocationParams::isTrustedProcess() const {
    return trustedProcess;
}

string InvocationParams::getUserName() const {
    return userName;
}

string InvocationParams::getDomain() const {
    return domain;
}

string InvocationParams::getPassword() const {
    return password;
}

string InvocationParams::getInjectDll() const {
    return injectDll;
}

bool InvocationParams::isIdlenessChecking() const {
    return idlenessChecking;
}

ProcessParams InvocationParams::asProcessParams() const {
    ProcessParams params;
    params.command_line = getCommandLine();

    if (isIdlenessChecking())
        params.check_idleness = true;

    long long timeLimit = getTimeLimit();
    if (timeLimit != InvocationParams::INFINITE_LIMIT_INT64)
        params.time_limit = timeLimit;

    long long memoryLimit = getMemoryLimit();
    if (memoryLimit != InvocationParams::INFINITE_LIMIT_INT64)
        params.memory_limit = memoryLimit;

    string redirectInput = getRedirectInput();
    if (!redirectInput.empty())
        params.input_file = redirectInput;

    string redirectOutput = getRedirectOutput();
    if (!redirectOutput.empty())
        params.output_file = redirectOutput;

    string redirectError = getRedirectError();
    if (!redirectError.empty())
        params.error_file = redirectError;

    string homeDirectory = getHomeDirectory();
    if (!homeDirectory.empty())
        params.directory = homeDirectory;

    return params;
}

void InvocationParams::setProgramName(const string &programName) {
    InvocationParams::programName = programName;
}

std::string InvocationParams::getProgramName() const {
    return programName;
}
