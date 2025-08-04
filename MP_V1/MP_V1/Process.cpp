#include "Process.h"
#include "Type.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cctype>      
#include <string>
#include <regex>
#include <cstdint>

Process::Process(const std::string& processName, std::vector<std::shared_ptr<CommandType>> commList, std::uint32_t pid, std::uint32_t pSize, std::uint32_t fSize)
    : pName(processName), commandList(std::move(commList)), pId(pid), pCreationTime(std::time(nullptr)), pEndTime(std::time(nullptr)),
    size(pSize), frameSize(fSize){
    instructionLimit = static_cast<std::uint32_t>(commandList.size());
    nPages = (size + frameSize - 1) / frameSize;
    //log("lol");
    memory = {
    {"0x100", 0}, {"0x200", 0}, {"0x300", 0}, {"0x400", 0},
    {"0x500", 0}, {"0x600", 0}, {"0x700", 0}, {"0x800", 0},
    {"0x900", 0}, {"0x1000", 0}, {"0x1100", 0}, {"0x1200", 0},
    {"0x1300", 0}, {"0x1400", 0}, {"0x1500", 0}, {"0x1600", 0},

    {"0x1700", 0}, {"0x1800", 0}, {"0x1900", 0}, {"0x2000", 0},
    {"0x2100", 0}, {"0x2200", 0}, {"0x2300", 0}, {"0x2400", 0},
    {"0x2500", 0}, {"0x2600", 0}, {"0x2700", 0}, {"0x2800", 0},
    {"0x2900", 0}, {"0x3000", 0}, {"0x3100", 0}, {"0x3200", 0},

    {"0x3300", 0}, {"0x3400", 0}, {"0x3500", 0}, {"0x3600", 0},
    {"0x3700", 0}, {"0x3800", 0}, {"0x3900", 0}, {"0x4000", 0},
    {"0x4100", 0}, {"0x4200", 0}, {"0x4300", 0}, {"0x4400", 0},
    {"0x4500", 0}, {"0x4600", 0}, {"0x4700", 0}, {"0x4800", 0},

    {"0x4900", 0}, {"0x5000", 0}, {"0x5100", 0}, {"0x5200", 0},
    {"0x5300", 0}, {"0x5400", 0}, {"0x5500", 0}, {"0x5600", 0},
    {"0x5700", 0}, {"0x5800", 0}, {"0x5900", 0}, {"0x6000", 0},
    {"0x6100", 0}, {"0x6200", 0}, {"0x6300", 0}, {"0x6400", 0}
    };
}

Process::Process(const std::string& processName, std::vector<std::vector<std::string>> commList, std::uint32_t pid, std::uint32_t pSize, std::uint32_t fSize)
    : pName(processName), commandString(commList), pId(pid), pCreationTime(std::time(nullptr)), pEndTime(std::time(nullptr)), pStopTime(std::time(nullptr)),
    size(pSize), frameSize(fSize) {
    nPages = (size + frameSize - 1) / frameSize;
    
    memory = {
    {"0x100", 0}, {"0x200", 0}, {"0x300", 0}, {"0x400", 0},
    {"0x500", 0}, {"0x600", 0}, {"0x700", 0}, {"0x800", 0},
    {"0x900", 0}, {"0x1000", 0}, {"0x1100", 0}, {"0x1200", 0},
    {"0x1300", 0}, {"0x1400", 0}, {"0x1500", 0}, {"0x1600", 0},

    {"0x1700", 0}, {"0x1800", 0}, {"0x1900", 0}, {"0x2000", 0},
    {"0x2100", 0}, {"0x2200", 0}, {"0x2300", 0}, {"0x2400", 0},
    {"0x2500", 0}, {"0x2600", 0}, {"0x2700", 0}, {"0x2800", 0},
    {"0x2900", 0}, {"0x3000", 0}, {"0x3100", 0}, {"0x3200", 0},

    {"0x3300", 0}, {"0x3400", 0}, {"0x3500", 0}, {"0x3600", 0},
    {"0x3700", 0}, {"0x3800", 0}, {"0x3900", 0}, {"0x4000", 0},
    {"0x4100", 0}, {"0x4200", 0}, {"0x4300", 0}, {"0x4400", 0},
    {"0x4500", 0}, {"0x4600", 0}, {"0x4700", 0}, {"0x4800", 0},

    {"0x4900", 0}, {"0x5000", 0}, {"0x5100", 0}, {"0x5200", 0},
    {"0x5300", 0}, {"0x5400", 0}, {"0x5500", 0}, {"0x5600", 0},
    {"0x5700", 0}, {"0x5800", 0}, {"0x5900", 0}, {"0x6000", 0},
    {"0x6100", 0}, {"0x6200", 0}, {"0x6300", 0}, {"0x6400", 0}
    };
    createRespectiveCommandsFromCommandString();
    instructionLimit = static_cast<std::uint32_t>(commandList.size());
}

