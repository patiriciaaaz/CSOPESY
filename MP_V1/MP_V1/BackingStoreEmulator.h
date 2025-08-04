#pragma once

#include <memory>
#include <cstdint>
#include <unordered_map>
#include <iostream>

class Process;

class BackingStoreEmulator
{
public:
    BackingStoreEmulator();
    void store(std::shared_ptr<Process> proc);
    void load(std::shared_ptr<Process> proc);
    void log(uint32_t operationType, std::shared_ptr<Process> proc);

private:
    std::string backingStore;
    std::string backingStoreLogger;
};
