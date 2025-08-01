#include <iostream>
#include <sstream>
#include "CommandHandler.h"
#include "MainManager.h"

// Assume you have a global or accessible instance of MainManager, e.g.:
extern std::shared_ptr<MainManager> mainManager;

void handleScreenCommand(const std::string& commandLine) {
    std::stringstream ss(commandLine);
    std::string screenCmd, option, procName, memSizeStr;

    ss >> screenCmd >> option >> procName >> memSizeStr;

    if (option == "-s") {
        int memSize = std::stoi(memSizeStr);
        mainManager->createProcessWithoutInstructions(procName, memSize);
    } 
    else if (option == "-c") {
        // Extract the rest of the line as instructions string
        std::string instrStr;
        std::getline(ss, instrStr);
        instrStr.erase(0, instrStr.find_first_not_of(" \""));
        instrStr.erase(instrStr.find_last_not_of(" \"") + 1);

        int memSize = std::stoi(memSizeStr);
        mainManager->createProcessWithInstructions(procName, memSize, instrStr);
    } 
    else {
        std::cout << "Unknown screen command\n";
    }
}
