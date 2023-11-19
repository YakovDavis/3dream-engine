#pragma once

#include "uuid_v4.h"

namespace D3E
{
	using Uuid = UUIDv4::UUID;

	const Uuid EmptyId = Uuid(0, 0);

	struct UuidHash
	{
		size_t operator()(const Uuid& ref) const;		
	};

	class UuidGenerator
	{
	public:
		UuidGenerator(UuidGenerator const&) = delete;
		void operator=(UuidGenerator const&) = delete;

		static Uuid NewGuid();

	private:
		UuidGenerator();
	};
} // namespace D3E