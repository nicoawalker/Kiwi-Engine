#ifndef _KIWI_TIMER_H_
#define _KIWI_TIMER_H_

namespace Kiwi
{

	class Timer
	{
	protected:

		long long m_startTime;
		long long m_procFrequency;

	public:

		Timer();

		/*resets and starts the timer*/
		void Start();

		/*returns the amount of time since the timer was started, in milliseconds (thousandth of a second)*/
		double Milliseconds();

		/*returns the amount of time since the timer was started, in microseconds (millionth of a second)*/
		double Microseconds();

	};
}

#endif