#include "Timer.h"

#include <Windows.h>

namespace Kiwi
{

	Timer::Timer()
	{
		m_startTime = 0;
	}

	void Timer::Start()
	{
		LARGE_INTEGER lInt;
		QueryPerformanceFrequency( &lInt );
		m_procFrequency = lInt.QuadPart;
		QueryPerformanceCounter( &lInt );
		m_startTime = lInt.QuadPart;
	}

	double Timer::Milliseconds()
	{
		if( m_procFrequency < 1 ) return 0;
		LARGE_INTEGER lInt;
		QueryPerformanceCounter( &lInt );

		/*convert elapsed time to microseconds*/
		return 1000.0 * ((long double)(lInt.QuadPart - m_startTime) / (long double)m_procFrequency);
	}

	double Timer::Microseconds()
	{
		if( m_procFrequency < 1 ) return 0;
		LARGE_INTEGER lInt;
		QueryPerformanceCounter( &lInt );

		/*convert elapsed time to microseconds*/
		return 1000000.0 * ((double)(lInt.QuadPart - m_startTime) / (double)m_procFrequency);
	}

}