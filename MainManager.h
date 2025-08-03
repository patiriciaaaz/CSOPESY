#pragma once

#include "ConfigValues.h"
#include "ConsoleFormat.h"
#include "ProcessConsole.h"
#include "PrintCommand.h"
#include "Process.h"
#include "CPUAndScheduler.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

class CPUAndScheduler;

void createProcessWithoutInstructions(const std::string& name, int memSize);
void createProcessWithInstructions(const std::string& name, int memSize, const std::string& instrStr);


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
private:
    bool isInitialized = false;
    std::shared_ptr<ConfigValues> config = nullptr;
    std::shared_ptr<CPUAndScheduler> cpuScheduler = nullptr;
    std::vector<std::string> tokenize(const std::string& line);
    std::unordered_map<std::string, std::shared_ptr<ConsoleFormat>> screenRegistry;
    std::shared_ptr<ConsoleFormat> activeScreen = nullptr;
};

