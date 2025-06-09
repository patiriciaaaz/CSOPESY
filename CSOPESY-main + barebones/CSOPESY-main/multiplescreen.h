#pragma once
#include <string>

struct Process {
    std::string name;
    int currentLine;
    int totalLines;
    std::string timestamp;
};

extern const int MAX_PROCESSES;
extern Process processes[];
extern int processCount;

std::string getCurrentTimestamp();
int findProcessIndex(const std::string& name);
void showScreen(const Process& p);
void screenConsole(const std::string& name);
