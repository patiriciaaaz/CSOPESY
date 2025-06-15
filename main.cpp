#include "multiplescreen.h"
#include "BarebonesProcess.h"
#include <iostream>
#include <ctime>
#include <sstream>

extern Process processes[];
extern int processCount;
extern const int MAX_PROCESSES;

std::string getCurrentTimestamp() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[20];
    sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d",
            ltm->tm_mon + 1, ltm->tm_mday, 1900 + ltm->tm_year,
            ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    return buffer;
}

void createBarebonesProcess(const std::string& name) {
    if (processCount >= MAX_PROCESSES) {
        std::cout << "Cannot create more processes. Max limit reached.\n";
        return;
    }

    if (findProcessIndex(name) != -1) {
        std::cout << "Process already exists.\n";
        return;
    }

    Process& p = processes[processCount++];
    p.name = name;
    p.currentLine = 1;
    p.totalLines = 100;
    p.timestamp = getCurrentTimestamp();

    std::cout << "Barebones Process Created:\n";
    std::cout << "Name: " << p.name << "\n";
    std::cout << "Current Instruction: " << p.currentLine << " / " << p.totalLines << "\n";
    std::cout << "Timestamp: " << p.timestamp << "\n";
}
