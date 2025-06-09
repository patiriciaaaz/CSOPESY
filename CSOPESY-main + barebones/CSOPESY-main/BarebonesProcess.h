
#ifndef BAREBONES_PROCESS_H
#define BAREBONES_PROCESS_H

#include <string>

struct Process {
    std::string name;
    int currentLine;
    int totalLines;
    std::string timestamp;
};

void createBarebonesProcess(const std::string& name);

#endif
