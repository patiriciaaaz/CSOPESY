#pragma once

#include <queue>
#include <memory>
#include <mutex>
#include <random> 
#include <vector>
#include <algorithm> 
#include <condition_variable>
#include <thread>
#include <unordered_map>
#include <optional>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <filesystem>  
#include <fstream>
#include "Process.h"
#include "ConfigValues.h"
#include "MainManager.h"

class MainManager;

class CPUAndScheduler {
public:
    CPUAndScheduler(std::shared_ptr<ConfigValues> cnfg,
        std::shared_ptr<MainManager> mm);
    void CPUScheduler();
    void batchProcessor();
    std::uint32_t getRandomNoOfInstructions();
    void cpuWorkerLoopFCFS(std::uint32_t cpuId, std::shared_ptr<Process> proc);
    void cpuWorkerLoopRR(std::uint32_t cpuId, std::shared_ptr<Process> proc, std::uint32_t quantumTurns);
    std::optional<std::uint32_t> getRandomFreeCPU();
    std::uint32_t getCPUCycles();
    void printProcessLine();
    void schedulerTest();
    void schedulerStop();
    void startCPUWorker(std::uint32_t cpuId, std::shared_ptr<Process> proc);
    void addProcessToProcessLine(std::shared_ptr<Process> proc);
    void transferFinishedProcesses();
    void printCPUReport();
    void printCPUReportInText();
    std::string getFormattedTime(std::time_t t);

private:
    std::uint32_t delayPerExec = 0;
    std::uint32_t batchProcessFreq = 0;
    std::uint32_t quantumCycles = 0;
    std::string schedulingAlgo;
    std::mutex queueMutex;
    std::mutex cpuMapMutex;
    std::vector<std::thread> cpuThreads;
    std::unordered_map<std::uint32_t, bool> cpuFreeMap;
    std::shared_ptr<std::uint32_t> currentPId;
    bool schedulerStateFlag = false;
    std::atomic_uint32_t cpuCycles;
    std::thread cpuSchedulingThread;
    std::thread batchProcessingThread;
    std::shared_ptr<ConfigValues> config;
    std::shared_ptr<MainManager> manager;
    std::vector<std::shared_ptr<Process>> workingProcesses;
    std::vector<std::shared_ptr<Process>> doneProcesses;
    std::queue<std::shared_ptr<Process>> processLine;
};
