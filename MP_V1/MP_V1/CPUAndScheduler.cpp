#include "CPUAndScheduler.h"
#include <random>
#include <cmath>
#include <vector>

CPUAndScheduler::CPUAndScheduler(std::shared_ptr<ConfigValues> cnfg, std::shared_ptr<MainManager> mm, std::shared_ptr<BackingStoreEmulator> bs, std::shared_ptr<PagingMemoryAllocator> malloc)
{
	config = cnfg;
    manager = mm;
    backingStore = bs;
    memoryAllocator = malloc;
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
    std::uint32_t frameSize = config->getMemoryPerFrame();

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
                commands.push_back(std::make_shared<Type>(true));
            }

            // 30 commands

            std::string procName = "p" + std::to_string(pid);

            // p0
            auto process = std::make_shared<Process>(procName, commands, pid, getRandomProcessSize(), frameSize);

            // process0

            {
                std::lock_guard<std::mutex> lock(queueMutex);
                processLine.push(process);
            }

            manager->registerScreen(procName, std::make_shared<ProcessConsole>(procName, process));


            //std::cout << "[BatchProcessor] Created process: " << procName << " at cycle " << cpuCycles << "\n";
        }

        //// Small sleep to avoid busy-waiting
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
        std::uint32_t activeCPUs = getAmountOfActiveCPUs();

        if (activeCPUs == 0) {
            idleCycles++;
        }
        else {
            activeCycles++;
        }
        // 500 5
        std::this_thread::sleep_for(std::chrono::microseconds(200));
        //std::this_thread::sleep_for(std::chrono::microseconds(500));
        transferFinishedProcesses();

        std::optional<std::uint32_t> freeCPUIdOpt = getRandomFreeCPU(); // id = 0

        if (freeCPUIdOpt.has_value() && !processLine.empty()) {
            std::uint32_t freeCPUId = freeCPUIdOpt.value();

            std::shared_ptr<Process> process = nullptr;

            {
                std::lock_guard<std::mutex> lock(queueMutex); //rQueue.front() = pinakaunang process sa queue
                process = processLine.front(); // process1
                processLine.pop(); // you need to pop the process from the queue to give way to the next process
            }

            if (process) {
                if (!memoryAllocator->isProcessInMemory(process)) {

                    backingStore->load(process);

                    bool isAllocated = memoryAllocator->allocateProcess(process);

                    if (isAllocated) {
                        bool wasNotPreviouslyPreempted = !(process->getPState() == Process::PAUSED);

                        startCPUWorker(freeCPUId, process);

                        bool isWorking = (process->getPState() == Process::WORKING);

                        if (isWorking && wasNotPreviouslyPreempted) {
                            workingProcesses.push_back(process);
                        }
                    }
                    else {
                        auto processToBeSentToBackingStore = memoryAllocator->getProcessForBackingStore();

                        if (!processToBeSentToBackingStore) {
                            {
                                std::lock_guard<std::mutex> lock(queueMutex);
                                processLine.push(process);
                            }
                            continue;
                        }

                        memoryAllocator->deallocateProcess(processToBeSentToBackingStore->getPId());
                        backingStore->store(processToBeSentToBackingStore);

                        bool isAllocated = memoryAllocator->allocateProcess(process);

                        if (isAllocated) {
                            bool wasNotPreviouslyPreempted = !(process->getPState() == Process::PAUSED);

                            startCPUWorker(freeCPUId, process);

                            bool isWorking = (process->getPState() == Process::WORKING);

                            if (isWorking && wasNotPreviouslyPreempted) {
                                workingProcesses.push_back(process);
                            }
                        }
                    }

                }
                else {
                    bool wasNotPreviouslyPreempted = !(process->getPState() == Process::PAUSED);

                    startCPUWorker(freeCPUId, process);

                    bool isWorking = (process->getPState() == Process::WORKING);

                    if (isWorking && wasNotPreviouslyPreempted) {
                        workingProcesses.push_back(process);
                    }
                }

            }
        }
        //std::this_thread::sleep_for(std::chrono::microseconds(1000)); // delay to slow down process counter
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
            memoryAllocator->deallocateProcess((*it)->getPId());
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

    // Working processes
    //ss << "Running processes:\n";
    //for (const auto& process : workingProcesses) {
    //    ss << std::left << std::setw(12) << process->getPName()
    //        << "(" << getFormattedTime(process->getPCreationTime()) << ")   "
    //        << "Core: " << process->getCPUIdWorkingOnIt() << "   "
    //        << process->getCurrentInstruction() << "/" << process->getInstructionLimit()
    //        << "\n";
    //}
    ss << "Running processes:\n";
    for (const auto& process : workingProcesses) { // 1000000
        if (process->getCPUIdWorkingOnIt() == -1) {
            continue; // Skip processes not actively running on a core
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
    ss << "\n";
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

std::uint32_t CPUAndScheduler::getRandomProcessSize()
{
    std::uint32_t minSize = config->getMinMemPerProc(); // e.g., 64
    std::uint32_t maxSize = config->getMaxMemPerProc(); // e.g., 65536

    // Build a list of all powers of 2 within the range
    std::vector<std::uint32_t> powerOfTwoSizes;
    for (std::uint32_t exp = 6; exp <= 16; ++exp) {
        std::uint32_t size = 1u << exp;
        if (size >= minSize && size <= maxSize) {
            powerOfTwoSizes.push_back(size);
        }
    }

    // Fallback in case no valid sizes were found
    if (powerOfTwoSizes.empty()) {
        return minSize; // or throw, or clamp to nearest valid power of 2
    }

    // Randomly select one of the valid powers of 2
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> dist(0, powerOfTwoSizes.size() - 1);

    return powerOfTwoSizes[dist(gen)];
}

std::uint32_t CPUAndScheduler::getAmountOfActiveCPUs()
{
    std::uint32_t activeCount = 0;

    for (const auto& [cpuId, isFree] : cpuFreeMap) {
        if (!isFree) { // if false, it means the CPU is active (not free)
            ++activeCount;
        }
    }

    return activeCount;
}

std::uint32_t CPUAndScheduler::getAmountOfTotalCPUs()
{
    return config->getNCPU();
}

std::vector<std::uint32_t> CPUAndScheduler::getCPUCycleStats() const
{
    return {
      cpuCycles.load(),
      activeCycles.load(),
      idleCycles.load()
    };
}

void CPUAndScheduler::cpuWorkerLoopFCFS(std::uint32_t cpuId, std::shared_ptr<Process> proc)
{
    {
        std::lock_guard<std::mutex> lock(cpuMapMutex); 
        cpuFreeMap[cpuId] = false; 
    }
    
    uint32_t targetCycle = cpuCycles + delayPerExec;

    while (!proc->hasFinished() && !proc->hasMemoryError()) {
        while (cpuCycles < targetCycle) {

        }

        proc->executeInstruction();
        proc->nextInstruction();

        targetCycle += delayPerExec;
    }

    if (proc->hasMemoryError()) {
        memoryAllocator->deallocateProcess(proc->getPId());

        auto it = std::find(workingProcesses.begin(), workingProcesses.end(), proc);
        if (it != workingProcesses.end()) {
            workingProcesses.erase(it);
        }

        {
            std::lock_guard<std::mutex> lock(cpuMapMutex);
            cpuFreeMap[cpuId] = true;
        }

        return;
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

    while (!proc->hasFinished() && executedThisQuantum < quantumTurns && !proc->hasMemoryError()) { 
        while (cpuCycles < targetCycle) {
        }

        

        proc->executeInstruction();
        proc->nextInstruction();

        executedThisQuantum++;
        // q
        targetCycle += delayPerExec;
    }

    if (proc->hasMemoryError()) {
        memoryAllocator->deallocateProcess(proc->getPId());

        auto it = std::find(workingProcesses.begin(), workingProcesses.end(), proc);
        if (it != workingProcesses.end()) {
            workingProcesses.erase(it);
        }

        {
            std::lock_guard<std::mutex> lock(cpuMapMutex);
            cpuFreeMap[cpuId] = true;
        }

        return;
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

void CPUAndScheduler::schedulerStart()
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
