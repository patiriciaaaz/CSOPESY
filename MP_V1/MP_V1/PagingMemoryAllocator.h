#pragma once
#include <vector>
#include <memory>
#include "Process.h"
#include <unordered_map>

class CPUAndScheduler;
class Process;

class PagingMemoryAllocator
{
public:
	PagingMemoryAllocator(std::uint32_t maxOverMem, std::uint32_t memPerFrame);
	bool allocateProcess(std::shared_ptr<Process> process);
	void deallocateProcess(std::uint32_t pId);
	bool isProcessInMemory(std::shared_ptr<Process> process);
	void visualizeAllocatedProcesses();
	std::shared_ptr<Process> getProcessForBackingStore();
	std::uint32_t getMaxOverallMemory();
	std::uint32_t getTotalUtilizedMemory();
	std::uint32_t getMemoryPerFrame();
	std::uint32_t getNPages();
	std::uint32_t getPagedInFrames();
	std::uint32_t getPagedOutFrames();
	void schedulerAccess(std::shared_ptr<CPUAndScheduler> scheduler);
	void advancedStatistics();

private:
	std::shared_ptr<CPUAndScheduler> cpuScheduler = nullptr;
	std::uint32_t maxOverallMemory = 0;
	std::uint32_t totalUtilizedMemory = 0;
	std::uint32_t memoryPerFrame = 0;
	std::uint32_t nPages = 0;
	std::uint32_t pagedInFrames = 0;
	std::uint32_t pagedOutFrames = 0;
	std::unordered_map<std::uint32_t, std::int32_t> pageTable;
	std::unordered_map<std::uint32_t, std::shared_ptr<Process>> allocatedProcesses;

};

