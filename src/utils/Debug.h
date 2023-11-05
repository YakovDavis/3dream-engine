#pragma once

#include "EASTL/string.h"
#include <fstream>
#include <Windows.h>

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
		inline static const eastl::string filePath_ = R"(..\..\..\..\..\log.txt)";
		static std::fstream fileStream;
		static HANDLE console;
	public:
		static void LogMessage(const eastl::string& text);
		static void LogWarning(const eastl::string& text);
		static void LogError(const eastl::string& text);

		static void HandleLastWindowsError(const eastl::string&  errorPlace);

		static void Assert(bool condition, const eastl::string& text);

		static void ClearLog();
		static void CloseLog();
	private:
		static void PrintColoredText(Color color, const eastl::string& text);
		static void LogText(const eastl::string& text);
		static eastl::string GetTime();
	};

}