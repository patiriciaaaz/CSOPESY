#pragma once
#include <string>
#include <cstdint>
#include <unordered_map>

class ConfigValues {
public:
    ConfigValues(const std::string& filename = "config.txt");

    uint32_t getNCPU() const;
    const std::string& getScheduler() const;
    uint32_t getQuantumCycles() const;
    uint32_t getBatchProcessFreq() const;
    uint32_t getMinInstructions() const;
    uint32_t getMaxInstructions() const;
    uint32_t getDelayPerExec() const;

private:
    uint32_t nCPU = 1;
    std::string scheduler = "fcfs";
    uint32_t quantumCycles = 0;
    uint32_t batchProcessFreq = 0;
    uint32_t minIns = 0;
    uint32_t maxIns = 0;
    uint32_t delayPerExec = 0;

    void parseConfigFile(const std::string& filename);
};

