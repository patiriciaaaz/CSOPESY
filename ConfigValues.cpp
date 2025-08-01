#include "ConfigValues.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

ConfigValues::ConfigValues(const std::string& filename) {
    parseConfigFile(filename);
}

void ConfigValues::parseConfigFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening config file: " << filename << "\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        iss >> key;

        if (key == "num-cpu") {
            iss >> nCPU;
        }
        else if (key == "scheduler") {
            iss >> scheduler;
            scheduler.erase(std::remove(scheduler.begin(), scheduler.end(), '"'), scheduler.end()); // remove quotes
        }
        else if (key == "quantum-cycles") {
            iss >> quantumCycles;
        }
        else if (key == "batch-process-freq") {
            iss >> batchProcessFreq;
        }
        else if (key == "min-ins") {
            iss >> minIns;
        }
        else if (key == "max-ins") {
            iss >> maxIns;
        }
        else if (key == "delay-per-exec") {
            iss >> delayPerExec;
        }
    }
}

uint32_t ConfigValues::getNCPU() const
{
    return nCPU;
}
const std::string& ConfigValues::getScheduler() const { return scheduler; }
uint32_t ConfigValues::getQuantumCycles() const { return quantumCycles; }
uint32_t ConfigValues::getBatchProcessFreq() const { return batchProcessFreq; }
uint32_t ConfigValues::getMinInstructions() const { return minIns; }
uint32_t ConfigValues::getMaxInstructions() const { return maxIns; }
uint32_t ConfigValues::getDelayPerExec() const { return delayPerExec; }
