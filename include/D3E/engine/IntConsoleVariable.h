#ifndef D3ENGINE_INTCONSOLEVARIABLE_H
#define D3ENGINE_INTCONSOLEVARIABLE_H

#include "ConsoleVariable.h"
#include "D3E/Debug.h"

namespace D3E
{
	class IntConsoleVariable : public ConsoleVariable
	{
	private:
		int value_;
	public:
		explicit IntConsoleVariable(int value) : value_(value) {}
		int getInt() override { return value_; }
		float getFloat() override
		{
			Debug::Assert(false, "Wrong type");
			return 0;
		}

		void setInt(int value) override { value_ = value; }
		void setFloat(float value) override
		{
			Debug::Assert(false, "Wrong type");
		}

		ConsoleVariableType getType() override { return tInt; }
	};
}

#endif // D3ENGINE_INTCONSOLEVARIABLE_H
