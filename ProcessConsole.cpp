#include "ProcessConsole.h"

ProcessConsole::ProcessConsole(const std::string& name, std::shared_ptr<Process> proc)
    : screenName(name), proc(proc) {
}

void ProcessConsole::onEnabled() {
    isProcessConsoleActive = true;
    std::cout << "=== Process name: " << proc->getPName() << " ===\n";
    std::cout << "=== ID: " << proc->getPId() << " ===\n";
    std::cout << "=== Current instruction: " << proc->getCurrentInstruction() << " ===\n";
    std::cout << "=== Instruction size: " << proc->getInstructionLimit() << " ===\n";
    time_t creation = proc->getPCreationTime();
    tm creationTm{};
    localtime_s(&creationTm, &creation);
    std::cout << "=== Creation time: " << std::put_time(&creationTm, "%Y-%m-%d %H:%M:%S") << " ===\n\n";
}

void ProcessConsole::process() {
    std::string line;
    while (isProcessConsoleActive) {
        std::cout << "Process" << ":/> ";
        std::getline(std::cin, line);

        if (line == "exit") {
            isProcessConsoleActive = false;
        }
        else if (line == "") {
            std::cout;
        }
        else if (line == "a" || line == "process-smi") {
            if (!proc->hasFinished()) {
                std::cout << "=== Process name: " << proc->getPName() << " ===\n";
                std::cout << "=== ID: " << proc->getPId() << " ===\n";
                std::cout << "=== Current instruction: " << proc->getCurrentInstruction() << " ===\n";
                std::cout << "=== Instruction size: " << proc->getInstructionLimit() << " ===\n";
                time_t creation = proc->getPCreationTime();
                tm creationTm{};
                localtime_s(&creationTm, &creation);
                std::cout << "=== Creation time: " << std::put_time(&creationTm, "%Y-%m-%d %H:%M:%S") << " ===\n\n";
            }
            else {
                std::cout << "=== Process name: " << proc->getPName() << " ===\n";
                std::cout << "=== ID: " << proc->getPId() << " ===\n";
                time_t end = proc->getPEndTime();
                tm endTm{};
                localtime_s(&endTm, &end);
                std::cout << "=== Finished time: " << std::put_time(&endTm, "%Y-%m-%d %H:%M:%S") << " ===\n\n";
            }
        }
        else {
            std::cout << "Command not found.\n\n";
        }
    }
}

std::shared_ptr<Process> ProcessConsole::getProcess()
{
    return proc;
}
