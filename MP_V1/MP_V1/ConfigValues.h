#pragma once
#include <string>
#include <cstdint>
#include <unordered_map>
#include <iostream>

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

    uint32_t getMaxOverallMemory() const;
    uint32_t getMemoryPerFrame() const;
    uint32_t getMinMemPerProc() const;
    uint32_t getMaxMemPerProc() const;

private:
    uint32_t nCPU = 1;
    std::string scheduler = "fcfs";
    uint32_t quantumCycles = 0;
    uint32_t batchProcessFreq = 0;
    uint32_t minIns = 0;
    uint32_t maxIns = 0;
    uint32_t delayPerExec = 0;

    uint32_t maxOverallMemory = 0;
    uint32_t memoryPerFrame = 0;
    uint32_t minMemPerProc = 0;
    uint32_t maxMemPerProc = 0;

    void parseConfigFile(const std::string& filename);
};
