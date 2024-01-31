#pragma once

#include "EASTL/priority_queue.h"
#include "EASTL/vector.h"

namespace D3E
{
	template<typename T, typename TPriority> class PrioritizedItem
	{
	public:
		PrioritizedItem(const T& item, const TPriority& priority)
			: item_(item), priority_(priority)
		{
		}
		PrioritizedItem(const PrioritizedItem& other)
		{
			item_ = other.item_;
			priority_ = other.priority_;
		}

		const T& GetValue() const { return item_; }

		TPriority GetPriority() const { return priority_; }

	private:
		T item_;
		TPriority priority_;
	};

	template<typename T, typename TPriority>
	bool operator>(const PrioritizedItem<T, TPriority>& lhs,
	               const PrioritizedItem<T, TPriority>& rhs)
	{
		return lhs.GetPriority() > rhs.GetPriority();
	}

	template<typename T, typename TPriority>
	bool operator<(const PrioritizedItem<T, TPriority>& lhs,
	               const PrioritizedItem<T, TPriority>& rhs)
	{
		return lhs.GetPriority() > rhs.GetPriority();
	}

	template<typename TItem, typename TPriority> class PriorityQueue
	{
		using PElement = PrioritizedItem<TItem, TPriority>;

	public:
		bool IsEmpty() const { return queue_.empty(); }

		void Push(TItem item, TPriority priority)
		{
			queue_.push({item, priority});
		}

		TItem Pop()
		{
			auto item = queue_.top().GetValue();
			queue_.pop();

			return item;
		}

	private:
		eastl::priority_queue<PElement, eastl::vector<PElement>,
		                      eastl::greater<PElement>>
			queue_;
	};
} // namespace D3E