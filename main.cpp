#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include "multiplescreen.h"
#include "commands.h"


int initialize() {
    return 1;
}

#include "commands.h"
#include <iostream>
#include <string>

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
            parseAndExecuteCommand(commandLine, running);
        }
    }

    std::cout << "Exiting program.\n";
    return 0;
}
