#ifndef _KIWI_THREAD_H_
#define _KIWI_THREAD_H_

#include "IAsyncWorker.h"

#include <deque>
#include <string>
#include <vector>
#include <mutex>
#include <future>
#include <thread>
#include <functional>

namespace Kiwi
{

	//TODO: implement detach thread on creation -> notify when done -> call callback function -> start next thread
	
	/*struct WorkerCallbackPair
	{
		std::shared_ptr<IAsyncWorker> worker;
		std::function<void( std::shared_ptr<IAsyncWorker> )> callback;

		WorkerCallbackPair( std::shared_ptr<IAsyncWorker> worker, std::function<void( std::shared_ptr<IAsyncWorker> )> callback = nullptr )
		{
			this->worker = worker;
			this->callback = callback;
		}
	};*/

	class Thread
	{
	protected:

		long m_UID;
		std::wstring m_threadName;

		std::future<void> m_future;

		std::deque<std::shared_ptr<IAsyncWorker>> m_workQueue;

		std::mutex m_cv_M;
		std::mutex m_threadMutex;

		std::condition_variable m_cv;

		/*flag read by the thread to tell it to shutdown*/
		bool m_shutdown;

		/*set to true by the thread when it returns*/
		bool m_threadFinished;

		bool m_running;

	protected:

		virtual void _ThreadFunction();

	public:

		Thread( std::wstring threadName );
		~Thread();

		bool EnqueueWorker( std::shared_ptr<IAsyncWorker> job );

		void Start();

		/*signals to the thread that it should shutdown*/
		void Shutdown();

		/*blocks until the thread finishes. once the thread returns this function will re-throw any exception thrown by the thread*/
		void Join();

		/*waits for the thread to finish for 'maxWait' milliseconds. if the thread does finish, this function will re-throw any exception thrown by the thread*/
		bool TryJoin( unsigned int maxWait = 0 );

		bool IsFinished() { std::lock_guard<std::mutex> lock( m_threadMutex ); return m_threadFinished; }

		bool IsShutdown() { std::lock_guard<std::mutex> lock( m_threadMutex ); return m_shutdown; }

		std::wstring GetName()const { return m_threadName; }
		long GetUID()const { return m_UID; }
		unsigned int GetWorkQueueSize() { std::lock_guard<std::mutex> lock( m_threadMutex ); return (unsigned int)m_workQueue.size(); }

	};

}

#endif