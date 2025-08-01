#include "MainManager.h"
#include <iostream>
#include <sstream>
#include "CommandTypeFactory.h"
#include <memory>
#include <algorithm>
#pragma once

#include <string>
#include <memory>
#include "CommandType.h"

class CommandTypeFactory {
public:
    static std::shared_ptr<CommandType> parseInstruction(const std::string& instr);
};

std::shared_ptr<CommandType> CommandTypeFactory::parseInstruction(const std::string& instr) {
    // Simple parsing logic (to be expanded)
    if (instr == "create") {
        return std::make_shared<CreateCommand>();
    }
    else if (instr == "delete") {
        return std::make_shared<DeleteCommand>();
    }
    // Add more command types as needed
    return nullptr;
}

MainManager::MainManager() {
}

void MainManager::initialize() {
    std::cout << "Initializing OS...\n";
    isInitialized = true;
    config = std::make_shared<ConfigValues>("config.txt");
    cpuScheduler = std::make_shared<CPUAndScheduler>(config, shared_from_this());
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

bool MainManager::isValidMemorySize(int size) {
    if (size < 64 || size > 65536) return false;
    return (size & (size - 1)) == 0;  // power of two check
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

std::vector<std::string> MainManager::splitInstructions(const std::string& instrStr) {
    std::vector<std::string> result;
    std::stringstream ss(instrStr);
    std::string token;

    while (std::getline(ss, token, ';')) {
        // Remove whitespace chars
        token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
        if (!token.empty()) {
            result.push_back(token);
        }
    }
    return result;
}


std::vector<std::string> MainManager::tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string word;
    while (iss >> word) {
        tokens.push_back(word);
    }
    return tokens;
}

void MainManager::run() {
    showMenu();

    while (true) {
        std::cout << "root:/> ";
        std::string line;
        std::getline(std::cin, line);
        if (!isInitialized) {
            if (line == "initialize") {
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

            if (input.size() == 1) {
                // existing single-command handling...
            }
            else if (input.size() == 2) {
                // existing two-command handling...
            }
            else if (input.size() >= 3) {
                // Combine first two tokens for the option, e.g. "screen -s"
                std::string cmdOption = input[0] + " " + input[1];

                if (cmdOption == "screen -s" || cmdOption == "screen -c") {
                    std::string procName = input[2];
                    if (screenExists(procName)) {
                        std::cout << "Screen '" << procName << "' already exists.\n\n";
                        continue;
                    }

                    // Parse memory size and validate
                    if (input.size() < 4) {
                        std::cout << "Invalid command format. Memory size missing.\n\n";
                        continue;
                    }

                    int memSize = 0;
                    try {
                        memSize = std::stoi(input[3]);
                    }
                    catch (...) {
                        std::cout << "Invalid memory size.\n\n";
                        continue;
                    }

                    if (!isValidMemorySize(memSize)) {
                        std::cout << "invalid memory allocation\n";
                        continue;
                    }

                    if (cmdOption == "screen -s") {
                        // Create process with random print commands, like before
                        std::vector<std::shared_ptr<CommandType>> commands;
                        std::uint32_t ins = cpuScheduler->getRandomNoOfInstructions();
                        for (std::size_t i = 0; i < ins; ++i) {
                            commands.push_back(std::make_shared<PrintCommand>());
                        }
                        auto newProc = std::make_shared<Process>(procName, commands, currentPId++);
                        cpuScheduler->addProcessToProcessLine(newProc);
                        registerScreen(procName, std::make_shared<ProcessConsole>(procName, newProc));
                        switchToScreen(procName);
                    }
                    else if (cmdOption == "screen -c") {
                        // Rebuild the instruction string (everything after the 4th token)
                        size_t pos = line.find(procName);
                        pos = line.find_first_of(" ", pos);
                        pos = line.find_first_not_of(" ", pos);
                        pos = line.find_first_of(" ", pos);
                        pos = line.find_first_not_of(" ", pos);

                        if (pos == std::string::npos) {
                            std::cout << "No instructions provided.\n\n";
                            continue;
                        }

                        std::string instrStr = line.substr(pos);

                        // Split instructions by ';' and parse to commands
                        std::vector<std::string> instrStrings = splitInstructions(instrStr);
                        if (instrStrings.size() < 1 || instrStrings.size() > 50) {
                            std::cout << "invalid command\n";
                            continue;
                        }

                        std::vector<std::shared_ptr<CommandType>> commands;
                        bool invalidInstructionFound = false;
                        for (const auto& instr : instrStrings) {
                            auto cmd = CommandTypeFactory::parseInstruction(instr);
                            if (!cmd) {
                                std::cout << "Invalid instruction: " << instr << "\n\n";
                                invalidInstructionFound = true;
                                break;
                            }
                            commands.push_back(cmd);
                        }

                        if (invalidInstructionFound)
                            continue;

                        auto newProc = std::make_shared<Process>(procName, commands, currentPId++);
                        cpuScheduler->addProcessToProcessLine(newProc);
                        registerScreen(procName, std::make_shared<ProcessConsole>(procName, newProc));
                        switchToScreen(procName);
                    }
                }
                else {
                    std::cout << "Command not found." << std::endl << std::endl;
                }
            }
            else if (input.size() == 0) {
                // do nothing
            }
        }
    }
}



