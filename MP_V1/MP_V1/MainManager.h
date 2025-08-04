#pragma once

#include "ConfigValues.h"
#include "ConsoleFormat.h"
#include "ProcessConsole.h"
#include "Process.h"
#include "CPUAndScheduler.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

class CPUAndScheduler;  
class PagingMemoryAllocator;

class MainManager : public std::enable_shared_from_this<MainManager> {
public:
    MainManager();

    void initialize();      
    void run();             
    void showMenu();
    void registerScreen(const std::string& name, std::shared_ptr<ConsoleFormat> screen);
    bool switchToScreen(const std::string& name);
    void listScreens() const;
    bool screenExists(const std::string& name) const;
    std::uint32_t currentPId = 0;
    std::vector<std::vector<std::string>> parseCommandString(const std::string& raw);
    std::uint32_t getRandomProcessSize();
    bool isValidProcessSize(std::uint32_t);
private:
    std::uint32_t frameSize = 0;
    bool isInitialized = false;
    std::shared_ptr<ConfigValues> config = nullptr;
    std::shared_ptr<CPUAndScheduler> cpuScheduler = nullptr;
    std::shared_ptr<PagingMemoryAllocator> memoryAllocator = nullptr;
    std::vector<std::string> tokenize(const std::string& line);
    std::unordered_map<std::string, std::shared_ptr<ConsoleFormat>> screenRegistry;
    std::shared_ptr<ConsoleFormat> activeScreen = nullptr;
};
