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

int main()
{
	auto app = D3E::CreateApp();

	app->Run();

	delete app;
}
