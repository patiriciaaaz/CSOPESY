#pragma once

#include <string>
#include <iostream>

class Process;

class CommandType
{
public:
	std::string commandTypeName;
	virtual ~CommandType() = default;
	virtual void executeCommand(Process& proc) = 0;
};

