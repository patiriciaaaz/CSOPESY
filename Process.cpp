#include "Process.h"
#include <ctime>

// Constructor
Process::Process(const std::string& processName, std::vector<std::shared_ptr<CommandType>> commList, std::uint32_t pid)
    : pName(processName), commandList(std::move(commList)), pId(pid), 
      pCreationTime(std::time(nullptr)), pEndTime(std::time(nullptr)) {
    instructionLimit = static_cast<std::uint32_t>(commandList.size());
}

// Getters
const std::string& Process::getPName() const {
    return pName;
}

std::uint32_t Process::getPId() const {
    return pId;
}

std::uint32_t Process::getCurrentInstruction() const {
    return currentInstruction;
}

std::uint32_t Process::getInstructionLimit() const {
    return instructionLimit;
}

std::int32_t Process::getCPUIdWorkingOnIt() const {
    return cpuIdWorkingOnIt;
}

std::time_t Process::getPCreationTime() const {
    return pCreationTime;
}

std::time_t Process::getPEndTime() const {
    return pEndTime;
}

Process::ProcessState Process::getPState() const {
    return pState;
}

// Setters
void Process::setProcessState(ProcessState state) {
    pState = state;
}

void Process::setNewPEndTime() {
    pEndTime = std::time(nullptr);
}

void Process::assignCPUIdWorkingOnIt(int32_t cpuId) {
    cpuIdWorkingOnIt = cpuId;
}

// Execute current instruction
void Process::executeInstruction() {
    if (currentInstruction < instructionLimit) {
        commandList[currentInstruction]->executeCommand();
    }
}

// Advance to next instruction
void Process::nextInstruction() {
    if (currentInstruction < instructionLimit) {
        currentInstruction++;
    }
}

// Check if all instructions executed
bool Process::hasFinished() const {
    return currentInstruction >= instructionLimit;
}
