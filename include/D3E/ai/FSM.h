#pragma once

#include "EASTL/stack.h"
#include "sol/sol.hpp"

#include <functional>

namespace D3E
{
	using FSMState = std::function<void()>;

	class FSM
	{
	public:
		FSM();

		const FSMState& Peek() const;
		void Pop();
		void Push(const FSMState& s);
		const FSMState& Current() const;
		bool Empty() const;
		void Update();

	private:
		eastl::stack<FSMState> states_;
	};
} // namespace D3E