#include "D3E/Uuid.h"

using namespace D3E;

Uuid UuidGenerator::NewGuid()
{
	static UUIDv4::UUIDGenerator<std::mt19937_64> g;

	return g.getUUID();
}

size_t UuidHash::operator()(const Uuid& ref) const
{
	return ref.hash();
}
