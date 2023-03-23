/// Added from WickedEngine

#pragma once
#include <mutex>


	template<typename T, size_t capacity>
	class ThreadSafeQueue
	{
	private:

		T data[capacity];
		size_t head = 0;
		size_t tail = 0;

		std::mutex lock;

	public:

		bool push_back(const T& item)
		{
			auto result = false;
			lock.lock();

			auto next = (head + 1) % capacity;

			if (next != tail)
			{
				data[head] = item;
				head = next;
				result = true;
			}

			lock.unlock();
			return result;
		}

		bool pop_front(T& item)
		{
			auto result = false;
			lock.lock();

			if (tail != head)
			{
				item = data[tail];
				tail = (tail + 1) % capacity;
				result = true;
			}

			lock.unlock();
			return result;
		}
	};
