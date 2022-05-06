#pragma once

class Time
{
public:

	Time();

	void Update(float elapsedTime);

	inline float GetDeltaTime() { return m_DeltaTime; }

private:

	float m_DeltaTime;
	float m_PreviousFrameTime;
};

