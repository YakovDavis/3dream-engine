#include "D3E/ai/FunctionalAction.h"

using namespace D3E;

FunctionalAction::FunctionalAction(const std::function<bool()> f)
	: isDone_(false), isRanged_(false), function_(f), targetLocation_(),
	  inRange_(nullptr)
{
}

FunctionalAction::FunctionalAction(const FunctionalAction& other)
	: isDone_(other.isDone_), isRanged_(other.isRanged_),
	  function_(other.function_), targetLocation_(other.targetLocation_),
	  inRange_(other.inRange_)
{
}

bool FunctionalAction::IsDone() const
{
	return isDone_;
}

bool FunctionalAction::IsRanged() const
{
	return isRanged_;
}

void FunctionalAction::Perform()
{
	if (!function_)
	{
		return;
	}

	isDone_ = function_();
}

bool FunctionalAction::InRange() const
{
	if (!inRange_)
	{
		return false;
	}

	return inRange_();
}