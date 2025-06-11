#include "commands.h"
#include "multiplescreen.h"  
#include <iostream>
#include <cstdlib> 
#include <fstream>
#include <sstream>
#include <limits>

ConfigParams currentConfig;

bool readConfigFile(const std::string& filename, ConfigParams& config) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not open config file: " << filename << std::endl;
        return false;
    }

    std::string line;
    int num_cpu = -1;
    std::string scheduler;
    uint32_t quantum_cycles = 0, batch_process_freq = 0, min_ins = 0, max_ins = 0, delays_per_exec = 0;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        if (!(iss >> key)) {
            continue; 
        }
        if (key == "num-cpu") {
            if (!(iss >> num_cpu)) {
                std::cout << "Error: Invalid value for num-cpu." << std::endl;
                return false;
            }
        } else if (key == "scheduler") {
            if (!(iss >> scheduler)) {
                std::cout << "Error: Invalid value for scheduler." << std::endl;
                return false;
            }
            // Remove quotes if present
            if (scheduler.front() == '"' && scheduler.back() == '"') {
                scheduler = scheduler.substr(1, scheduler.size() - 2);
            }
        } else if (key == "quantum-cycles") {
            if (!(iss >> quantum_cycles)) {
                std::cout << "Error: Invalid value for quantum-cycles." << std::endl;
                return false;
            }
        } else if (key == "batch-process-freq") {
            if (!(iss >> batch_process_freq)) {
                std::cout << "Error: Invalid value for batch-process-freq." << std::endl;
                return false;
            }
        } else if (key == "min-ins") {
            if (!(iss >> min_ins)) {
                std::cout << "Error: Invalid value for min-ins." << std::endl;
                return false;
            }
        } else if (key == "max-ins") {
            if (!(iss >> max_ins)) {
                std::cout << "Error: Invalid value for max-ins." << std::endl;
                return false;
            }
        } else if (key == "delay-perexec") {
            if (!(iss >> delays_per_exec)) {
                std::cout << "Error: Invalid value for delay-perexec." << std::endl;
                return false;
            }
        } else {
            std::cout << "Warning: Unknown config key: " << key << std::endl;
        }
    }

    // Validate required parameters
    if (num_cpu == -1) {
        std::cout << "Error: num-cpu not specified." << std::endl;
        return false;
    }
    if (scheduler.empty()) {
        std::cout << "Error: scheduler not specified." << std::endl;
        return false;
    }
    if (min_ins == 0 || max_ins == 0) {
        std::cout << "Error: min-ins or max-ins not specified." << std::endl;
        return false;
    }

    // Validate ranges
    if (num_cpu < 1 || num_cpu > 128) {
        std::cout << "Error: num-cpu out of range [1,128]." << std::endl;
        return false;
    }
    if (scheduler != "fcfs" && scheduler != "rr") {
        std::cout << "Error: scheduler must be 'fcfs' or 'rr'." << std::endl;
        return false;
    }
    if (min_ins < 1 || max_ins < 1) {
        std::cout << "Error: min-ins and max-ins must be >= 1." << std::endl;
        return false;
    }
    if (min_ins > max_ins) {
        std::cout << "Error: min-ins cannot be greater than max-ins." << std::endl;
        return false;
    }

    // Assign to config struct
    config.num_cpu = num_cpu;
    config.scheduler = scheduler;
    config.quantum_cycles = quantum_cycles;
    config.batch_process_freq = batch_process_freq;
    config.min_ins = min_ins;
    config.max_ins = max_ins;
    config.delays_per_exec = delays_per_exec;

    return true;
}

void showMenu() {
    std::cout << R"(
 ,-----. ,---.   ,-----. ,------. ,------. ,---.,--.   ,--. 
'  .--./'   .-' '  .-.  '|  .--. '|  .---''   .-'\  `.'  /  
|  |    `.  `-. |  | |  ||  '--' ||  `--, `.  `-. '.    /   
'  '--'\.-'    |'  '-'  '|  | --' |  `---..-'    |  |  |    
 `-----'`-----'  `-----' `--'     `------'`-----'   `--'    
    )" << std::endl;

    std::cout << "=========================================================\n";
    std::cout << "Welcome to CSOPESY Emulator!\n\n";
    std::cout << "Developers:\n";
    std::cout << "Estrella, Elmeranita\n";
    std::cout << "Fulo, Rulet\n";
    std::cout << "Reinante, Christian\n";
    std::cout << "Zamora, Patricia\n\n";
    std::cout << "Last updated: 01/06/2025 \n";
    std::cout << "=========================================================\n";
}

void parseAndExecuteCommand(const std::string& commandLine, bool& running) {

    std::string cmd, option, name;
    size_t pos1 = commandLine.find(' ');
    if (pos1 == std::string::npos) {
        cmd = commandLine;
    } else {
        cmd = commandLine.substr(0, pos1);
        size_t pos2 = commandLine.find(' ', pos1 + 1);
        if (pos2 == std::string::npos) {
            option = commandLine.substr(pos1 + 1);
        } else {
            option = commandLine.substr(pos1 + 1, pos2 - pos1 - 1);
            name = commandLine.substr(pos2 + 1);
        }
    }

    if (cmd == "initialize"){
        return;
    }
 
    if (cmd == "exit") {
        running = false;
        return;
    }

    else if (cmd == "clear") {
        system("cls");  
        showMenu();
    }

    else if (cmd == "report-util") {
        std::cout << "Doing report_util\n";
    }

    else if (cmd == "scheduler-test") {
        std::cout << "Doing scheduler_test\n";
    }

    else if (cmd == "scheduler-stop") {
        std::cout << "Doing scheduler_stop\n";
    }

    else if (cmd == "screen") {
        if (option == "-s") {
            if (name == "") {
                std::cout << "Error: screen name required for -s option.\n";
                return;
            }
            if (findProcessIndex(name) != -1) {
                std::cout << "Screen with name '" << name << "' already exists.\n";
                return;
            }
            if (processCount >= MAX_PROCESSES) {
                std::cout << "Maximum number of screens reached.\n";
                return;
            }
            Process p;
            p.name = name;
            p.currentLine = 1;
            p.totalLines = 100;
            p.timestamp = getCurrentTimestamp();
            processes[processCount++] = p;
            screenConsole(name);
        } else if (option == "-r") {
            if (name == "") {
                std::cout << "Error: screen name required for -r option.\n";
                return;
            }
            if (findProcessIndex(name) == -1) {
                std::cout << "No screen with name '" << name << "' found.\n";
                return;
            }
            screenConsole(name);
        } else if (option == "-ls") {
            if (processCount == 0) {
                std::cout << "No running screens found.\n";
            } else {
                std::cout << "There are " << processCount << " screen(s) running:\n";
                for (int i = 0; i < processCount; ++i) {
                    std::cout << "\t" << processes[i].name << "\n";
                }
            }
        } else {
            std::cout << "Unknown option for screen command.\n";
        }
    } 
    
    else {
        std::cout << "Unknown command.\n";
    }
}
