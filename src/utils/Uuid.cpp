#include "D3E/Uuid.h"

using namespace D3E;

Uuid UuidGenerator::NewGuid()
{
	static UUIDv4::UUIDGenerator<std::mt19937_64> g;

	return g.getUUID();
}

std::string UuidGenerator::NewGuidStdStr()
{
	return NewGuid().str();
}

String UuidGenerator::NewGuidString()
{
	return NewGuidStdStr().c_str();
}

size_t UuidHash::operator()(const Uuid& ref) const
{
	return ref.hash();
}
