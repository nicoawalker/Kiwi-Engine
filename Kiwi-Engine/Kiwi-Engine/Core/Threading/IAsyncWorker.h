#ifndef _KIWI_IASYNCWORKER_H_
#define _KIWI_IASYNCWORKER_H_

#include <string>
#include <mutex>

namespace Kiwi
{

	class IAsyncWorker
	{
	protected:

		std::wstring m_targetThread;

		std::mutex m_worker_M;

		bool m_shutdown; //true if Stop() has been called, indicates that the worker has been instructed to shutdown

		bool m_ready;

	public:

		IAsyncWorker() :
			m_ready(false), m_shutdown( false ) {}
		IAsyncWorker( std::wstring targetThread ) :
			m_targetThread( targetThread ),
			m_ready(false), m_shutdown( false ) {}

		virtual ~IAsyncWorker(){}

		/*called in the thread once before calling Run in a loop*/
		virtual void Start() {}

		/*main function that does the work. this is called in a while loop in the thread until m_ready is true*/
		virtual void Run() = 0;

		/*called once when Run() is finished*/
		virtual void Finish() {}

		/*tells the worker to shutdown. whether or not the worker will respond is up to the implementation of the worker*/
		void Stop() { std::lock_guard<std::mutex> lock( m_worker_M ); m_shutdown = true; }

		bool IsReady()const { return m_ready || m_shutdown; }

		std::wstring GetThread()const { return m_targetThread; }

	};

}

#endif