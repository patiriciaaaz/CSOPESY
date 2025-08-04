#pragma once
#include "CommandType.h"

class Process;

class Type : public CommandType {
public:
	Type(bool noEffect);
	Type(std::string declareType, std::string declareVar, std::int16_t declareVal);
	Type(std::string addSubType, std::string resultantVar, std::string variableOp1, std::string variableOp2, std::int16_t valOp1, std::int16_t valOp2);
	Type(std::string writeType, std::string writeMemoryAdd, std::string writeVar, std::int16_t writeVal, bool isWr);
	Type(std::string readType, std::string readVar, std::string readMemoryAdd, bool isRd);
	Type(std::string printType, std::string msg, std::string printVar);
	void executeCommand(Process& proc) override;
private:
	std::string instructionType = "";
	bool hasNoEffect = false;

	// DECLARE
	std::string declareVariable = "";
	std::int16_t declareValue = 0;

	// ADD // SUB
	std::string resultantVariable = "";
	std::int16_t valueOperand1 = 0;
	std::int16_t valueOperand2 = 0;
	std::string variableOperand1 = "";
	std::string variableOperand2 = "";

	// WRITE
	std::string writeMemoryAddress = "";
	std::string writeVariable = "";
	std::int16_t writeValue = 0;
	bool isWrite = false;

	// READ
	std::string readVariable = "";
	std::string readMemoryAddress = "";
	bool isRead = false;

	// PRINT
	std::string message = "";
	std::string printVariable = "";
};

