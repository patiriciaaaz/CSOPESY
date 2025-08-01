#pragma once
#include <string>

// Base class for all commands
class CommandType {
public:
    std::string commandTypeName;

    CommandType() : commandTypeName("COMMAND") {}
    virtual ~CommandType() = default;

    // Execute the command logic
    virtual void executeCommand() = 0;

    // Optionally, other helpers or data members
};
