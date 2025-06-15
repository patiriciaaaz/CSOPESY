#include <iostream>
#include <string>
#include <sstream>
#include "multiplescreen.h"
#include "commands.h"
#include "BarebonesProcess.h"

int main() {
    bool running = true;
    bool initialized = false;
    showMenu();

    while (running) {
        std::cout << "root:/> ";
        std::string commandLine;
        std::getline(std::cin, commandLine);

        if (!initialized) {
            if (commandLine == "initialize") {
                if (readConfigFile("config.txt", currentConfig)) {
                    initialized = true;
                    std::cout << "\nProgram successfully initialized.\n\n";
                } else {
                    std::cout << "Failed to initialize program. Check config.txt.\n";
                }
            } else {
                std::cout << "initialize the program first\n";
            }
        } else {
            std::istringstream iss(commandLine);
            std::string cmd, processName;
            iss >> cmd >> processName;

            if (cmd == "barebones-create") {
                if (!processName.empty()) {
                    createBarebonesProcess(processName);
                } else {
                    std::cout << "Usage: barebones-create <process_name>\n";
                }
            } else {
                parseAndExecuteCommand(commandLine, running);
            }
        }
    }

    std::cout << "Exiting program.\n";
    return 0;
}
