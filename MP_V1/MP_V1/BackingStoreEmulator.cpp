#include "BackingStoreEmulator.h"
#include "Process.h"

#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>

BackingStoreEmulator::BackingStoreEmulator()
{
    backingStore = "csopesy-backing-store.txt";
    backingStoreLogger = "csopesy-backing-store-operation-logs.txt";

    // Clear both files at the start
    std::ofstream(backingStore, std::ios::trunc).close();
    std::ofstream(backingStoreLogger, std::ios::trunc).close();
}

void BackingStoreEmulator::store(std::shared_ptr<Process> proc)
{
    std::ofstream storeFile(backingStore, std::ios::app);
    if (storeFile.is_open()) {
        storeFile << proc->getPId() << " "
            << proc->getPName() << " "
            << proc->getCurrentInstruction() << " "
            << proc->getInstructionLimit() << " "
            << proc->getPSize() << " "
            << proc->getNPages() << "\n";
        storeFile.close();
    }

    log(1, proc); // Log as STORED
}

void BackingStoreEmulator::load(std::shared_ptr<Process> proc)
{
    std::ifstream inputFile(backingStore);
    std::ostringstream buffer;
    std::string line;
    uint32_t targetId = proc->getPId();

    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        uint32_t pid;
        iss >> pid;

        if (pid != targetId) {
            buffer << line << "\n";
        }
    }
    inputFile.close();

    std::ofstream outputFile(backingStore, std::ios::trunc);
    outputFile << buffer.str();
    outputFile.close();

    log(2, proc); // Log as LOADED
}

void BackingStoreEmulator::log(uint32_t operationType, std::shared_ptr<Process> proc)
{
    std::ofstream logFile(backingStoreLogger, std::ios::app);
    if (!logFile.is_open()) return;

    std::time_t now = std::time(nullptr);
    std::tm timeInfo{};
    localtime_s(&timeInfo, &now);

    std::string op = (operationType == 1) ? "STORED" : "LOADED";

    logFile << op << ": "
        << "PID=" << proc->getPId() << ", "
        << "Name=" << proc->getPName() << ", "
        << "CurrentInstruction=" << proc->getCurrentInstruction() << ", "
        << "Instructions=" << proc->getInstructionLimit() << ", "
        << "Size=" << proc->getPSize() << ", "
        << "Pages=" << proc->getNPages() << ", "
        << "Time=" << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S") << "\n";

    logFile.close();
}
