#include "Thread.h"

#include "..\Exception.h"

#include "../../Types.h"

namespace Kiwi
{

	Thread::Thread( std::wstring threadName )
	{
		m_threadName = threadName;
		m_shutdown = false;
		m_threadFinished = false;
		m_running = false;
	}

	Thread::~Thread()
	{
		std::lock_guard<std::mutex> lock( m_threadMutex );

		if( !m_shutdown )
		{
			/*tell the thread to shutdown*/
			m_shutdown = true;

			/*stop any pending workers*/
			for( auto itr = m_workQueue.begin(); itr != m_workQueue.end(); itr++ )
			{
				(*itr)->Stop();
			}

			/*if a lock can be acquired, the thread is waiting on the condition variable so notify it to wakeup and shutdown*/
			std::unique_lock<std::mutex> lock( m_cv_M, std::defer_lock );
			if( lock.try_lock() )
			{
				m_cv.notify_one();
			}
		}

		if( m_future.valid() )
		{
			try
			{
				m_future.get();

			} catch( Kiwi::Exception& e ) 
			{
				KIWIDEBUGSTRING( e.ToString().c_str() );
			}
		}
	}

	void Thread::_ThreadFunction()
	{
		while( true )
		{
			std::shared_ptr<IAsyncWorker> currentTask = nullptr;

			/*efficiently wait until there are jobs in the work queue*/
			std::unique_lock<std::mutex> lock( m_cv_M );
			m_cv.wait( lock, [&] {return m_workQueue.size() > 0 || m_shutdown == true; } );

			while( m_workQueue.size() > 0 )
			{
				try
				{
					currentTask = m_workQueue.front();

					if( currentTask == 0 )
					{
						std::lock_guard<std::mutex> lock( m_threadMutex );
						m_workQueue.pop_front();
						continue;
					}

					currentTask->Start();
					while( !m_shutdown && currentTask->IsReady() == false ) currentTask->Run();
					currentTask->Finish();

				} catch( ... )
				{
					std::lock_guard<std::mutex> lock( m_threadMutex );
					m_workQueue.pop_front(); //remove task from work queue
					m_threadFinished = true;
					m_shutdown = true;
					throw;
				}

				{
					std::lock_guard<std::mutex> lock( m_threadMutex );
					m_workQueue.pop_front(); //remove task from work queue
					if( m_shutdown == true ) break;
				}
			}

			{
				std::lock_guard<std::mutex> lock( m_threadMutex );
				if( m_shutdown == true ) break;
			}
		}

		std::lock_guard<std::mutex> lock( m_threadMutex );
		m_threadFinished = true;
		m_shutdown = true;
	}

	bool Thread::EnqueueWorker( std::shared_ptr<IAsyncWorker> job )
	{

		std::lock_guard<std::mutex> lock( m_threadMutex );
		if( job && m_threadFinished == false && m_shutdown == false )
		{
			m_workQueue.push_back( job );

			/*if the thread is waiting on the condition variable, wake it up*/
			std::unique_lock<std::mutex> lock( m_cv_M, std::defer_lock );
			if( lock.try_lock() )
			{
				m_cv.notify_one();
			}
			return true;
		}

		return false;
	}

	void Thread::Start()
	{

		std::lock_guard<std::mutex> lock( m_threadMutex );

		if( m_running == false )
		{
			m_future = std::async( std::launch::async, &Thread::_ThreadFunction, this );

			if( m_workQueue.size() > 0 )
			{
				std::unique_lock<std::mutex> lock( m_cv_M );
				m_cv.notify_one();
			}

			m_running = true;
		}

	}

	void Thread::Shutdown()
	{
		std::lock_guard<std::mutex> lock( m_threadMutex );
		if( !m_shutdown )
		{
			/*tell the thread to shutdown*/
			m_shutdown = true;

			/*stop any pending workers*/
			for( auto itr = m_workQueue.begin(); itr != m_workQueue.end(); itr++ )
			{
				(*itr)->Stop();
			}

			/*if a lock can be acquired, the thread is waiting on the condition variable so notify it to wake-up and shutdown
			if the lock can't be acquired the thread is processing a worker and will shutdown once finished*/
			std::unique_lock<std::mutex> lock( m_cv_M, std::defer_lock );
			if( lock.try_lock() )
			{
				m_cv.notify_one();
			}
		}
	}

	void Thread::Join()
	{
		std::lock_guard<std::mutex> lock( m_threadMutex );

		/*wait for the thread to finish*/
		if( m_future.valid() )
		{
			try
			{
				m_future.get();

			} catch( Kiwi::Exception& e )
			{
				m_running = false;
				throw e;
			}
		}

		m_running = false;
	}

	bool Thread::TryJoin( unsigned int maxWait )
	{
		std::lock_guard<std::mutex> lock( m_threadMutex );

		if( m_future.valid() )
		{
			auto status = m_future.wait_for( std::chrono::milliseconds( maxWait ) );
			if( status == std::future_status::ready )
			{
				try
				{
					m_future.get();

				} catch( Kiwi::Exception& e )
				{
					m_running = false;
					throw e;
				}

				m_running = false;
				return true;
			}
		}

		return false;
	}

}