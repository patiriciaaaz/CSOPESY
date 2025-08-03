#ifndef COMMANDTYPEFACTORY_H
#define COMMANDTYPEFACTORY_H

#include <memory>
#include <string>
#include "CommandType.h"

class CommandTypeFactory {
public:
    static std::shared_ptr<CommandType> parseInstruction(const std::string& instr);
};

#endif // COMMANDTYPEFACTORY_H
