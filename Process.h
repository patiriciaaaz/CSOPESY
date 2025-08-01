#pragma once
#include <string>
#include "CommandType.h"
#include <vector>
#include <memory>
#include <ctime>
#include <cstdint>

class Process {
public:
    enum ProcessState {
        WAITING,
        WORKING,
        PAUSED,
        FINISHED
    };

    Process(const std::string& processName, std::vector<std::shared_ptr<CommandType>> commList, std::uint32_t pid);

    const std::string& getPName() const;
    std::uint32_t getPId() const;
    std::uint32_t getCurrentInstruction() const;
    std::uint32_t getInstructionLimit() const;
    std::int32_t getCPUIdWorkingOnIt() const;
    std::time_t getPCreationTime() const;
    std::time_t getPEndTime() const;
    ProcessState getPState() const;
    void setProcessState(ProcessState state);
    void setNewPEndTime();
    void assignCPUIdWorkingOnIt(int32_t cpuId);

    void executeInstruction();
    void nextInstruction();
    bool hasFinished() const;

private:
    std::string pName;
    std::uint32_t pId;
    std::uint32_t currentInstruction = 0;
    std::uint32_t instructionLimit = 0;
    std::time_t pCreationTime;
    std::time_t pEndTime;
    std::vector<std::shared_ptr<CommandType>> commandList;
    std::int32_t cpuIdWorkingOnIt = -1;
    ProcessState pState = WAITING;
};
