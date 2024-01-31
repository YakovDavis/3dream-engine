#include "D3E/ai/FSM.h"

using namespace D3E;

FSM::FSM() : states_()
{
}

FSMState FSM::Pop()
{
	FSMState state = states_.top();
	states_.pop();

	return state;
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
