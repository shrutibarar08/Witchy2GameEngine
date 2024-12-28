#include "W2Timer.h"
#include <Windows.h>

W2Timer::W2Timer():
	m_secondsPerCount(0.0),
	m_deltaTime(0.0),
	m_baseTime(0),
	m_pausedTime(0),
	m_stopTime(0),
	m_prevTime(0),
	m_currTime(0),
	m_bStopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_secondsPerCount = 1.0 / (double)countsPerSec;
}

float W2Timer::TotalTime() const
{
	if (m_bStopped)
	{
		return static_cast<float>(((m_stopTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
	}
	return static_cast<float>(((m_currTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
}

float W2Timer::DeltaTime() const
{
	return static_cast<float>(m_deltaTime);
}

void W2Timer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_baseTime = currTime;
	m_prevTime = currTime;
	m_stopTime = 0;
	m_bStopped = false;
}

void W2Timer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
	if (m_bStopped)
	{
		m_pausedTime += (startTime - m_stopTime);
		m_prevTime = startTime;
		m_stopTime = 0;
		m_bStopped = false;
	}
}

void W2Timer::Stop()
{
	if (!m_bStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		m_stopTime = currTime;
		m_bStopped = true;
	}
}

void W2Timer::Tick()
{
	if (m_bStopped)
	{
		m_deltaTime = 0.0;
		return;
	}
	// Get the time for this frame
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_currTime = currTime;

	// Time difference between this frame and the previous frame
	m_deltaTime = (m_currTime - m_prevTime) * m_secondsPerCount;

	// Prepare for the next frame
	m_prevTime = m_currTime;

	// Force non-negative
	if (m_deltaTime < 0.0)
	{
		m_deltaTime = 0.0;
	}
}
