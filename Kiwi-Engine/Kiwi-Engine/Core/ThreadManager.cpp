#include "ThreadManager.h"

namespace Kiwi
{

	Kiwi::ThreadParam* ThreadManager::CreateThread( void( *threadFunction )(void) )
	{
		std::lock_guard<std::mutex> lockGuard( m_threadManagerMutex );

		ThreadData data;
		data.parameters = std::make_unique<Kiwi::ThreadParam>();

		if( data.parameters == nullptr ) return nullptr;

		data.future = std::async( std::launch::async, threadFunction );

		Kiwi::ThreadParam* p = data.parameters.get();

		m_test.insert( std::make_pair( 2, data ) );

		return p;
	}

	/*creates a new named thread that has no return value and takes no parameters
	returns an empty thread parameter class that contains the unique name and ID number of the generated thread
	the returned thread parameter is owned by the thread manager until the thread is joined, at which time ownership is transferred to the joining function*/
	Kiwi::ThreadParam* ThreadManager::CreateThread( std::wstring uniqueThreadName, void( *threadFunction )(void) )
	{
		std::lock_guard<std::mutex> lockGuard( m_threadManagerMutex );

		ThreadData data;
		data.parameters = std::make_unique<Kiwi::ThreadParam>( uniqueThreadName );
		data.future = std::async( std::launch::async, threadFunction );

		Kiwi::ThreadParam* p = data.parameters.get();

		m_threads.insert( std::make_pair( ThreadManager::ThreadKey( data.parameters->GetUID(), data.parameters->GetName() ), data ) );

		return p;
	}

	/*creates a new thread that takes one parameter of type Kiwi::IThreadParam and has no return value
	the thread will be created using the unique ID of the thread parameter, and the unique name if it is set
	thread parameter ownership is passed to the thread manager and is returned when the thread is joined, and deleted if the thread throws an exception when joined*/
	void ThreadManager::CreateThread( void( *threadFunction )(Kiwi::ThreadParam*), std::unique_ptr<Kiwi::ThreadParam> threadParameter )
	{
		std::lock_guard<std::mutex> lockGuard( m_threadManagerMutex );

		if( threadParameter == 0 ) return;

		ThreadData data;
		data.parameters = std::move( threadParameter );
		data.future = std::async( std::launch::async, threadFunction, data.parameters.get() );

		m_threads.insert( std::make_pair( ThreadManager::ThreadKey( data.parameters->GetUID(), data.parameters->GetName() ), data ) );
	}

	/*returns the current status of the thead with the given thread ID, or returns THREAD_INVALID if there is no thread with matching ID*/
	ThreadStatus ThreadManager::CheckThread( long threadID )
	{
		std::lock_guard<std::mutex> lockGuard( m_threadManagerMutex );

		auto threadDataItr = m_threads.find( ThreadManager::ThreadKey( threadID ) );
		if( threadDataItr != m_threads.end() )
		{
			auto fStatus = threadDataItr->second.future.wait_for( std::chrono::milliseconds( 0 ) );
			switch( fStatus )
			{
				case std::future_status::ready:
					{
						return ThreadStatus::THREAD_READY;
					}
				case std::future_status::deferred:
					{
						return ThreadStatus::THREAD_DEFERRED;
					}
				case std::future_status::timeout:
					{
						return ThreadStatus::THREAD_TIMEOUT;
					}
				default: return ThreadStatus::THREAD_UNKNOWN;
			}

		} else
		{
			return THREAD_INVALID;
		}
	}

	/*returns the current status of the thead with the given name, or returns THREAD_INVALID if there is no thread with matching name*/
	ThreadStatus ThreadManager::CheckThread( std::wstring threadName )
	{
		std::lock_guard<std::mutex> lockGuard( m_threadManagerMutex );

		auto threadDataItr = m_threads.find( ThreadManager::ThreadKey( 0, threadName ) );
		if( threadDataItr != m_threads.end() )
		{
			auto fStatus = threadDataItr->second.future.wait_for( std::chrono::milliseconds( 0 ) );
			switch( fStatus )
			{
				case std::future_status::ready:
					{
						return ThreadStatus::THREAD_READY;
					}
				case std::future_status::deferred:
					{
						return ThreadStatus::THREAD_DEFERRED;
					}
				case std::future_status::timeout:
					{
						return ThreadStatus::THREAD_TIMEOUT;
					}
				default: return ThreadStatus::THREAD_UNKNOWN;
			}

		} else
		{
			return THREAD_INVALID;
		}
	}

