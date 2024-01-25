#pragma once

#include "CommonHeader.h"

#include <Windows.h>
#include <fstream>

namespace D3E
{
	class D3EAPI Debug
	{
	private:
		inline static const String filePath_ = R"(..\..\..\..\log.txt)";
		static std::fstream fileStream;
		static HANDLE console;
	public:
		enum TextColor
		{
			White,
			Yellow,
			Red,
		};
		static void LogMessage(const String& text);
		static void LogWarning(const String& text);
		static void LogError(const String& text);

		static void HandleLastWindowsError(const String&  errorPlace);

		static void Assert(bool condition, const String& text);

		static void ClearLog();
		static void CloseLog();
	private:
		static void PrintColoredText(TextColor color, const String& text);
		static void LogText(const String& text);
		static String GetTime();
	};

}