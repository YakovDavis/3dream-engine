#pragma once

#include "SimpleMath.h"

#include <functional>

namespace D3E
{
	class FunctionalAction
	{
	public:
		FunctionalAction() = default;
		FunctionalAction(const std::function<bool()> f);
		FunctionalAction(const FunctionalAction& other);

		bool IsDone() const;
		bool IsRanged() const;
		void Perform();
		bool InRange() const;

	private:
		bool isDone_;
		bool isRanged_;
		DirectX::SimpleMath::Vector3 targetLocation_;
		std::function<bool()> function_;
		std::function<bool()> inRange_;
	};
} // namespace D3E