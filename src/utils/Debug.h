#pragma once

#include <string>
#include "windows.h"

namespace D3E
{
	class Debug
	{
	private:
		enum Color
		{
			White,
			Yellow,
			Red,
		};
		inline static const std::string filePath_ = R"(..\..\..\..\..\log.txt)";
	public:
		static void LogMessage(const std::string& text);
		static void LogWarning(const std::string& text);
		static void LogError(const std::string& text);

		static void Assert(bool condition, const std::string& text);

		static void ClearLog();
	private:
		static void PrintColoredText(Color color, const std::string& text);
		static void LogText(const std::string& text);
		static std::string GetTime();
	};

}