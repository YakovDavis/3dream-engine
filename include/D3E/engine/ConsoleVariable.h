#ifndef D3ENGINE_CONSOLEVARIABLE_H
#define D3ENGINE_CONSOLEVARIABLE_H

namespace  D3E
{
	enum ConsoleVariableType { tInt, tFloat };

	class ConsoleVariable
	{
	public:
		virtual ~ConsoleVariable() = default;
		virtual int getInt() = 0;
		virtual float getFloat() = 0;
		virtual void setInt(int value) = 0;
		virtual void setFloat(float value) = 0;
		virtual ConsoleVariableType getType() = 0;
	};
}

#endif // D3ENGINE_CONSOLEVARIABLE_H
