
#include "BarebonesProcess.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <sstream>

std::vector<Process> processes;

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
    Process p;
    p.name = name;
    p.currentLine = 1;
    p.totalLines = 100;
    p.timestamp = getCurrentTimestamp();

    processes.push_back(p);

    std::cout << "Barebones Process Created:\n";
    std::cout << "Name: " << p.name << "\n";
    std::cout << "Current Instruction: " << p.currentLine << " / " << p.totalLines << "\n";
    std::cout << "Timestamp: " << p.timestamp << "\n";
}
