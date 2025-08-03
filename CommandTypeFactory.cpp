#include "CommandTypeFactory.h"
#include "PrintCommand.h" // include your command implementations here

std::shared_ptr<CommandType> CommandTypeFactory::parseInstruction(const std::string& instr) {
    // Simple example: if instruction is PRINT, create a PrintCommand instance
    if (instr.find("PRINT") == 0) {
        return std::make_shared<PrintCommand>();
    }
    // Add more instructions here...

    return nullptr; // unknown instruction
}
