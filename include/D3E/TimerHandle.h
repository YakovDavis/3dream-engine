#pragma once

#include "D3E/CommonHeader.h"
#include <cstdint>

namespace D3E
{
	class D3EAPI TimerHandle
	{
		friend class TimerManager;
		friend struct TimerHandleHash;

	public:
		TimerHandle();

		bool IsValid() const;
		void Invalidate();

		bool operator==(const TimerHandle& other) const;
		bool operator!=(const TimerHandle& other) const;

	private:
		uint64_t id_;
	};

	struct TimerHandleHash
	{
		size_t operator()(const TimerHandle& handle) const
		{
			return handle.id_;
		}
	};
} // namespace D3E
