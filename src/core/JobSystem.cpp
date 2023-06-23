#include "JobSystem.h"
#include "utils/ThreadSafeQueue.h"

void JobSystem::Initialize()
{
	auto numCores = std::thread::hardware_concurrency();
	m_FinishedLabel.store(0);
	m_NumThreads = std::max(1u, numCores);

	for (uint32_t threadId = 0; threadId < m_NumThreads; ++threadId)
	{
		std::thread worker
		{ 
			[&]
			{
				std::function<void()> job;

				while (1)
				{
					if (m_JobPool.pop_front(job))
					{
						job();
						m_FinishedLabel.fetch_add(1);
					}
					else
					{
						std::unique_lock<std::mutex> lock(m_WakeMutex);
						m_WakeCondition.wait(lock);
					}
				}
			} 
		};

		worker.detach();
	}
}

void JobSystem::Execute(const std::function<void()>& job)
{
	m_CurrentLabel += 1;

	while (!m_JobPool.push_back(job)) { Poll(); }

	m_WakeCondition.notify_one();
}

void JobSystem::Dispatch(uint32_t jobCount, uint32_t groupSize, const std::function<void(JobDispatchArgs)>& job)
{
	if (jobCount == 0 || groupSize == 0)
		return;

	const auto groupCount = (jobCount + groupSize - 1) / groupSize;

	m_CurrentLabel += groupCount;

	for (auto groupIndex = 0; groupIndex < groupCount; ++groupIndex)
	{
		const auto& jobGroup = [&]
		{
			const auto groupJobOffset = groupIndex * groupSize;
			const auto groupJobEnd = std::min(groupJobOffset + groupSize, jobCount);

			JobDispatchArgs args;
			args.groupIndex = groupIndex;

			for (auto i = groupJobOffset; i < groupJobEnd; ++i)
			{
				args.jobIndex = i;
				job(args);
			}
		};

		while (m_JobPool.push_back(jobGroup)) { Poll(); }

		m_WakeCondition.notify_one();
	}
}

void JobSystem::Wait()
{
	while (IsBusy()) Poll();
}

bool JobSystem::IsBusy()
{
	return m_FinishedLabel.load() < m_CurrentLabel;
}

void JobSystem::Poll()
{
	m_WakeCondition.notify_one();
	std::this_thread::yield();
}
