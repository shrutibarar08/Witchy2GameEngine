#pragma once

class W2Timer
{
public:
	W2Timer();
	
	//~ Time Represnted in Seconds
	float TotalTime() const;
	float DeltaTime() const;

	void Reset();	// Call before message loop.
	void Start();	// Call when unpaused
	void Stop();	// Call when paused
	void Tick();	// call very frame

private:
	double m_secondsPerCount;
	double m_deltaTime;

	__int64 m_baseTime;
	__int64 m_pausedTime;
	__int64 m_stopTime;
	__int64 m_prevTime;
	__int64 m_currTime;

	bool m_bStopped;
};

