#include "PagingMemoryAllocator.h"
#include "CPUAndScheduler.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

PagingMemoryAllocator::PagingMemoryAllocator(std::uint32_t maxOverMem, std::uint32_t memPerFrame)
{
    maxOverallMemory = maxOverMem;
    memoryPerFrame = memPerFrame;
    nPages = maxOverallMemory / memoryPerFrame;

    for (std::uint32_t i = 0; i < nPages; ++i)
        pageTable[i] = -1;

    pagedInFrames = 0;
    pagedOutFrames = 0;
    totalUtilizedMemory = 0;
}

bool PagingMemoryAllocator::allocateProcess(std::shared_ptr<Process> process)
{
    std::uint32_t pid = process->getPId();
    std::uint32_t pSize = process->getPSize();
    std::uint32_t pagesNeeded = (pSize + memoryPerFrame - 1) / memoryPerFrame;

    // Find free frames
    std::vector<std::uint32_t> freeFrames;
    for (const auto& [frame, assignedPid] : pageTable)
    {
        if (assignedPid == -1)
            freeFrames.push_back(frame);
        if (freeFrames.size() == pagesNeeded)
            break;
    }

    if (freeFrames.size() < pagesNeeded)
    {
        return false;
    }

    // Allocate frames to process
    for (std::uint32_t frame : freeFrames)
    {
        pageTable[frame] = pid;
        pagedInFrames++;
    }

    totalUtilizedMemory += pagesNeeded * memoryPerFrame;
    allocatedProcesses[pid] = process;
    return true;
}

void PagingMemoryAllocator::deallocateProcess(std::uint32_t pId)
{
    std::uint32_t framesFreed = 0;

    for (auto& [frame, assignedPid] : pageTable)
    {
        if (assignedPid == static_cast<std::int32_t>(pId))
        {
            assignedPid = -1;
            pagedOutFrames++;
            framesFreed++;
        }
    }

    totalUtilizedMemory -= framesFreed * memoryPerFrame;

    // Remove from allocatedProcesses
    allocatedProcesses.erase(pId);
}

bool PagingMemoryAllocator::isProcessInMemory(std::shared_ptr<Process> process)
{
    std::uint32_t pid = process->getPId();

    for (const auto& [frame, assignedPid] : pageTable)
    {
        if (assignedPid == static_cast<std::int32_t>(pid))
            return true;
    }

    return false;
}

void PagingMemoryAllocator::visualizeAllocatedProcesses()
{
    std::uint32_t nActiveCPUs = cpuScheduler->getAmountOfActiveCPUs();
    std::uint32_t nTotalCPUs = cpuScheduler->getAmountOfTotalCPUs();
    std::uint32_t totalMemory = getMaxOverallMemory();

    std::uint32_t usedMemory = 0;
    for (const auto& [pid, process] : allocatedProcesses) {
        usedMemory += process->getPSize();
    }

    double cpuUtilPercent = (nTotalCPUs == 0) ? 0.0 : (static_cast<double>(nActiveCPUs) / nTotalCPUs) * 100.0;
    double memUtilPercent = (totalMemory == 0) ? 0.0 : (static_cast<double>(usedMemory) / totalMemory) * 100.0;

    std::ostringstream oss;

    oss << "-------------------------------------------\n";
    oss << "|               PROCESS-SMI               |\n";
    oss << "|             PAGING ALLOCATOR            |\n";
    oss << "-------------------------------------------\n";
    oss << "CPU-Util: " << std::fixed << std::setprecision(2) << cpuUtilPercent << "%\n";
    oss << "Memory Usage: " << usedMemory << " B / " << totalMemory << " B\n";
    oss << "Memory Util: " << std::fixed << std::setprecision(2) << memUtilPercent << "%\n\n";

    oss << "===========================================\n";
    oss << "Running processes and memory usage:\n";
    oss << "-------------------------------------------\n";

    for (const auto& [pid, process] : allocatedProcesses)
    {
        oss << process->getPName() << " (" << process->getPSize() << " B)\n";
    }

    oss << "-------------------------------------------\n\n";

    std::cout << oss.str();
}

std::shared_ptr<Process> PagingMemoryAllocator::getProcessForBackingStore()
{
    std::shared_ptr<Process> selected = nullptr;

    for (const auto& [pid, proc] : allocatedProcesses)
    {
        if (proc->getPState() == Process::PAUSED)
        {
            if (!selected || proc->getPId() < selected->getPId())
                selected = proc;
        }
    }

    return selected;
}

std::uint32_t PagingMemoryAllocator::getMaxOverallMemory()
{
    return maxOverallMemory;
}

std::uint32_t PagingMemoryAllocator::getTotalUtilizedMemory()
{
    return totalUtilizedMemory;
}

std::uint32_t PagingMemoryAllocator::getMemoryPerFrame()
{
    return memoryPerFrame;
}

std::uint32_t PagingMemoryAllocator::getNPages()
{
    return nPages;
}

std::uint32_t PagingMemoryAllocator::getPagedInFrames()
{
    return pagedInFrames;
}

std::uint32_t PagingMemoryAllocator::getPagedOutFrames()
{
    return pagedOutFrames;
}

void PagingMemoryAllocator::schedulerAccess(std::shared_ptr<CPUAndScheduler> scheduler)
{
    cpuScheduler = scheduler;
}

void PagingMemoryAllocator::advancedStatistics()
{
    std::uint32_t totalMemory = getMaxOverallMemory();
    std::uint32_t usedMemory = 0;
    for (const auto& [pid, process] : allocatedProcesses) {
        usedMemory += process->getPSize();
    }

    std::vector<std::uint32_t> cycles = cpuScheduler->getCPUCycleStats();
    std::uint32_t totalCPUTicks = cycles[0];
    std::uint32_t activeCPUTicks = cycles[1];
    std::uint32_t idleCPUTicks = cycles[2];

    std::uint32_t numPagedIn = getPagedInFrames();
    std::uint32_t numPagedOut = getPagedOutFrames();
    std::uint32_t freeMemory = totalMemory > usedMemory ? totalMemory - usedMemory : 0;

    std::stringstream ss;
    ss << "-------------------------------------------\n";
    ss << "|                  VMSTAT                 |\n";
    ss << "|             PAGING ALLOCATOR            |\n";
    ss << "-------------------------------------------\n";
    ss << "Total Memory: " << totalMemory << "B\n";
    ss << "Used Memory: " << usedMemory << "B\n";
    ss << "Free Memory: " << freeMemory << "B\n";
    ss << "Idle CPU Ticks: " << idleCPUTicks << "\n";
    ss << "Active CPU Ticks: " << activeCPUTicks << "\n";
    ss << "Total CPU Ticks: " << totalCPUTicks << "\n";
    ss << "Num Paged In: " << numPagedIn << "\n";
    ss << "Num Paged Out: " << numPagedOut << "\n";
    ss << "-------------------------------------------\n\n";

    std::cout << ss.str();
}

