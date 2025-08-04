#pragma once
#include <string>
#include "CommandType.h"
#include <vector>
#include <memory>
#include <ctime>
#include <unordered_map>

class Process {
public:

    enum ProcessState {
        WAITING,
        WORKING,
        PAUSED,
        FINISHED,
        MEMORY_ERROR
    };

    Process(const std::string& processName, std::vector<std::shared_ptr<CommandType>> commList, std::uint32_t pid, std::uint32_t pSize, std::uint32_t fSize);
    Process(const std::string& processName, std::vector<std::vector<std::string>> commString, std::uint32_t pid, std::uint32_t pSize, std::uint32_t fSize);

    const std::string& getPName() const;
    std::uint32_t getPId();
    std::uint32_t getCurrentInstruction() const;
    std::uint32_t getInstructionLimit() const;
    std::int32_t getCPUIdWorkingOnIt();
    std::time_t getPCreationTime();
    std::time_t getPEndTime();
    ProcessState getPState();
    void setProcessState(ProcessState state);
    void setNewPEndTime();
    void assignCPUIdWorkingOnIt(int32_t cpuId);

    void executeInstruction();
    void nextInstruction();
    bool hasFinished();
    bool hasMemoryError();
    std::string getMemoryErrorMessage();
    void stopDueToMemoryError(std::string invalidMemoryAddress);
    std::uint32_t getPSize();
    std::uint32_t getFSize();
    std::uint32_t getNPages();
    void printOutLogs();
    void log(std::string log);
    void createRespectiveCommandsFromCommandString();

    bool isVariableName(std::string);
    bool isMemoryAddress(std::string);
    bool isValue(std::string);
    std::unordered_map<std::string, std::int16_t> variableTable;
    std::unordered_map<std::string, std::int16_t> memory;

    void printVariableTableAndMemory();
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


    std::time_t pStopTime;
    std::uint32_t size;
    std::uint32_t frameSize;
    std::uint32_t nPages;
    std::vector<std::vector<std::string>> commandString;
    std::string memoryErrorMessage = "";
 
    std::vector<std::string> logs;
};
