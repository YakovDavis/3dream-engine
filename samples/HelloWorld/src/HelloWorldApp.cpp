#include <iostream>
#include "HelloWorldApp.h"

void HelloWorldApp::Run()
{
	std::cout << "Hello, 3dream Engine! " << std::endl;
}

D3E::App* D3E::CreateApp()
{
	return new HelloWorldApp();
}
