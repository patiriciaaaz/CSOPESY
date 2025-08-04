#include "Type.h"
#include "Process.h"

Type::Type(bool noEffect)
{
	hasNoEffect = noEffect;
}

Type::Type(std::string declareType, std::string declareVar, std::int16_t declareVal)
{
	instructionType = declareType;
	declareVariable = declareVar;
	declareValue = declareVal;
}

Type::Type(std::string addSubType, std::string resultantVar, std::string variableOp1, std::string variableOp2, std::int16_t valOp1, std::int16_t valOp2)
{
	instructionType = addSubType;
	resultantVariable = resultantVar;
	variableOperand1 = variableOp1;
	variableOperand2 = variableOp2;
	valueOperand1 = valOp1;
	valueOperand2 = valOp2;
}

Type::Type(std::string writeType, std::string writeMemoryAdd, std::string writeVar, std::int16_t writeVal, bool isWr)
{
	instructionType = writeType;
	writeMemoryAddress = writeMemoryAdd;
	writeVariable = writeVar;
	writeValue = writeVal;
	isWrite = isWr;
}

Type::Type(std::string readType, std::string readVar, std::string readMemoryAdd, bool isRd)
{
	instructionType = readType;
	readVariable = readVar;
	readMemoryAddress = readMemoryAdd;
	isRead = isRd;
}

Type::Type(std::string printType, std::string msg, std::string printVar)
{
	instructionType = printType;
	message = msg;
	printVariable = printVar;
}

void Type::executeCommand(Process& proc)
{
	if (!hasNoEffect) {
		if (instructionType == "DECLARE") {
			if (proc.variableTable.size() >= 32) {
				return;
			}
			proc.variableTable[declareVariable] = declareValue;
		}
		else if (instructionType == "WRITE") {
			if (proc.memory.find(writeMemoryAddress) == proc.memory.end()) {
				proc.stopDueToMemoryError(writeMemoryAddress);
				return;
			}

			if (writeVariable == "noVar") {
				proc.memory[writeMemoryAddress] = writeValue;
			}
			else {
				std::int16_t valueFromVar = proc.variableTable[writeVariable];
				proc.memory[writeMemoryAddress] = valueFromVar;
			}
		}
		else if (instructionType == "READ") {
			if (proc.memory.find(readMemoryAddress) == proc.memory.end()) {
				proc.stopDueToMemoryError(writeMemoryAddress);
				return;
			}
			std::int16_t valueFromAdd = proc.memory[readMemoryAddress];
			proc.variableTable[readVariable] = valueFromAdd;
		}

		else if (instructionType == "ADD") {
			if (valueOperand1 == -1 && valueOperand2 == -1) {
				std::int16_t sum = proc.variableTable[variableOperand1] + proc.variableTable[variableOperand2];
				proc.variableTable[resultantVariable] = sum;
			}
			else if (variableOperand1 == "noVarOp1") {
				std::int16_t sum = valueOperand1 + proc.variableTable[variableOperand2];
				proc.variableTable[resultantVariable] = sum;
			}
			else if (variableOperand2 == "noVarOp2") {
				std::int16_t sum = proc.variableTable[variableOperand1] + valueOperand2;
				proc.variableTable[resultantVariable] = sum;
			}
			else if (variableOperand1 == "noVarOps" && variableOperand2 == "noVarOps") {
				std::int16_t sum = valueOperand1 + valueOperand2;
				proc.variableTable[resultantVariable] = sum;
			}
		}
		else if (instructionType == "SUBTRACT") {
			if (valueOperand1 == -1 && valueOperand2 == -1) {
				std::int16_t sub = proc.variableTable[variableOperand1] - proc.variableTable[variableOperand2];
				proc.variableTable[resultantVariable] = sub;
			}
			else if (variableOperand1 == "noVarOp1") {
				std::int16_t sub = valueOperand1 - proc.variableTable[variableOperand2];
				proc.variableTable[resultantVariable] = sub;
			}
			else if (variableOperand2 == "noVarOp2") {
				std::int16_t sub = proc.variableTable[variableOperand1] - valueOperand2;
				proc.variableTable[resultantVariable] = sub;
			}
			else if (variableOperand1 == "noVarOps" && variableOperand2 == "noVarOps") {
				std::int16_t sub = valueOperand1 - valueOperand2;
				proc.variableTable[resultantVariable] = sub;
			}
		}

		else if (instructionType == "PRINT") {
			std::int16_t valueFromVar = proc.variableTable[printVariable];
			std::string msg = message;
			std::string combinedMsg = message + std::to_string(valueFromVar);
			proc.log(combinedMsg);
		}
	}
	else {
		return;
	}
}
