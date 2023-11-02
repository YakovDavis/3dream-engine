#include "App.h"

#include <fstream>
#include <iostream>

void* __cdecl operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	return new uint8_t[size];
}

void * __cdecl operator new[](unsigned __int64 size, unsigned __int64 flags, unsigned __int64 debugFlags, char const * name, int a,unsigned int b, char const* file, int line)
{
	return new uint8_t[size];
}

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
