#include "D3E/ai/FSM.h"

#include "D3E/Debug.h"

#include <format>

using namespace D3E;

FSM::FSM() : states_()
{
}

const FSMState& FSM::Peek() const
{
	return states_.top();
}

void FSM::Pop()
{
	states_.pop();
}

void FSM::Push(const FSMState& s)
{
	states_.push(s);
}

const FSMState& FSM::Current() const
{
	return states_.top();
}

bool FSM::Empty() const
{
	return states_.empty();
}

void FSM::Update()
{
	if (states_.empty())
	{
		Debug::LogWarning("[FSM] : Update(): states_ is empty!");

		return;
	}

	if (!states_.top())
	{
		Debug::LogWarning("[FSM] : Update(): states_.top() is nullptr");

		return;
	}

	states_.top()();
}
