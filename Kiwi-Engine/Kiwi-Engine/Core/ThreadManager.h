#ifndef _KIWI_THREADMANAGER_H_
#define _KIWI_THREADMANAGER_H_

#include "Exception.h"
#include "ThreadParam.h"

#include <thread>
#include <future>
#include <unordered_map>
#include <memory>
#include <string>

namespace Kiwi
{

	enum ThreadStatus { THREAD_INVALID, THREAD_READY, THREAD_DEFERRED, THREAD_TIMEOUT, THREAD_UNKNOWN };

	class ThreadManager
	{
	protected:

		struct ThreadData
		{
			std::shared_future<void> future;
			std::unique_ptr<Kiwi::ThreadParam> parameters;

			ThreadData()
			{
				parameters = nullptr;
			}

			ThreadData( ThreadData& other )
			{
				future = other.future;
				parameters = std::move( other.parameters );
			}
		};

		struct ThreadKey
		{
			/*unique name and ID number of the thread, one or both are required*/
			std::wstring name;
			long id;

			ThreadKey( const ThreadKey& other )
			{
				name = other.name;
				id = other.id;
			}

			ThreadKey( long threadID, std::wstring threadName = L"" )
			{
				name = threadName;
				id = threadID;
			}

			bool operator== ( const ThreadKey& other ) const
			{
				return (id == other.id ||
						 (name.compare( L"" ) != 0 && other.name.compare( L"" ) != 0 && name.compare( other.name ) == 0));
			}

		};

		struct ThreadKeyHash
		{
			std::size_t operator() ( const ThreadKey& key )const
			{
				return (std::hash<std::wstring>()(key.name)) ^ ((std::hash<long>()(key.id) << 1) >> 1);
			}
		};

		std::unordered_map<int, ThreadData> m_test;
		std::unordered_map<ThreadKey, ThreadData, ThreadKeyHash> m_threads;

		std::mutex m_threadManagerMutex;

	public:

		ThreadManager()
		{
		}

		~ThreadManager() 
		{
		}

		/*creates a new thread that has no return value and takes no parameters
		returns an empty thread parameter class that contains the unique ID number of the generated thread
		the returned thread parameter is owned by the thread manager until the thread is joined, at which time ownership is transferred to the joining function*/
		Kiwi::ThreadParam* CreateThread( void( *threadFunction )(void) );

		/*creates a new named thread that has no return value and takes no parameters
		returns an empty thread parameter class that contains the unique name and ID number of the generated thread
		the returned thread parameter is owned by the thread manager until the thread is joined, at which time ownership is transferred to the joining function*/
		Kiwi::ThreadParam* CreateThread( std::wstring uniqueThreadName, void( *threadFunction )(void) );

		/*creates a new thread that takes one parameter of type Kiwi::IThreadParam and has no return value
		the thread will be created using the unique ID of the thread parameter, and the unique name if it is set
		thread parameter ownership is passed to the thread manager and is returned when the thread is joined, and deleted if the thread throws an exception when joined*/
		void CreateThread( void( *threadFunction )(Kiwi::ThreadParam*), std::unique_ptr<Kiwi::ThreadParam> threadParameter );

		/*creates a new thread using a class member function that takes no parameters and has no return value
		returns an empty thread parameter class that contains the unique ID number of the generated thread
		the returned thread parameter is owned by the thread manager until the thread is joined, at which time ownership is transferred to the joining function*/
		template<typename ParentClass>
		Kiwi::ThreadParam* CreateThread( ParentClass* parentClass, void(ParentClass::*threadFunction)(void) )
		{

			std::lock_guard<std::mutex> lockGuard( m_threadManagerMutex );

			ThreadData data;
			data.parameters = std::make_unique<Kiwi::ThreadParam>();
			data.future = std::async( std::launch::async, threadFunction, parentClass );

			m_threads.insert( std::make_pair( ThreadKey( data.parameters->GetUniqueID() ), data ) );

			Kiwi::ThreadParam* p = data.parameters.get();

			return p;

		}

		/*creates a new thread using a class member function that takes one parameter and has no return value
		the thread will be created using the unique ID of the thread parameter, and the unique name if it is set
		thread parameter ownership is passed to the thread manager and is returned when the thread is joined, and deleted if the thread throws an exception when joined*/
		template<typename ParentClass>
		void CreateThread( ParentClass* parentClass, void(ParentClass::*threadFunction)(Kiwi::ThreadParam*), std::unique_ptr<Kiwi::ThreadParam> threadParameter )
		{

			std::lock_guard<std::mutex> lockGuard( m_threadManagerMutex );

			if( threadParameter == 0 ) return;

			ThreadData data;
			data.parameters = std::move( threadParameter );
			data.future = std::async( std::launch::async, threadFunction, parentClass, data.parameters.get() );

			m_threads.insert( std::make_pair( ThreadKey( data.parameters->GetUID(), data.parameters->GetName() ), data ) );

		}

		/*returns the current status of the thead with the given thread ID, or returns THREAD_INVALID if there is no thread with matching ID*/
		ThreadStatus CheckThread( long threadID );

		/*returns the current status of the thead with the given name, or returns THREAD_INVALID if there is no thread with matching name*/
		ThreadStatus CheckThread( std::wstring threadName );

		/*attemps to join the thread with the matching name and sets the thread's shutdown flag. if no matching thread is found will throw INVALIDTHREAD
		if the thread is joined this function will re-throw that exception, otherwise this function will return ownership of the thread's parameter
		-maxWaitMilliseconds: if > 0, will throw TIMEOUT exception if the thread isn't joined in that amount of time, else function will block until it is joined*/
		std::unique_ptr<Kiwi::ThreadParam> JoinThread( long threadID, int maxWaitMilliseconds = -1 );

