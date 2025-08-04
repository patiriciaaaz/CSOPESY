#include "MainManager.h"
#include <iostream>
#include <sstream>
#include <algorithm>

MainManager::MainManager() {
}

void MainManager::initialize() {
    std::cout << "Initializing OS...\n";
    isInitialized = true;
    config = std::make_shared<ConfigValues>("config.txt");
    frameSize = config->getMemoryPerFrame();
    auto bs = std::make_shared<BackingStoreEmulator>();
    auto malloc = std::make_shared<PagingMemoryAllocator>(config->getMaxOverallMemory(), config->getMemoryPerFrame());
    memoryAllocator = malloc;
    cpuScheduler = std::make_shared<CPUAndScheduler>(config, shared_from_this(), bs, malloc);
    malloc->schedulerAccess(cpuScheduler);
}

void MainManager::run() {
    showMenu();
    while (true) {
        std::cout << "root:/> ";
        std::string line;
        std::getline(std::cin, line);
        if (!isInitialized) {
            if (line == "in" || line == "initialize") {
                initialize();
                system("cls");
                showMenu();
            }
            else {
                std::cout << "Please initialize the OS." << std::endl << std::endl;
            }
        }
        else {
            std::vector<std::string> input = tokenize(line);

            /*        for (size_t i = 0; i < input.size(); ++i) {
                        std::cout << "Token " << i << ": " << input[i] << std::endl;
                    }*/


            if (input.size() == 1) {
                if (input[0] == "1") {
                    /*config->printConfigParameters();*/
                    cpuScheduler->printCPUReport();
               /*     memoryAllocator->visualizeAllocatedProcesses();
                    memoryAllocator->advancedStatistics();*/
                }
                else if (input[0] == "marquee") {
                    std::cout << "marquee" << std::endl << std::endl;
                }
                else if (input[0] == "exit") {
                    exit(0);
                }
                else if (input[0] == "clear") {
                    system("cls");
                    showMenu();
                }
                else if (input[0] == "report-util") {
                    cpuScheduler->printCPUReportInText();
                }
                else if (input[0] == "report-util") {
                    cpuScheduler->printCPUReportInText();
                }
                else if (input[0] == "vmstat") {
                    memoryAllocator->advancedStatistics();
                }
                else if (input[0] == "process-smi") {
                    memoryAllocator->visualizeAllocatedProcesses();
                }
                else if (input[0] == "s" || input[0] == "scheduler-start") {
                    cpuScheduler->schedulerStart();
                }
                else if (input[0] == "ss" || input[0] == "scheduler-stop") {
                    cpuScheduler->schedulerStop();
                }
                else {
                    std::cout << "Command not found." << std::endl << std::endl;
                }
            }
            else if (input.size() == 2) {
                if (input[0] + " " + input[1] == "screen -ls") {
                    cpuScheduler->printCPUReport();
                }
                else {
                    std::cout << "Command not found." << std::endl << std::endl;
                }
            }
            else if (input.size() == 3) {
                if (input[0] + " " + input[1] == "screen -s") {
                    std::string name = input[2];

                    if (screenExists(name)) {
                        std::cout << "Screen '" << name << "' already exists.\n\n";
                    }
                    else {
                        std::vector<std::shared_ptr<CommandType>> commands;
                        std::uint32_t ins = cpuScheduler->getRandomNoOfInstructions();
                        for (std::size_t i = 0; i < ins; ++i) {
                            commands.push_back(std::make_shared<Type>(true));
                        }
                        std::shared_ptr<Process> newProc = std::make_shared<Process>(name, commands, currentPId, getRandomProcessSize(), frameSize);

                        cpuScheduler->addProcessToProcessLine(newProc);

                        registerScreen(name, std::make_shared<ProcessConsole>(name, newProc));
                        switchToScreen(name);
                        currentPId++;
                    }
                }
                else if (input[0] + " " + input[1] == "screen -r") {
                    std::string name = input[2];

                    if (screenExists(name)) {
                        auto screen = screenRegistry.at(name);
                        auto procScreen = std::dynamic_pointer_cast<ProcessConsole>(screen);
                        if (procScreen) {
                            auto proc = procScreen->getProcess();
                            if (proc->hasMemoryError()) {
                                std::cout << proc->getMemoryErrorMessage() << std::endl;
                            }
                            else if (proc->hasFinished()) {
                                std::cout << "Screen '" << name << "' not found.\n\n";
                            }
                            else {
                                switchToScreen(name);
                            }
                        }
                    }
                    else {
                        std::cout << "Screen '" << name << "' not found.\n\n";
                    }
                }
                else {
                    std::cout << "Command not found." << std::endl << std::endl;
                }
            }
            else if (input.size() == 4) {
                if (input[0] + " " + input[1] == "screen -s") {
                    std::string name = input[2];

                    if (screenExists(name)) {
                        std::cout << "Screen '" << name << "' already exists.\n\n";
                    }
                    else {
                        uint32_t processMemorySize = 0;
                        try {
                            processMemorySize = std::stoul(input[3]);
                        }
                        catch (...) {
                            std::cout << "Invalid process memory size. (64-65536 powers of 2 only)" << std::endl;
                            continue;
                        }

                        if (!isValidProcessSize(processMemorySize)) {
                            std::cout << "Invalid process memory size. (64-65536 powers of 2 only)" << std::endl;
                            continue;
                        }

                        std::vector<std::shared_ptr<CommandType>> commands;
                        std::uint32_t ins = cpuScheduler->getRandomNoOfInstructions();
                        for (std::size_t i = 0; i < ins; ++i) {
                            commands.push_back(std::make_shared<Type>(true));
                        }
                        std::shared_ptr<Process> newProc = std::make_shared<Process>(name, commands, currentPId, processMemorySize, frameSize);

                        cpuScheduler->addProcessToProcessLine(newProc);

                        registerScreen(name, std::make_shared<ProcessConsole>(name, newProc));
                        switchToScreen(name);
                        currentPId++;
                    }
                }
                else if (input[0] + " " + input[1] == "screen -c") {
                    std::string name = input[2];
                    std::string commandString = input[3];
                    auto parsedCommandString = parseCommandString(commandString);

                    if (screenExists(name)) {
                        std::cout << "Screen '" << name << "' already exists.\n\n";
                    }
                    else {
                        std::shared_ptr<Process> newProc = std::make_shared<Process>(name, parsedCommandString, currentPId, getRandomProcessSize(), frameSize);

                        cpuScheduler->addProcessToProcessLine(newProc);

                        registerScreen(name, std::make_shared<ProcessConsole>(name, newProc));
                        switchToScreen(name);
                        currentPId++;
                    }
                }
            }
            else if (input.size() == 5) {
                if (input[0] + " " + input[1] == "screen -c") {
                    std::string name = input[2];
                    std::string commandString = input[4];
                    auto parsedCommandString = parseCommandString(commandString);

                    if (screenExists(name)) {
                        std::cout << "Screen '" << name << "' already exists.\n\n";
                    }
                    else {
                        uint32_t processMemorySize = 0;
                        try {
                            processMemorySize = std::stoul(input[3]);
                        }
                        catch (...) {
                            std::cout << "Invalid process memory size. (64-65536 powers of 2 only)" << std::endl;
                            continue;
                        }

                        if (!isValidProcessSize(processMemorySize)) {
                            std::cout << "Invalid process memory size. (64-65536 powers of 2 only)" << std::endl;
                            continue;
                        }

                        std::shared_ptr<Process> newProc = std::make_shared<Process>(name, parsedCommandString, currentPId, processMemorySize, frameSize);

                        cpuScheduler->addProcessToProcessLine(newProc);

                        registerScreen(name, std::make_shared<ProcessConsole>(name, newProc));
                        switchToScreen(name);
                        currentPId++;
                    }
                }

            }
            else if (input.size() == 0) {
                std::cout;
            }
        }
    }
}

