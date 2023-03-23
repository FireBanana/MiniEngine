#pragma once
#include <functional>

#include "utils/ThreadSafeQueue.h"

	class Job;

	using JobFunc = void (*)(Job& job);

	class Job
	{

	};

	struct JobDispatchArgs
	{
		uint32_t jobIndex;
		uint32_t groupIndex;
	};

	class JobSystem
	{
	private:
		uint32_t m_NumThreads = 0;
		ThreadSafeQueue<std::function<void()>, 256> m_JobPool;
		std::condition_variable m_WakeCondition;
		std::mutex m_WakeMutex;
		uint64_t m_CurrentLabel = 0;
		std::atomic<uint64_t> m_FinishedLabel;

	public:

		void Initialize();
		void Execute(const std::function<void()>& job);
		void Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(JobDispatchArgs)>& job);
		void Wait();

		bool IsBusy();

	private:

		void Poll();
	};