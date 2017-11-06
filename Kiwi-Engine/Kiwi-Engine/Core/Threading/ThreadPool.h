#ifndef _KIWI_THREADPOOL_H_
#define _KIWI_THREADPOOL_H_

#include "Thread.h"

#include <vector>

namespace Kiwi
{

	class ThreadPool
	{
	protected:

		/*thread pool of automatically created and named threads*/
		std::vector<std::unique_ptr<Thread>> m_autoThreadPool;

		/*thread pool for named threads specified by the user*/
		std::vector<std::unique_ptr<Thread>> m_userThreadPool;

		//maximum number of threads that can be automatically created
		unsigned int m_maxAutoPoolSize;

		unsigned int m_threadCounter;

		std::mutex m_threadPool_M;

	protected:

		/*spawns a new thread*/
		void _StartNewThread( std::shared_ptr<IAsyncWorker> worker, bool autoThread );

		/*sorts the auto thread pool by the number of worker tasks each has in its queue*/
		void _SortAutoThreadPool();

	public:

		ThreadPool();
		~ThreadPool();

		void Update();

		/*
		asynchronously runs a worker
		if the worker does not specify a thread manually, it will be run on a new thread if the thread pool is not at maximum size
		otherwise, the worker will be queued in the thread with the least amount of work in its queue
		if the worker does specify a thread it will be run on that thread if it exists, otherwise a new thread with that name will be created
		|-worker: a shared pointer to the worker that will be run asynchronously
		*/
		void RunAsync( std::shared_ptr<IAsyncWorker> worker );

		/*triggers a shutdown event on the thread and detaches the thread once shutdown
		>threadName: name of the thread to shutdown (only user-created threads can be manually shutdown)
		>waitForShutdown (false): if true, the function will block until the thread is finished before detaching the thread and returning*/
		void ShutdownThread( std::wstring threadName, bool waitForShutdown = false );

		void ShutdownAll();

		void SetMaxAutoPoolSize( unsigned int maxSize );

		unsigned int GetMaxPoolSize()const { return m_maxAutoPoolSize; }

	};

}

#endif