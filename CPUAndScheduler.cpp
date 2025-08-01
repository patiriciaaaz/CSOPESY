#include "CPUAndScheduler.h"

CPUAndScheduler::CPUAndScheduler(std::shared_ptr<ConfigValues> cnfg, std::shared_ptr<MainManager> mm)
{
    config = cnfg;
    manager = mm;
    delayPerExec = config->getDelayPerExec() + 1;
    batchProcessFreq = config->getBatchProcessFreq();
    quantumCycles = config->getQuantumCycles();
    schedulingAlgo = config->getScheduler();

    std::uint32_t nCPU = config->getNCPU();

    std::thread([this]() {
        this->CPUScheduler();
        }).detach();

    for (std::uint32_t i = 0; i < nCPU; ++i) {
        cpuFreeMap[i] = true;
    }
}

void CPUAndScheduler::batchProcessor()
{
    std::uint32_t lastCreatedCycle = 0;

    while (schedulerStateFlag)
    {
        if ((cpuCycles - lastCreatedCycle) >= batchProcessFreq)
        {
            lastCreatedCycle = cpuCycles;

            std::uint32_t pid = manager->currentPId++;

            std::vector<std::shared_ptr<CommandType>> commands;

            commands.clear();

            std::uint32_t counts = getRandomNoOfInstructions();

            for (std::uint32_t i = 0; i < counts; ++i) {
                commands.push_back(std::make_shared<PrintCommand>());
            }

            std::string procName = "p" + std::to_string(pid);
            auto process = std::make_shared<Process>(procName, commands, pid);

            {
                std::lock_guard<std::mutex> lock(queueMutex);
                processLine.push(process);
            }

            manager->registerScreen(procName, std::make_shared<ProcessConsole>(procName, process));

        }
    }
}


std::uint32_t CPUAndScheduler::getRandomNoOfInstructions()
{
    std::uint32_t minIns = config->getMinInstructions();
    std::uint32_t maxIns = config->getMaxInstructions();

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<std::uint32_t> dist(minIns, maxIns);

    return dist(gen);
}

void CPUAndScheduler::CPUScheduler()
{
    while (true) {
        cpuCycles++;

        std::optional<std::uint32_t> freeCPUIdOpt = getRandomFreeCPU();

        if (freeCPUIdOpt.has_value() && !processLine.empty()) {
            std::uint32_t freeCPUId = freeCPUIdOpt.value();

            std::shared_ptr<Process> process = nullptr;

            {
                std::lock_guard<std::mutex> lock(queueMutex);
                process = processLine.front();
                processLine.pop();
            }

            if (process) {
                if (process->getPState() == Process::PAUSED) {
                    startCPUWorker(freeCPUId, process);
                }
                else if (process->getPState() == Process::WAITING) {
                    startCPUWorker(freeCPUId, process);
                    if (process->getPState() == Process::WORKING) {
                        workingProcesses.push_back(process);
                    }
                }
            }
        }
        transferFinishedProcesses();
        std::this_thread::sleep_for(std::chrono::microseconds(1000000)); // delay
    }
}

void CPUAndScheduler::startCPUWorker(std::uint32_t cpuId, std::shared_ptr<Process> proc)
{
    if (schedulingAlgo == "fcfs") {
        proc->setProcessState(Process::WORKING);
        proc->assignCPUIdWorkingOnIt(cpuId);

        std::thread([this, cpuId, proc]() {
            cpuWorkerLoopFCFS(cpuId, proc);
            }).detach();
    }
    else if (schedulingAlgo == "rr") {
        proc->setProcessState(Process::WORKING);
        proc->assignCPUIdWorkingOnIt(cpuId);

        std::thread([this, cpuId, proc]() {
            cpuWorkerLoopRR(cpuId, proc, quantumCycles);
            }).detach();
    }
}

void CPUAndScheduler::addProcessToProcessLine(std::shared_ptr<Process> proc)
{
    std::lock_guard<std::mutex> lock(queueMutex);
    processLine.push(proc);
}

