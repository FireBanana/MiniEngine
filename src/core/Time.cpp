#include "Time.h"

Time::Time()
	: m_DeltaTime(0), m_PreviousFrameTime(0)
{
}

void Time::Update(float elapsedTime)
{
	m_DeltaTime = elapsedTime - m_PreviousFrameTime;
	m_PreviousFrameTime = elapsedTime;
}