void MainManager::showMenu()
{
    std::cout << R"(
 ,-----. ,---.   ,-----. ,------. ,------. ,---.,--.   ,--. 
'  .--./'   .-' '  .-.  '|  .--. '|  .---''   .-'\  `.'  /  
|  |    `.  `-. |  | |  ||  '--' ||  `--, `.  `-. '.    /   
'  '--'\.-'    |'  '-'  '|  | --' |  `---..-'    |  |  |    
 `-----'`-----'  `-----' `--'     `------'`-----'   `--'    
    )" << std::endl;
}

void MainManager::registerScreen(const std::string& name, std::shared_ptr<ConsoleFormat> screen) {
    screenRegistry[name] = screen;
}

bool MainManager::switchToScreen(const std::string& name) {
    auto it = screenRegistry.find(name);
    if (it != screenRegistry.end()) {
        activeScreen = it->second;
        system("cls");
        activeScreen->onEnabled();
        activeScreen->process();
        activeScreen = nullptr;
        system("cls");
        showMenu();  
        return true;
    }
    else {
        std::cout << "Screen not found: " << name << "\n";
        return false;
    }
}

void MainManager::listScreens() const {
    std::stringstream ss;

    if (screenRegistry.empty()) {
        ss << "No screens available.\n";
    }
    else {
        ss << "Registered Screens:\n";
        for (const auto& pair : screenRegistry) {
            ss << "- " << pair.first << "\n";
        }
    }

    ss << "\n";

    std::cout << ss.str();
}

bool MainManager::screenExists(const std::string& name) const {
    return screenRegistry.find(name) != screenRegistry.end();
}

std::vector<std::vector<std::string>> MainManager::parseCommandString(const std::string& raw)
{
    std::vector<std::vector<std::string>> parsedCommands;

    // Step 1: Remove the surrounding quotes if they exist
    std::string trimmed = raw;
    if (!trimmed.empty() && trimmed.front() == '"' && trimmed.back() == '"') {
        trimmed = trimmed.substr(1, trimmed.length() - 2);
    }

    // Step 2: Split by semicolons
    std::istringstream stream(trimmed);
    std::string command;
    while (std::getline(stream, command, ';')) {
        // Step 3: Trim leading/trailing spaces
        command.erase(0, command.find_first_not_of(" \t\n\r"));
        command.erase(command.find_last_not_of(" \t\n\r") + 1);

        if (command.empty()) continue;

        // Step 4: Special handling for PRINT
        if (command.rfind("PRINT(", 0) == 0) {
            std::vector<std::string> tokens;
            tokens.push_back("PRINT");

            size_t startQuote = command.find("\\\"");
            size_t endQuote = command.find("\\\"", startQuote + 2);

            if (startQuote != std::string::npos && endQuote != std::string::npos) {
                std::string literal = command.substr(startQuote + 2, endQuote - (startQuote + 2));
                tokens.push_back(literal);

                // Check for + varX
                size_t plusPos = command.find('+', endQuote);
                if (plusPos != std::string::npos) {
                    tokens.push_back("+");

                    // Find the variable after the +
                    size_t varStart = command.find_first_not_of(" \t", plusPos + 1);
                    size_t varEnd = command.find(')', varStart);
                    std::string variable = command.substr(varStart, varEnd - varStart);
                    tokens.push_back(variable);
                }
            }

            parsedCommands.push_back(tokens);
        }
        else {
            // Step 5: Regular space-split tokens
            std::istringstream cmdStream(command);
            std::string word;
            std::vector<std::string> tokens;
            while (cmdStream >> word) {
                tokens.push_back(word);
            }
            parsedCommands.push_back(tokens);
        }
    }

    return parsedCommands;
}

std::uint32_t MainManager::getRandomProcessSize()
{
    std::uint32_t minSize = config->getMinMemPerProc(); // e.g., 64
    std::uint32_t maxSize = config->getMaxMemPerProc(); // e.g., 65536

    // Build a list of all powers of 2 within the range
    std::vector<std::uint32_t> powerOfTwoSizes;
    for (std::uint32_t exp = 6; exp <= 16; ++exp) {
        std::uint32_t size = 1u << exp;
        if (size >= minSize && size <= maxSize) {
            powerOfTwoSizes.push_back(size);
        }
    }

    // Fallback in case no valid sizes were found
    if (powerOfTwoSizes.empty()) {
        return minSize; // or throw, or clamp to nearest valid power of 2
    }

    // Randomly select one of the valid powers of 2
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> dist(0, powerOfTwoSizes.size() - 1);

    return powerOfTwoSizes[dist(gen)];
}

bool MainManager::isValidProcessSize(std::uint32_t size)
{
    // Check if size is a power of 2 and within the range [64, 65536]
    return size >= 64 && size <= 65536 && (size & (size - 1)) == 0;
}




//std::vector<std::string> MainManager::tokenize(const std::string& line) {
//    std::vector<std::string> tokens;
//    std::istringstream iss(line);
//    std::string word;
//    while (iss >> word) {
//        tokens.push_back(word);
//    }
//    return tokens;
//}

std::vector<std::string> MainManager::tokenize(const std::string& line) {
    std::vector<std::string> tokens;

    std::istringstream iss(line);
    std::string word;
    std::string remainingLine = line;

    // Parse tokens until we find one starting with a quote
    size_t quoteStart = line.find('\"');
    if (quoteStart == std::string::npos) {
        // No quotes found, fall back to regular split
        while (iss >> word) {
            tokens.push_back(word);
        }
        return tokens;
    }

    // Split space-separated tokens up to the quoted string
    std::istringstream headStream(line.substr(0, quoteStart));
    while (headStream >> word) {
        tokens.push_back(word);
    }

    // Now capture everything from the first quote to the end
    std::string commandString = line.substr(quoteStart);
    tokens.push_back(commandString);

    return tokens;
}






