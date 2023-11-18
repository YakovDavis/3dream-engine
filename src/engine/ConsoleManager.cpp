#include "D3E/engine/ConsoleManager.h"
#include <iostream>
#include <string>

D3E::ConsoleManager* D3E::ConsoleManager::instance_ = nullptr;

D3E::ConsoleManager::~ConsoleManager()
{
	for (auto & mapIter : variableMap_)
	{
		delete mapIter.second;
	}
	delete instance_;
}

D3E::ConsoleManager* D3E::ConsoleManager::getInstance()
{
	if (!instance_)
	{
		instance_ = new ConsoleManager;
	}
	return instance_;
}

void D3E::ConsoleManager::registerConsoleVariable(const char* name, int value)
{
	variableMap_.insert(name, new IntConsoleVariable(value));
}

void D3E::ConsoleManager::registerConsoleVariable(const char* name, float value)
{
	variableMap_.insert(name, new FloatConsoleVariable(value));
}

D3E::ConsoleVariable* D3E::ConsoleManager::findConsoleVariable(const char* name)
{
	auto foundVariable = variableMap_.find(name);
	if (foundVariable == variableMap_.end())
	{
		return nullptr;
	}
	return foundVariable->second;
}

void D3E::ConsoleManager::handleConsoleInput()
{
	std::string line;
	while (std::getline(std::cin, line))
	{
		int pos = line.find_first_of(' ');
		if (pos == std::string::npos)
		{
			ConsoleVariable* var = findConsoleVariable(line.data());
			if (var)
			{
				if (var->getType() == tInt)
				{
					std::cout << var->getInt() << "\n";
				}
				else
				{
					std::cout << var->getFloat() << "\n";
				}
			}
			else
			{
				std::cout << "No such variable\n";
			}
		}
		else
		{
			std::string name = line.substr(0, pos);
			std::string value = line.substr(pos+1);
			ConsoleVariable* var = findConsoleVariable(name.data());
			if (var)
			{
				if (var->getType() == tInt)
				{
					var->setInt(std::stoi(value));
				}
				else
				{
					var->setFloat(std::stof(value));
				}
			}
			else
			{
				std::cout << "No such variable\n";
			}
		}
	}
}