	/*attemps to join the thread with the matching name. if no matching thread is found will throw INVALIDTHREAD
	if the thread is joined this function will re-throw that exception, otherwise this function will return ownership of the thread's parameter
	-maxWaitMilliseconds: if > -1, will throw TIMEOUT exception if the thread isn't joined in that amount of time, else function will block until it is joined*/
	std::unique_ptr<Kiwi::ThreadParam> ThreadManager::JoinThread( long threadID, int maxWaitMilliseconds )
	{
		std::lock_guard<std::mutex> lockGuard( m_threadManagerMutex );

		auto threadDataItr = m_threads.find( ThreadManager::ThreadKey( threadID ) );
		if( threadDataItr != m_threads.end() )
		{
			std::shared_future<void> future = threadDataItr->second.future;

			/*tell the thread to shutdown*/
			threadDataItr->second.parameters->SetShutdownFlag( true );

			if( maxWaitMilliseconds > -1 )
			{
				/*wait for the thread to be ready*/
				auto fStatus = future.wait_for( std::chrono::milliseconds( maxWaitMilliseconds ) );
				switch( fStatus )
				{
					case std::future_status::ready:
						{
							std::unique_ptr<Kiwi::ThreadParam> param = std::move( threadDataItr->second.parameters );
							m_threads.erase( threadDataItr );

							/*will re-throw any errors thrown by the thread*/
							future.get();

							return param;
						}
					case std::future_status::deferred:
					case std::future_status::timeout:
						{
							throw Kiwi::Exception( L"ThreadManager::JoinThread", L"Thread timed out", KIWI_EXCEPTION::TIMEOUT );
						}
					default: throw Kiwi::Exception( L"ThreadManager::JoinThread", L"Unknown thread error occured", KIWI_EXCEPTION::UNKNOWNERROR );
				}

			} else
			{
				std::unique_ptr<Kiwi::ThreadParam> param = std::move( threadDataItr->second.parameters );
				m_threads.erase( threadDataItr );

				/*will re-throw any errors thrown by the thread*/
				future.get();

				return param;
			}

		} else
		{
			throw Kiwi::Exception( L"ThreadManager::JoinThread", L"Invalid thread requested", KIWI_EXCEPTION::INVALIDTHREAD );
		}
	}

	/*attemps to join the thread with the matching name. if no matching thread is found will throw INVALIDTHREAD
	if the thread is joined this function will re-throw that exception, otherwise this function will return ownership of the thread's parameter
	-maxWaitMilliseconds: if > -1, will throw TIMEOUT exception if the thread isn't joined in that amount of time, else function will block until it is joined*/
	std::unique_ptr<Kiwi::ThreadParam> ThreadManager::JoinThread( std::wstring threadName, int maxWaitMilliseconds )
	{
		std::lock_guard<std::mutex> lockGuard( m_threadManagerMutex );

		auto threadDataItr = m_threads.find( ThreadManager::ThreadKey( 0, threadName ) );
		if( threadDataItr != m_threads.end() )
		{
			std::shared_future<void> future = threadDataItr->second.future;

			/*tell the thread to shutdown*/
			threadDataItr->second.parameters->SetShutdownFlag( true );

			if( maxWaitMilliseconds > -1 )
			{
				/*wait for the thread to be ready*/
				auto fStatus = future.wait_for( std::chrono::milliseconds( maxWaitMilliseconds ) );
				switch( fStatus )
				{
					case std::future_status::ready:
						{
							std::unique_ptr<Kiwi::ThreadParam> param = std::move( threadDataItr->second.parameters );
							m_threads.erase( threadDataItr );

							/*will re-throw any errors thrown by the thread*/
							future.get();

							return param;
						}
					case std::future_status::deferred:
					case std::future_status::timeout:
						{
							throw Kiwi::Exception( L"ThreadManager::JoinThread", L"Thread timed out", KIWI_EXCEPTION::TIMEOUT );
						}
					default: throw Kiwi::Exception( L"ThreadManager::JoinThread", L"Unknown thread error occured", KIWI_EXCEPTION::UNKNOWNERROR );
				}

			} else
			{
				std::unique_ptr<Kiwi::ThreadParam> param = std::move( threadDataItr->second.parameters );
				m_threads.erase( threadDataItr );

				/*will re-throw any errors thrown by the thread*/
				future.get();

				return param;
			}

		} else
		{
			throw Kiwi::Exception( L"ThreadManager::JoinThread", L"Invalid thread requested", KIWI_EXCEPTION::INVALIDTHREAD );
		}
	}

	/*attempts to join all running threads
	-maxWaitMilliseconds: if > -1, will throw TIMEOUT exception if a thread isn't joined in that amount of time, else function will block indefinitely for each thread
	-isSilent: if true, all exceptions thrown by joined threads will be suppressed, otherwise the function will re-throw the first exception thrown by a thread*/
	void ThreadManager::JoinAllThreads( int maxWaitMilliseconds, bool isSilent )
	{
		std::lock_guard<std::mutex> lockGuard( m_threadManagerMutex );

		for( auto threadItr = m_threads.begin(); threadItr != m_threads.end(); )
		{
			//copy the future, and remove it from the future map
			std::shared_future<void> future = threadItr->second.future;

			/*tell each thread to shutdown*/
			threadItr->second.parameters->SetShutdownFlag( true );

			if( maxWaitMilliseconds > -1 )
			{
				auto fStatus = future.wait_for( std::chrono::milliseconds( maxWaitMilliseconds ) );
				switch( fStatus )
				{
					case std::future_status::ready:
						{
							threadItr = m_threads.erase( threadItr );

							if( isSilent == false )
							{
								/*will re-throw any errors thrown by the thread*/
								future.get();

							} else
							{
								try
								{
									future.get();
								} catch( ... ) {}
							}
						}
					case std::future_status::deferred:
					case std::future_status::timeout:
						{
							throw Kiwi::Exception( L"ThreadManager::JoinThread", L"Thread timed out", KIWI_EXCEPTION::TIMEOUT );
						}
					default: throw Kiwi::Exception( L"ThreadManager::JoinThread", L"Unknown thread error occured", KIWI_EXCEPTION::UNKNOWNERROR );
				}
			} else
			{
				threadItr = m_threads.erase( threadItr );

				if( isSilent == false )
				{
					/*will re-throw any errors thrown by the thread*/
					future.get();

				} else
				{
					try
					{
						future.get();
					} catch( ... ) {}
				}
			}
		}
	}

	/*returns the current number of threads that are either running or haven't yet been joined*/
	unsigned int ThreadManager::GetActiveThreadCount()const
	{
		return (unsigned int)m_threads.size();
	}

}