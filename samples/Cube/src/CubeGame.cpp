#include <iostream>
#include "CubeGame.h"

D3E::App* D3E::CreateApp()
{
	return dynamic_cast<D3E::App*>(new CubeGame());
}
