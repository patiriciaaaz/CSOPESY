#pragma once
#include "ConsoleFormat.h"
#include "Process.h"
#include <string>
#include <iostream>
#include <memory>
#include <iomanip>   
#include <ctime>     

class ProcessConsole : public ConsoleFormat {
public:
    ProcessConsole(const std::string& name, std::shared_ptr<Process> proc);
    void onEnabled() override;
    void process() override;
    std::shared_ptr<Process> getProcess();

private:
    std::string screenName;
    std::shared_ptr<Process> proc;
    bool isProcessConsoleActive = false;
};