		/*attemps to join the thread with the matching name and sets the thread's shutdown flag. if no matching thread is found will throw INVALIDTHREAD
		if the thread is joined this function will re-throw that exception, otherwise this function will return ownership of the thread's parameter
		-maxWaitMilliseconds: if > 0, will throw TIMEOUT exception if the thread isn't joined in that amount of time, else function will block until it is joined*/
		std::unique_ptr<Kiwi::ThreadParam> JoinThread( std::wstring threadName, int maxWaitMilliseconds = -1 );

		/*attempts to join all running threads, and sets each thread's shutdown flag
		-maxWaitMilliseconds: if set, will throw TIMEOUT exception if a thread isn't joined in that amount of time, else function will block indefinitely for each thread
		-isSilent: if true, all exceptions thrown by joined threads will be suppressed, otherwise the function will re-throw the first exception thrown by a thread*/
		void JoinAllThreads( int maxWaitMilliseconds = -1, bool isSilent = false );

		/*returns the current number of threads that are either running or haven't yet been joined*/
		unsigned int GetActiveThreadCount()const;


		/*spawns a new thread that takes no parameters and returns a value of type ThreadReturnType
		returns the threadID of the new thread*/
		/*unsigned int SpawnThread( ThreadReturnType (*threadFunction)(void) )
		{

			m_threadID += 1;
			m_futures[m_threadID] = std::async( std::launch::async, threadFunction );

			return m_threadID;

		}*/

		/*spawns a new thread that takes a parameter of type ThreadParameterType and returns a value of type ThreadReturnType
		returns the threadID of the new thread*/
		/*template<typename ThreadParameterType>
		unsigned int SpawnThread( ThreadReturnType ( *threadFunction )(ThreadParameterType), ThreadParameterType parameter )
		{

			m_threadID += 1;
			m_futures[m_threadID] = std::async( std::launch::async, threadFunction, parameter );

			return m_threadID;

		}*/

		/*spawns a new thread using a member function of class FunctionClass, that takes no parameters
		returns the threadID of the new thread*/
		/*template<typename FunctionClass>
		unsigned int SpawnThread( FunctionClass* fClass, ThreadReturnType ( FunctionClass::*threadFunction )(void) )
		{

			m_threadID += 1;
			m_futures[m_threadID] = std::async( std::launch::async, threadFunction, fClass );

			return m_threadID;

		}*/

		/*spawns a new thread using a member function of class FunctionClass
		returns the threadID of the new thread*/
		/*template<typename FunctionClass, typename ThreadParameterType>
		unsigned int SpawnThread( FunctionClass* fClass, ThreadReturnType ( FunctionClass::*threadFunction )(ThreadParameterType), ThreadParameterType parameter )
		{

			m_threadID += 1;
			m_futures[m_threadID] = std::async( std::launch::async, threadFunction, fClass, parameter );
			return m_threadID;

		}*/

		/*returns the status of the requested thread
		if threadID is invalid it will throw*/
		/*ThreadStatus GetThreadStatus( unsigned int threadID )
		{

			auto futureItr = m_futures.find( threadID );
			if( futureItr != m_futures.end() )
			{
				auto fStatus = futureItr->second.wait_for( std::chrono::milliseconds( 0 ) );
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
				throw Kiwi::Exception( L"ThreadManager::GetThreadStatus", L"Thread ID is invalid: " + Kiwi::ToWString( threadID ), KIWI_EXCEPTION::INVALIDINDEX );
			}

		}*/

		/*calls get() on the thread's future
		if the thread has not finished this function will block until the thread finishes
		if the thread threw an exception this function will re-throw it
		returns the value returned by the function
		if threadID is invalid it will throw*/
		//ThreadReturnType GetThread( unsigned int threadID )
		//{

		//	auto futureItr = m_futures.find( threadID );
		//	if( futureItr != m_futures.end() )
		//	{
		//		//copy the future, and remove it from the future map
		//		std::shared_future<ThreadReturnType> future = futureItr->second;
		//		
		//		futureItr = m_futures.erase( futureItr );

		//		//if all threads are complete, reset the thread ID
		//		if( m_futures.size() == 0 )
		//		{
		//			m_threadID = 0;
		//		}

		//		/*gets the value returned by the thread. if the thread is not finished, blocks until it is done
		//		if the thread threw an exception, this will re-throw it*/
		//		return future.get();

		//	} else
		//	{
		//		throw Kiwi::Exception( L"ThreadManager::GetThreadStatus", L"Thread ID is invalid: " + Kiwi::ToWString( threadID ), KIWI_EXCEPTION::INVALIDINDEX );
		//	}

		//}

		/*joins all currently running threads, blocking until this is done
		if maxWaitTime is > 0, this function will wait for each thread only for the specified time, in milliseconds
		any exceptions thrown by the threads are ignored*/
		/*void JoinAll( int maxWaitTime = -1 )
		{

			for( auto futureItr = m_futures.begin(); futureItr != m_futures.end(); )
			{
				if( futureItr->second.valid() )
				{
					if( maxWaitTime > 0 )
					{
						futureItr->second.wait_for( std::chrono::milliseconds( maxWaitTime ) );

					} else
					{
						futureItr->second.wait();
					}
				}

				futureItr = m_futures.erase( futureItr );
			}

		}*/

	};
}

#endif