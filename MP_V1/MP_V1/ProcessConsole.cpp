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
    std::cout << "=== Process size: " << proc->getPSize() << " ===\n";
    std::cout << "=== Number of pages: " << proc->getNPages() << " ===\n";
    std::cout << "=== Frame size: " << proc->getFSize() << " ===\n";
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
            if (proc->hasMemoryError()) {
                std::cout << "=== Process name: " << proc->getPName() << " ===\n";
                std::cout << "=== ID: " << proc->getPId() << " ===\n";
                std::cout << "=== Current instruction: " << proc->getCurrentInstruction() << " ===\n";
                std::cout << "=== Instruction size: " << proc->getInstructionLimit() << " ===\n";
                proc->printOutLogs();
                time_t creation = proc->getPCreationTime();
                tm creationTm{};
                localtime_s(&creationTm, &creation);
                std::cout << "=== Creation time: " << std::put_time(&creationTm, "%Y-%m-%d %H:%M:%S") << " ===\n\n";
                std::cout << proc->getMemoryErrorMessage() << std::endl;
            }
            else if (!proc->hasFinished()) {
                std::cout << "=== Process name: " << proc->getPName() << " ===\n";
                std::cout << "=== ID: " << proc->getPId() << " ===\n";
                std::cout << "=== Current instruction: " << proc->getCurrentInstruction() << " ===\n";
                std::cout << "=== Instruction size: " << proc->getInstructionLimit() << " ===\n";
                proc->printOutLogs();
                time_t creation = proc->getPCreationTime();
                tm creationTm{};
                localtime_s(&creationTm, &creation);
                std::cout << "=== Creation time: " << std::put_time(&creationTm, "%Y-%m-%d %H:%M:%S") << " ===\n\n";
             /*   proc->printVariableTableAndMemory();*/
            }
            else {
                std::cout << "=== Process name: " << proc->getPName() << " ===\n";
                std::cout << "=== ID: " << proc->getPId() << " ===\n";
                proc->printOutLogs();
                time_t end = proc->getPEndTime();
                tm endTm{};
                localtime_s(&endTm, &end);
                std::cout << "=== Finished time: " << std::put_time(&endTm, "%Y-%m-%d %H:%M:%S") << " ===\n\n";
              /*  proc->printVariableTableAndMemory();*/
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
