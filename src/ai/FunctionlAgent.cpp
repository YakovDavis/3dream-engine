#include "D3E/ai/FunctionalAction.h"

using namespace D3E;

FunctionalAction::FunctionalAction()
	: isDone_(false), isRanged_(false), action_(nullptr), inRange_(nullptr)
{
}

FunctionalAction::FunctionalAction(const std::function<bool()>& action)
	: isDone_(false), isRanged_(false), action_(action), inRange_(nullptr)
{
}

FunctionalAction::FunctionalAction(const std::function<bool()>& action,
                                   const std::function<bool()>& inRange)
	: isDone_(false), isRanged_(true), action_(action), inRange_(inRange)
{
}

FunctionalAction::FunctionalAction(const FunctionalAction& other)
	: isDone_(other.isDone_), isRanged_(other.isRanged_),
	  action_(other.action_), inRange_(other.inRange_)
{
}

bool FunctionalAction::IsDone() const
{
	return isDone_;
}

bool FunctionalAction::RequiresToBeInRange() const
{
	return isRanged_;
}

void FunctionalAction::Perform()
{
	if (!action_)
	{
		return;
	}

	isDone_ = action_();
}

bool FunctionalAction::InRange() const
{
	if (!inRange_)
	{
		return false;
	}

	return inRange_();
}