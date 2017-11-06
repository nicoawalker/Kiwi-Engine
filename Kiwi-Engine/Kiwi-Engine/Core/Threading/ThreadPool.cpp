#include "ThreadPool.h"

#include <algorithm>

#include "..\Exception.h"
#include "../EngineRoot.h"

#include "..\..\Types.h"

namespace Kiwi
{

	ThreadPool::ThreadPool()
	{
		m_maxAutoPoolSize = 500;
		m_threadCounter = 0;
	}

	ThreadPool::~ThreadPool()
	{
		for( auto threadItr = m_autoThreadPool.begin(); threadItr != m_autoThreadPool.end(); )
		{
			try
			{
				(*threadItr)->Join();

			} catch( Kiwi::Exception& e )
			{
				_kConsole.Print( Kiwi::ConsoleMessage( ConsoleMessageType::Error, e ) );
			}

			threadItr = m_autoThreadPool.erase( threadItr );
		}

		for( auto threadItr = m_userThreadPool.begin(); threadItr != m_userThreadPool.end(); )
		{
			try
			{
				(*threadItr)->Join();

			} catch( Kiwi::Exception& e )
			{
				_kConsole.Print( Kiwi::ConsoleMessage( ConsoleMessageType::Error, e ) );
			}

			threadItr = m_userThreadPool.erase( threadItr );
		}
	}

	void ThreadPool::_StartNewThread( std::shared_ptr<IAsyncWorker> worker, bool autoThread )
	{
		if( autoThread )
		{
			m_autoThreadPool.push_back( std::make_unique<Thread>( L"auto_" + Kiwi::ToWString( m_threadCounter ) ) );
			m_autoThreadPool.back()->EnqueueWorker( worker );
			m_autoThreadPool.back()->Start();

		} else
		{
			m_userThreadPool.push_back( std::make_unique<Thread>( worker->GetThread() ) );
			m_userThreadPool.back()->EnqueueWorker( worker );
			m_userThreadPool.back()->Start();
		}
	}

	void ThreadPool::_SortAutoThreadPool()
	{
		std::sort( m_autoThreadPool.begin(), m_autoThreadPool.end(), []( const std::unique_ptr<Thread>& t1, const std::unique_ptr<Thread>& t2 )
		{
			return t1->GetWorkQueueSize() < t2->GetWorkQueueSize();
		} );
	}

	void ThreadPool::Update()
	{
		std::lock_guard<std::mutex> lock( m_threadPool_M );

		/*remove any automatically generated threads that have finished*/
		for( auto threadItr = m_autoThreadPool.begin(); threadItr != m_autoThreadPool.end(); )
		{
			if( (*threadItr)->GetWorkQueueSize() == 0 && (*threadItr)->IsShutdown() == false )
			{
				(*threadItr)->Shutdown();

			} else
			{
				try
				{
					if( (*threadItr)->TryJoin( 0 ) )
					{
						threadItr = m_autoThreadPool.erase( threadItr );
						continue;
					}

				} catch( Kiwi::Exception& e )
				{
					_kConsole.Print( Kiwi::ConsoleMessage( ConsoleMessageType::Error, e ) );
					threadItr = m_autoThreadPool.erase( threadItr );
					continue;
				}
			}

			threadItr++;
		}

		if( m_autoThreadPool.size() == 0 ) m_threadCounter = 0;

		for( auto threadItr = m_userThreadPool.begin(); threadItr != m_userThreadPool.end(); )
		{
			if( (*threadItr)->IsFinished() )
			{
				try
				{
					if( (*threadItr)->TryJoin( 0 ) )
					{
						threadItr = m_userThreadPool.erase( threadItr );
						continue;
					}

				} catch( Kiwi::Exception& e )
				{
					_kConsole.Print( Kiwi::ConsoleMessage( ConsoleMessageType::Error, e ) );
					threadItr = m_userThreadPool.erase( threadItr );
					continue;
				}

			}

			threadItr++;
		}
	}

	void ThreadPool::RunAsync( std::shared_ptr<IAsyncWorker> worker )
	{
		if( worker == 0 ) return;

		std::lock_guard<std::mutex> lock( m_threadPool_M );

		if( worker->GetThread().size() == 0 )
		{/*worker doesn't specify any particular thread, use an auto thread*/
			if( m_autoThreadPool.size() < m_maxAutoPoolSize )
			{
				/*create a new thread*/
				this->_StartNewThread( worker, true );

			} else if( m_autoThreadPool.size() > 0 )
			{
				/*cant create a new thread, put the new task in the least-loaded thread*/
				this->_SortAutoThreadPool();
				m_autoThreadPool[0]->EnqueueWorker( worker );
			}

		} else
		{
			/*try to add the worker to an existing thread with a matching name*/
			for( auto itr = m_userThreadPool.begin(); itr != m_userThreadPool.end(); itr++ )
			{
				if( (*itr)->GetName().compare( worker->GetThread() ) == 0 )
				{
					(*itr)->EnqueueWorker( worker );
					return;
				}
			}

			/*thread with that name doesn't exist, create one*/
			this->_StartNewThread( worker, false );
		}
	}

	void ThreadPool::ShutdownThread( std::wstring threadName, bool waitForShutdown )
	{
		std::lock_guard<std::mutex> lock( m_threadPool_M );

		for( auto itr = m_userThreadPool.begin(); itr != m_userThreadPool.end(); itr++ )
		{
			if( (*itr)->GetName().compare( threadName ) == 0 )
			{
				(*itr)->Shutdown();
				return;
			}
		}
	}

	void ThreadPool::ShutdownAll()
	{
		std::lock_guard<std::mutex> lock( m_threadPool_M );

		for( auto itr = m_userThreadPool.begin(); itr != m_userThreadPool.end(); )
		{
			if( (*itr)->IsFinished() )
			{
				itr = m_userThreadPool.erase( itr );

			} else
			{
				(*itr)->Shutdown();
				itr++;
			}
		}

		for( auto itr = m_autoThreadPool.begin(); itr != m_autoThreadPool.end(); )
		{
			if( (*itr)->IsFinished() )
			{
				itr = m_userThreadPool.erase( itr );

			} else
			{
				(*itr)->Shutdown();
				itr++;
			}
		}
	}

	void ThreadPool::SetMaxAutoPoolSize( unsigned int maxSize )
	{
		std::lock_guard<std::mutex> lock( m_threadPool_M );

		m_maxAutoPoolSize = maxSize;

		if( m_autoThreadPool.size() > maxSize )
		{
			/*sort the thread pool so that the threads with no workers in the queue are at the front*/
			this->_SortAutoThreadPool();

			/*remove idle threads from the front until the thread pool is equal to maxSize, or until no more idle threads are left*/
			for( auto itr = m_autoThreadPool.begin(); itr != m_autoThreadPool.end(), m_autoThreadPool.size() > maxSize; )
			{
				if( (*itr)->GetWorkQueueSize() > 0 ) break;

				itr = m_autoThreadPool.erase( itr );
			}
		}
	}

}