#ifndef D3ENGINE_CONSOLEMANAGER_H
#define D3ENGINE_CONSOLEMANAGER_H

#include "EASTL/string_map.h"
#include "IntConsoleVariable.h"
#include "FloatConsoleVariable.h"

namespace D3E
{
	class ConsoleManager
	{
	private:
		static ConsoleManager* instance_;

		eastl::string_map<ConsoleVariable*> variableMap_;

	protected:
		ConsoleManager() = default;

	public:
		~ConsoleManager();
		static ConsoleManager* getInstance();
		void registerConsoleVariable(const char* name, int value);
		void registerConsoleVariable(const char* name, float value);
		ConsoleVariable* findConsoleVariable(const char* name);
		void handleConsoleInput();
	};
}

#endif // D3ENGINE_CONSOLEMANAGER_H
