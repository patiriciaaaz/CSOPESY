#pragma once
#include <string>

#include <string>
#include <cstdint>

struct ConfigParams {
    int num_cpu;
    std::string scheduler;
    uint32_t quantum_cycles;
    uint32_t batch_process_freq;
    uint32_t min_ins;
    uint32_t max_ins;
    uint32_t delays_per_exec;
};

extern ConfigParams currentConfig;

void showMenu();
void parseAndExecuteCommand(const std::string& commandLine, bool& running);

bool readConfigFile(const std::string& filename, ConfigParams& config);
