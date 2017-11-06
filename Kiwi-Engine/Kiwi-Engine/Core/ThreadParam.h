#ifndef _KIWI_THREADPARAM_H_
#define _KIWI_THREADPARAM_H_

#include <string>
#include <mutex>

namespace Kiwi
{

	class ThreadParam
	{
	protected:

		std::mutex m_threadMutex;

		//unique ID automatically generated for each thread when the thread is created
		long m_UID; 

		//optional unique name for the thread, specified by the user when the thread is created
		std::wstring m_threadName;

		//if set to true, indicates to a thread that it should shut down immediately
		//this should never be modified by a thread
		bool m_shutdownFlag;

		//set to true when the thread manager successfully joins the thread
		bool m_isReady;

		//current frame count held by the thread param, used for syncronization with the engine's frame count
		long long m_currentFrame;

	public:

		ThreadParam( std::wstring threadName = L"" );
		virtual ~ThreadParam() {}

		virtual void Lock();
		virtual void Unlock();

		virtual bool TryLock();

		/*if set to true, indicates to a thread that it should shut down immediately
		this should never be modified by a thread and is not thread safe*/
		void SetShutdownFlag( bool shutdown ) { m_shutdownFlag = shutdown; }

		void SetReady( bool isReady ) { m_isReady = isReady; }

		/*returns the unique global id of the thread*/
		long GetUID()const { return m_UID; }

		std::wstring GetName()const { return m_threadName; }

		bool GetShutdownFlag()const { return m_shutdownFlag; }

		/*blocks until the engine's frame count is higher than the frame count held by the thread param*/
		void Synchronize();

		bool IsReady()const { return m_isReady; }

	};
}


#endif