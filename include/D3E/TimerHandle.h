#pragma once

namespace D3E
{
	class TimerHandle
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
		int id_;
	};

	struct TimerHandleHash
	{
		size_t operator()(const TimerHandle& handle) const
		{
			return handle.id_;
		}
	};
} // namespace D3E
