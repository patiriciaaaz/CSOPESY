#include "multiplescreen.h"
#include <iostream>
#include <ctime>

const int MAX_PROCESSES = 10;
Process processes[MAX_PROCESSES];
int processCount = 0;

std::string getCurrentTimestamp() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[30];
    int hour = ltm->tm_hour;
    std::string ampm = "AM";
    if (hour >= 12) {
        ampm = "PM";
        if (hour > 12) hour -= 12;
    }
    if (hour == 0) hour = 12;
    sprintf(buffer, "%02d/%02d/%04d, %02d:%02d:%02d %s",
            ltm->tm_mon + 1, ltm->tm_mday, 1900 + ltm->tm_year,
            hour, ltm->tm_min, ltm->tm_sec, ampm.c_str());
    return std::string(buffer);
}

int findProcessIndex(const std::string& name) {
    for (int i = 0; i < processCount; i++) {
        if (processes[i].name == name) {
            return i;
        }
    }
    return -1;
}

void showScreen(const Process& p) {
    std::cout << "----------------------------------------\n";
    std::cout << "Process name: " << p.name << "\n";
    std::cout << "Current line of instruction / Total line of instruction: "
              << p.currentLine << " / " << p.totalLines << "\n";
    std::cout << "Timestamp: " << p.timestamp << "\n";
    std::cout << "Type 'exit' to return to main menu.\n";
    std::cout << "----------------------------------------\n";
}

void screenConsole(const std::string& name) {
    int idx = findProcessIndex(name);
    if (idx == -1) {
        std::cout << "No such screen with name '" << name << "'. Returning to main menu.\n";
        return;
    }
    bool inScreen = true;
    while (inScreen) {
        showScreen(processes[idx]);
        std::cout << name << ":/> ";
        std::string input;
        std::getline(std::cin, input);
        if (input == "exit") {
            inScreen = false;
        } else {
            std::cout << "Unknown command inside screen. Type 'exit' to return.\n";
        }
    }
}
