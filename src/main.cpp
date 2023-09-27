#include <iostream>
#include <EASTL/vector.h>

void* __cdecl operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	return new uint8_t[size];
}

void * __cdecl operator new[](unsigned __int64 size, unsigned __int64 flags, unsigned __int64 debugFlags, char const * name, int a,unsigned int b, char const* file, int line)
{
	return new uint8_t[size];
}

int main()
{
	eastl::vector<int> v;
	v.push_back(1);
	std::cout << "Hello, 3dream Engine! " << v[0] << std::endl;
}
