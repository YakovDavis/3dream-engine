#ifndef D3ENGINE_FLOATCONSOLEVARIABLE_H
#define D3ENGINE_FLOATCONSOLEVARIABLE_H

#include "ConsoleVariable.h"
#include "D3E/Debug.h"

namespace D3E
{
	class FloatConsoleVariable : public ConsoleVariable
	{
	private:
		float value_;
	public:
		explicit FloatConsoleVariable(float value) : value_(value) {}
		int getInt() override
		{
			Debug::Assert(false, "Wrong type");
			return 0;
		}
		float getFloat() override { return value_; }

		void setInt(int value) override
		{
			Debug::Assert(false, "Wrong type");
		}
		void setFloat(float value) override { value_ = value; }

		ConsoleVariableType getType() override { return tFloat; }
	};
}

#endif // D3ENGINE_FLOATCONSOLEVARIABLE_H
