#include <iostream>
#include <fstream>
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	try
	{
		D3E::App mApp;
		std::ofstream MyFile("D:\\Library\\Documents\\before.txt");
		MyFile << "before";
		MyFile.close();
		D3E::App* app = D3E::CreateApp();
		std::ofstream MyFile1("D:\\Library\\Documents\\after.txt");
		MyFile1 << "after";
		MyFile1.close();

		app->SetAppInstance(hInstance);

		app->Run();

		delete app;
	}
	catch (std::exception& e)
	{
		std::ofstream MyFile("D:\\Library\\Documents\\error.txt");
		MyFile << e.what();
		MyFile.close();
	}
}

/*int main()
{
	auto app = D3E::CreateApp();

	app->Run();

	delete app;
}*/
