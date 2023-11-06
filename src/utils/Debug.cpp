#include "D3E/Debug.h"
#include <chrono>
#include <iostream>

std::fstream D3E::Debug::fileStream;
HANDLE D3E::Debug::console = GetStdHandle(STD_OUTPUT_HANDLE);;

void D3E::Debug::LogMessage(const eastl::string& text)
{
	auto time = GetTime();
	const eastl::string message = time + " " + text;
	PrintColoredText(Color::White, message);
	LogText(message);
}

void D3E::Debug::LogWarning(const eastl::string& text)
{
	auto time = GetTime();
	const eastl::string warning = time + " " + text;
	PrintColoredText(Color::Yellow, warning);
	LogText("WARNING: " + warning);
}

void D3E::Debug::LogError(const eastl::string& text)
{
	auto time = GetTime();
	const eastl::string error = time + " " + text;
	PrintColoredText(Color::Red, error);
	LogText("ERROR!!!: " + error);
}

void D3E::Debug::Assert(bool condition, const eastl::string& text)
{
	if(!condition)
	{
		throw std::runtime_error(text.c_str());
	}
}

void D3E::Debug::ClearLog()
{
	if (!fileStream.is_open())
	{
		fileStream.open(filePath_.c_str(), std::ios_base::out | std::ios_base::trunc);
		if (!fileStream.is_open())
		{
			PrintColoredText(Color::Red, "Can't open log file");
			return;
		}
	}

	fileStream.close();
}

void D3E::Debug::CloseLog()
{
	if (fileStream.is_open())
	{
		fileStream.close();
	}
}

void D3E::Debug::PrintColoredText(D3E::Debug::Color color, const eastl::string& text)
{
	if(!console)
	{
		console = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	switch (color)
	{
		case White:
		{
			SetConsoleTextAttribute(console,
			                        FOREGROUND_RED |
			                            FOREGROUND_GREEN |
			                            FOREGROUND_BLUE |
			                            FOREGROUND_INTENSITY);

			break;
		}
		case Yellow:
		{
			SetConsoleTextAttribute(console,
			                        FOREGROUND_RED |
			                            FOREGROUND_GREEN |
			                            FOREGROUND_INTENSITY);

			break;
		}
		case Red:
		{
			SetConsoleTextAttribute(console, FOREGROUND_RED);

			break;
		}
	}

	std::cout << text.c_str() << std::endl;

	SetConsoleTextAttribute(console,
	                        FOREGROUND_RED |
	                            FOREGROUND_GREEN |
	                            FOREGROUND_BLUE |
	                            FOREGROUND_INTENSITY);
}
void D3E::Debug::LogText(const eastl::string& text)
{
	if (!fileStream.is_open())
	{
		fileStream.open(filePath_.c_str(), std::ios_base::out | std::ios_base::app);
		if (!fileStream.is_open())
		{
			PrintColoredText(Color::Red, "Can't open log file");
			return;
		}
	}

	fileStream << text.c_str() << std::endl;
}
eastl::string D3E::Debug::GetTime()
{
	const auto now = std::chrono::system_clock::now();
	const std::time_t t_c = std::chrono::system_clock::to_time_t(now);

	const std::tm* t_m = std::localtime(&t_c);
	std::stringstream ss;
	ss << std::put_time(t_m, "%H:%M:%S");

	return ss.str().c_str();
}

void D3E::Debug::HandleLastWindowsError(const eastl::string& errorPlace)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, nullptr );

	eastl::string ErrorStr = errorPlace + " failed with error " + std::to_string(dw).c_str() + ": " + static_cast<char*>(lpMsgBuf);

	LogError(ErrorStr);

	Assert(true, ErrorStr);

	LocalFree(lpMsgBuf);
}
