#pragma once

#include "CommonHeader.h"

#include <Windows.h>
#include <fstream>

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
		inline static const String filePath_ = R"(..\..\..\..\..\log.txt)";
		static std::fstream fileStream;
		static HANDLE console;
	public:
		static void LogMessage(const String& text);
		static void LogMessage(const int& text);
		static void LogWarning(const String& text);
		static void LogError(const String& text);

		static void HandleLastWindowsError(const String&  errorPlace);

		static void Assert(bool condition, const String& text);

		static void ClearLog();
		static void CloseLog();
	private:
		static void PrintColoredText(Color color, const String& text);
		static void LogText(const String& text);
		static String GetTime();
	};

}