const std::string& Process::getPName() const {
    return pName;
}

std::uint32_t Process::getPId() {
    return pId;
}

std::uint32_t Process::getCurrentInstruction() const {
    return currentInstruction;
}

std::uint32_t Process::getInstructionLimit() const {
    return instructionLimit;
}

std::int32_t Process::getCPUIdWorkingOnIt() {
    return cpuIdWorkingOnIt;
}

std::time_t Process::getPCreationTime() {
    return pCreationTime;
}

std::time_t Process::getPEndTime() {
    return pEndTime;
}

Process::ProcessState Process::getPState() {
    return pState;
}

void Process::setProcessState(ProcessState state)
{
    pState = state;
}

void Process::setNewPEndTime()
{
    pEndTime = std::time(nullptr);
}

void Process::assignCPUIdWorkingOnIt(int32_t cpuId)
{
    cpuIdWorkingOnIt = cpuId;
}

void Process::executeInstruction() {
    if (currentInstruction < instructionLimit) {
        commandList[currentInstruction]->executeCommand(*this);
    }
}

void Process::nextInstruction() {
    if (currentInstruction < instructionLimit) {
        currentInstruction++;
    }
}

bool Process::hasFinished() {
    return commandList.size() == currentInstruction;
}

bool Process::hasMemoryError()
{
    return pState == Process::MEMORY_ERROR;
}

std::string Process::getMemoryErrorMessage()
{
    return memoryErrorMessage;
}

void Process::stopDueToMemoryError(std::string invalidMemoryAddress)
{
    pState = Process::MEMORY_ERROR;
    
    std::string processName = pName;

    pStopTime = std::time(nullptr);

    // Use localtime_s (thread-safe)
    std::tm timeInfo{};
    localtime_s(&timeInfo, &pStopTime);

    std::ostringstream timeStream;
    timeStream << std::put_time(&timeInfo, "%H:%M:%S");

    memoryErrorMessage = "Process " + processName +
        " shut down due to memory access violation error that occurred at " +
        timeStream.str() + ". " + invalidMemoryAddress + " invalid.\n";
}

std::uint32_t Process::getPSize()
{
    return size;
}

std::uint32_t Process::getFSize()
{
    return frameSize;
}

std::uint32_t Process::getNPages()
{
    return nPages;
}

void Process::printOutLogs()
{
    std::stringstream ss;
    ss << "Logs:" << std::endl;
    for (const auto& log : logs) {
        ss << log << std::endl;;
    }
    ss << std::endl;
    std::cout << ss.str();
}

void Process::log(std::string log)
{
    std::int32_t cpuID = cpuIdWorkingOnIt;
    std::string processName = pName;

    // Get current time
    std::time_t now = std::time(nullptr);
    std::tm localTime;
    localtime_s(&localTime, &now); // Windows-safe

    // Format time
    std::ostringstream timeStream;
    timeStream << std::put_time(&localTime, "(%m/%d/%Y %I:%M:%S %p)");

    // Compose final log
    std::ostringstream finalLog;
    finalLog << timeStream.str() << " Core:" << cpuID << " \"" << log << "\"";

    // Append to logs vector
    logs.push_back(finalLog.str());
}

