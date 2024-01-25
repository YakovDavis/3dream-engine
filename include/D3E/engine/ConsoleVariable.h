#pragma once

#include "D3E/CommonHeader.h"

namespace  D3E
{
	enum ConsoleVariableType { tInt, tFloat };

	class D3EAPI ConsoleVariable
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
