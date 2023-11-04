#include "Debug.h"
#include <iostream>
#include <fstream>
#include "string"
#include <chrono>

void D3E::Debug::LogMessage(const std::string& text)
{
	auto time = GetTime();
	const std::string message = time + " " + text;
	PrintColoredText(Color::White, message);
	LogText(message);
}
void D3E::Debug::LogWarning(const std::string& text)
{
	auto time = GetTime();
	const std::string warning = time + " " + text;
	PrintColoredText(Color::Yellow, warning);
	LogText("WARNING: " + warning);
}
void D3E::Debug::LogError(const std::string& text)
{
	auto time = GetTime();
	const std::string error = time + " " + text;
	PrintColoredText(Color::Red, error);
	LogText("ERROR!!!: " + error);
}
void D3E::Debug::Assert(bool condition, const std::string& text)
{
	if(!condition)
	{
		throw std::runtime_error(text);
	}
}
void D3E::Debug::ClearLog()
{
	std::ofstream ofs(filePath_, std::ios_base::out | std::ios_base::trunc);
	ofs.close();
}

void D3E::Debug::PrintColoredText(D3E::Debug::Color color, const std::string& text)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

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

	std::cout << text << std::endl;

	SetConsoleTextAttribute(console,
	                        FOREGROUND_RED |
	                            FOREGROUND_GREEN |
	                            FOREGROUND_BLUE |
	                            FOREGROUND_INTENSITY);
}
void D3E::Debug::LogText(const std::string& text)
{
	std::fstream s{filePath_, std::ios_base::out | std::ios_base::app};
	if (s.is_open())
	{
		s << text << std::endl;
	}
	else
	{
		PrintColoredText(Color::Red, "Can't open log file");
	}
	s.close();
}
std::string D3E::Debug::GetTime()
{
	const auto now = std::chrono::system_clock::now();
	const std::time_t t_c = std::chrono::system_clock::to_time_t(now);

	const std::tm* t_m = std::localtime(&t_c);
	std::stringstream ss;
	ss << std::put_time(t_m, "%H:%M:%S");

	return ss.str();
}