void Process::createRespectiveCommandsFromCommandString()
{
    for (std::size_t i = 0; i < commandString.size(); ++i) {
        const std::vector<std::string>& commandPart = commandString[i];

        if (commandPart.size() == 3) {
            if (commandPart[0] == "DECLARE" && isVariableName(commandPart[1]) && isValue(commandPart[2])) {
                int16_t declareVal = static_cast<int16_t>(std::stoi(commandPart[2]));
                commandList.push_back(std::make_shared<Type>(commandPart[0], commandPart[1], declareVal));
            }
            else if (commandPart[0] == "WRITE" && isMemoryAddress(commandPart[1]) && isVariableName(commandPart[2])) {
                commandList.push_back(std::make_shared<Type>(commandPart[0], commandPart[1], commandPart[2], 0, true));
            }
            else if (commandPart[0] == "WRITE" && isMemoryAddress(commandPart[1]) && isValue(commandPart[2])) {
                int16_t writeVal = static_cast<int16_t>(std::stoi(commandPart[2]));
                commandList.push_back(std::make_shared<Type>(commandPart[0], commandPart[1], "noVar", writeVal, true));
            }
            else if (commandPart[0] == "READ" && isVariableName(commandPart[1]) && isMemoryAddress(commandPart[2])) {
                commandList.push_back(std::make_shared<Type>(commandPart[0], commandPart[1], commandPart[2], true));
            }
        }
        else if (commandPart.size() == 4) {
            if ((commandPart[0] == "ADD" || commandPart[0] == "SUBTRACT") && isVariableName(commandPart[1])) {
                if (isVariableName(commandPart[2]) && isVariableName(commandPart[3])) {
                    commandList.push_back(std::make_shared<Type>(commandPart[0], commandPart[1], commandPart[2], commandPart[3], -1, -1));
                }
                if (isValue(commandPart[2]) && isVariableName(commandPart[3])) {
                    int16_t valOp1 = static_cast<int16_t>(std::stoi(commandPart[2]));
                    commandList.push_back(std::make_shared<Type>(commandPart[0], commandPart[1], "noVarOp1", commandPart[3], valOp1, 0));
                }
                if (isVariableName(commandPart[2]) && isValue(commandPart[3])) {
                    int16_t valOp2 = static_cast<int16_t>(std::stoi(commandPart[3]));
                    commandList.push_back(std::make_shared<Type>(commandPart[0], commandPart[1], commandPart[2], "noVarOp2", 0, valOp2));
                }
                if (isValue(commandPart[2]) && isValue(commandPart[3])) {
                    int16_t valOp1 = static_cast<int16_t>(std::stoi(commandPart[2]));
                    int16_t valOp2 = static_cast<int16_t>(std::stoi(commandPart[3]));
                    commandList.push_back(std::make_shared<Type>(commandPart[0], commandPart[1], "noVarOps", "noVarOps", valOp1, valOp2));
                }
            }
            else if (commandPart[0] == "PRINT" && isVariableName(commandPart[3])) {
                commandList.push_back(std::make_shared<Type>(commandPart[0], commandPart[1], commandPart[3]));
            }
        }
        else if (commandPart.size() == 2) {
            if (commandPart[0] == "PRINT") {
                commandList.push_back(std::make_shared<Type>(commandPart[0], commandPart[1], "noVar"));
            }
        }
        else {
            commandList.push_back(std::make_shared<Type>(true));
        }
    }
}

bool Process::isVariableName(std::string token)
{
    if (token.empty()) return false;
    if (!std::isalpha(token[0]) && token[0] != '_') return false;

    for (char ch : token) {
        if (!std::isalnum(ch) && ch != '_') return false;
    }
    return true;
}

bool Process::isMemoryAddress(std::string token)
{
    return token.size() > 2 && token[0] == '0' && token[1] == 'x';
}

bool Process::isValue(std::string token)
{
    if (token.empty()) return false;

    // Handle optional negative sign
    size_t start = (token[0] == '-') ? 1 : 0;
    for (size_t i = start; i < token.size(); ++i) {
        if (!std::isdigit(token[i])) return false;
    }

    return true;
}

void Process::printVariableTableAndMemory()
{
    std::cout << "=== Variable Table ===" << std::endl;
    for (const auto& [varName, value] : variableTable) {
        std::cout << varName << " = " << value << std::endl;
    }

    std::cout << "\n=== Memory ===" << std::endl;
    for (const auto& [memAddress, value] : memory) {
        std::cout << memAddress << " = " << value << std::endl;
    }
}



