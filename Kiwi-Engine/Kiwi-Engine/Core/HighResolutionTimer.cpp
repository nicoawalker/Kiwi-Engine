#include "HighResolutionTimer.h"
#include "Math.h"
#include "Utilities.h"

#include <Windows.h>

namespace Kiwi
{

	HighResolutionTimer::HighResolutionTimer()
	{

		m_countsPerSecond = 0.0;
		m_counterStart = 0;
		m_frameCount = 0;
		m_frameTimeOld = 0;
		m_frameTime = 0.0;
		m_totalTime = 0.0;

		m_started = false;

	}

	HighResolutionTimer::~HighResolutionTimer()
	{

	}

	void HighResolutionTimer::StartTimer()
	{
		this->Reset();

		//get the 64-bit integer containing the number of counts per second
		LARGE_INTEGER freqCount;
		QueryPerformanceFrequency(&freqCount);
		m_countsPerSecond = (double)(freqCount.QuadPart);

		//now get the current time (in counts)
		QueryPerformanceCounter(&freqCount);
		m_counterStart = freqCount.QuadPart;
		m_frameTimeOld = m_counterStart;

		m_started = true;
	}

	void HighResolutionTimer::Update()
	{

		this->_Tick();

	}

	void HighResolutionTimer::Reset()
	{

		m_countsPerSecond = 0.0;
		m_counterStart = 0;
		m_frameCount = 0;
		m_frameTimeOld = 0;
		m_frameTime = 0.0;
		m_totalTime = 0.0;

	}

	double HighResolutionTimer::GetTotalTime()const
	{

		return m_totalTime;

	}

	double HighResolutionTimer::GetFrameTime()const
	{

		return m_frameTime;

	}

	void HighResolutionTimer::_Tick()
	{

		//store the time in seconds since the timer was started
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		m_totalTime = (double)(currentTime.QuadPart - m_counterStart)/m_countsPerSecond;

		//now store the time since the last update (time since the last frame)
		__int64 tickCount;
		tickCount = currentTime.QuadPart - m_frameTimeOld;
		m_frameTimeOld = currentTime.QuadPart;

		if(tickCount < 0)
			tickCount = 1;

		if(m_countsPerSecond != 0.0)
		{
			m_frameTime = ((double)tickCount)/m_countsPerSecond;

		}else
		{
			m_frameTime = 1.0;
		}

		m_frameCount += 1;

	}

};