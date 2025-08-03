#pragma once
#include <string>


class CommandType {
public:
    std::string commandTypeName;

    CommandType() : commandTypeName("COMMAND") {}
    virtual ~CommandType() = default;

   
    virtual void executeCommand() = 0;

    
};