void CPUAndScheduler::transferFinishedProcesses()
{
    auto it = workingProcesses.begin();
    while (it != workingProcesses.end()) {
        if ((*it)->getPState() == Process::FINISHED) {
            doneProcesses.push_back(*it);
            it = workingProcesses.erase(it);
        }
        else {
            ++it;
        }
    }
}

void CPUAndScheduler::printCPUReport()
{
    std::stringstream ss;

    std::uint32_t totalCores = config->getNCPU();
    std::uint32_t usedCores = 0;

    {
        std::lock_guard<std::mutex> lock(cpuMapMutex);
        for (const auto& [id, isFree] : cpuFreeMap) {
            if (!isFree) ++usedCores;
        }
    }

    float utilization = (totalCores == 0) ? 0.0f : static_cast<float>(usedCores) / totalCores * 100.0f;

    ss << std::fixed << std::setprecision(2);
    ss << "CPU Utilization: " << utilization << "%\n";
    ss << "Cores used: " << usedCores << "\n";
    ss << "Cores available: " << (totalCores - usedCores) << "\n\n";

    ss << "--------------------------\n";
    ss << "Running processes:\n";
    for (const auto& process : workingProcesses) {
        if (process->getCPUIdWorkingOnIt() == -1) {
            continue; 
        }

        ss << std::left << std::setw(12) << process->getPName()
            << "(" << getFormattedTime(process->getPCreationTime()) << ")   "
            << "Core: " << process->getCPUIdWorkingOnIt() << "   "
            << process->getCurrentInstruction() << "/" << process->getInstructionLimit()
            << "\n";
    }

    ss << "\nFinished processes:\n";
    for (const auto& process : doneProcesses) {
        ss << std::left << std::setw(12) << process->getPName()
            << "(" << getFormattedTime(process->getPEndTime()) << ")    Finished  "
            << process->getInstructionLimit() << "/" << process->getInstructionLimit()
            << "\n";
    }

    std::cout << ss.str();
}

void CPUAndScheduler::printCPUReportInText()
{
    std::stringstream ss;

    // Utilization metrics
    std::uint32_t totalCores = config->getNCPU();
    std::uint32_t usedCores = 0;

    {
        std::lock_guard<std::mutex> lock(cpuMapMutex);
        for (const auto& [id, isFree] : cpuFreeMap) {
            if (!isFree) ++usedCores;
        }
    }

    float utilization = (totalCores == 0) ? 0.0f : static_cast<float>(usedCores) / totalCores * 100.0f;

    ss << std::fixed << std::setprecision(2);
    ss << "CPU Utilization: " << utilization << "%\n";
    ss << "Cores used: " << usedCores << "\n";
    ss << "Cores available: " << (totalCores - usedCores) << "\n\n";

    ss << "--------------------------\n";

    ss << "Running processes:\n";
    for (const auto& process : workingProcesses) {
        if (process->getCPUIdWorkingOnIt() == -1) {
            continue;
        }

        ss << std::left << std::setw(12) << process->getPName()
            << "(" << getFormattedTime(process->getPCreationTime()) << ")   "
            << "Core: " << process->getCPUIdWorkingOnIt() << "   "
            << process->getCurrentInstruction() << "/" << process->getInstructionLimit()
            << "\n";
    }

    ss << "\nFinished processes:\n";
    for (const auto& process : doneProcesses) {
        ss << std::left << std::setw(12) << process->getPName()
            << "(" << getFormattedTime(process->getPEndTime()) << ")    Finished  "
            << process->getInstructionLimit() << "/" << process->getInstructionLimit()
            << "\n";
    }

    const std::string filename = "csopesy-log.txt";
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << ss.str();
        outFile.close();

        std::filesystem::path fullPath = std::filesystem::absolute(filename);
        std::cout << "Report generated at " << fullPath.string() << "\n\n";
    }
    else {
        std::cerr << "Failed to write to file: " << filename << "\n\n";
    }
}


