#pragma once

namespace D3E
{
	class TimerHandle
	{
		friend class TimerManager;

	public:
		TimerHandle();

		bool IsValid() const;
		void Invalidate();

		bool operator==(const TimerHandle& other) const;
		bool operator!=(const TimerHandle& other) const;

	private:
		int id_;
	};
} // namespace D3E
