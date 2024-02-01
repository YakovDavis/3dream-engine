#pragma once

#include <functional>

namespace D3E
{
	class FunctionalAction
	{
	public:
		FunctionalAction();
		FunctionalAction(const std::function<bool()>& action);
		FunctionalAction(const std::function<bool()>& action,
		                 const std::function<bool()>& inRange);
		FunctionalAction(const FunctionalAction& other);

		bool IsDone() const;
		bool RequiresToBeInRange() const;
		void Perform();
		bool InRange() const;

	private:
		bool isDone_;
		bool isRanged_;
		std::function<bool()> action_;
		std::function<bool()> inRange_;
	};
} // namespace D3E