std::string CPUAndScheduler::getFormattedTime(std::time_t t)
{
    std::tm localTime{};
    localtime_s(&localTime, &t);

    std::stringstream timeSS;
    timeSS << std::put_time(&localTime, "%m/%d/%Y %I:%M:%S%p");
    return timeSS.str();
}

void CPUAndScheduler::cpuWorkerLoopFCFS(std::uint32_t cpuId, std::shared_ptr<Process> proc)
{
    {
        std::lock_guard<std::mutex> lock(cpuMapMutex);
        cpuFreeMap[cpuId] = false;
    }

    uint32_t targetCycle = cpuCycles + delayPerExec;

    while (!proc->hasFinished()) {
        while (cpuCycles < targetCycle) {

        }

        proc->executeInstruction();
        proc->nextInstruction();

        targetCycle += delayPerExec;
    }


    proc->setNewPEndTime();
    proc->setProcessState(Process::FINISHED);

    {
        std::lock_guard<std::mutex> lock(cpuMapMutex);
        cpuFreeMap[cpuId] = true;
    }
}

void CPUAndScheduler::cpuWorkerLoopRR(std::uint32_t cpuId, std::shared_ptr<Process> proc, std::uint32_t quantumTurns)
{
    {
        std::lock_guard<std::mutex> lock(cpuMapMutex);
        cpuFreeMap[cpuId] = false;
    }

    uint32_t targetCycle = cpuCycles + delayPerExec;

    std::uint32_t executedThisQuantum = 0;

    while (!proc->hasFinished() && executedThisQuantum < quantumTurns) {
        while (cpuCycles < targetCycle) {
            // wait for the cycle
        }

        proc->executeInstruction();
        proc->nextInstruction();

        executedThisQuantum++;
        targetCycle += delayPerExec;
    }

    if (proc->hasFinished()) {
        proc->setNewPEndTime();
        proc->setProcessState(Process::FINISHED);
    }
    else {
        proc->assignCPUIdWorkingOnIt(-1);
        proc->setProcessState(Process::PAUSED);
        addProcessToProcessLine(proc);
    }

    {
        std::lock_guard<std::mutex> lock(cpuMapMutex);
        cpuFreeMap[cpuId] = true;
    }
}

std::optional<std::uint32_t> CPUAndScheduler::getRandomFreeCPU() {
    std::vector<std::uint32_t> freeCPUs;
    for (const auto& [cpuId, isFree] : cpuFreeMap) {
        if (isFree) {
            freeCPUs.push_back(cpuId);
        }
    }

    if (freeCPUs.empty()) {
        return std::nullopt;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(freeCPUs.begin(), freeCPUs.end(), gen);

    return freeCPUs.front();
}

std::uint32_t CPUAndScheduler::getCPUCycles()
{
    return cpuCycles;
}

void CPUAndScheduler::printProcessLine()
{
    std::lock_guard<std::mutex> lock(queueMutex);

    if (processLine.empty()) {
        std::cout << "Process line is empty.\n";
        return;
    }

    std::ostringstream oss;
    oss << "Processes in line:\n";

    std::queue<std::shared_ptr<Process>> tempQueue = processLine;
    while (!tempQueue.empty()) {
        auto proc = tempQueue.front();
        tempQueue.pop();
        oss << "- " << proc->getPName() << " (PID: " << proc->getPId() << ")\n";
    }

    std::cout << oss.str();
}

void CPUAndScheduler::schedulerTest()
{
    if (schedulerStateFlag) return;

    schedulerStateFlag = true;

    batchProcessingThread = std::thread([this]() {
        this->batchProcessor();
        });
    batchProcessingThread.detach();

    std::cout << "[Scheduler] Started.\n\n";
}

void CPUAndScheduler::schedulerStop()
{
    if (!schedulerStateFlag) return;

    schedulerStateFlag = false;

    if (batchProcessingThread.joinable()) {
        batchProcessingThread.join();
    }

    std::cout << "[Scheduler] Stopped.\n\n";
}
