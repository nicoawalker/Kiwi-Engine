#ifndef _KIWI_HIGHRESOLUTIONTIMER_H_
#define _KIWI_HIGHRESOLUTIONTIMER_H_

namespace Kiwi
{

	class HighResolutionTimer
	{
	protected:

		double m_countsPerSecond;
		__int64 m_counterStart;

		int m_frameCount;

		__int64 m_frameTimeOld;

		//stores the time the last frame took to complete, in seconds
		double m_frameTime;

		//the total time, in seconds, since StartTimer was called
		double m_totalTime;

		bool m_started;

	protected:

		//updates the timer
		void _Tick();

	public:

		HighResolutionTimer();
		virtual ~HighResolutionTimer();

		virtual void StartTimer();

		virtual void Update();

		virtual void Reset();

		/*returns the time in seconds since StartTimer() was last called*/
		virtual double GetTotalTime()const;

		/*returns the time the last frame took to complete*/
		virtual double GetFrameTime()const;

		virtual bool IsStarted()const { return m_started; }

	};
};

#endif