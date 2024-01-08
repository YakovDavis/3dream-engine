#pragma once

#include "App.h"
#include "CommonHeader.h"

#include <fstream>
#include <iostream>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

extern D3E::App* D3E::CreateApp();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	if (!AttachConsole(ATTACH_PARENT_PROCESS))
	{
		auto res = AllocConsole();
		// TODO: assert(res != 0);
	}

	FILE* pNewStdout = nullptr;
	FILE* pNewStderr = nullptr;
	FILE* pNewStdin = nullptr;

	freopen_s(&pNewStdout, "CONOUT$", "w", stdout);
	freopen_s(&pNewStderr, "CONOUT$", "w", stderr);
	freopen_s(&pNewStdin, "CONIN$", "r", stdin);

	std::cout.clear();
	std::cerr.clear();
	std::cin.clear();

	std::wcout.clear();
	std::wcerr.clear();
	std::wcin.clear();

	D3E::App* app = D3E::CreateApp();

	app->SetAppInstance(hInstance);

	app->Run();
	
	delete app;
}

/*int main()
{
	auto app = D3E::CreateApp();

	app->Run();

	delete app;
}*/
