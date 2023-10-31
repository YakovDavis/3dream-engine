#include <iostream>
#include "App.h"

void* __cdecl operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	return new uint8_t[size];
}

void * __cdecl operator new[](unsigned __int64 size, unsigned __int64 flags, unsigned __int64 debugFlags, char const * name, int a,unsigned int b, char const* file, int line)
{
	return new uint8_t[size];
}

extern D3E::App* D3E::CreateApp();

#if D3E_PLATFORM_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	auto app = D3E::CreateApp();

	app->SetAppInstance(hInstance);

	app->Run();

	delete app;
}
#elif // D3E_PLATFORM_WINDOWS
int main()
{
	auto app = D3E::CreateApp();

	app->Run();

	delete app;
}
#endif // D3E_PLATFORM_WINDOWS
