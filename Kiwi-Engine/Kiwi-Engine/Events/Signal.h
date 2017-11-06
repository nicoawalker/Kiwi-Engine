#ifndef _KIWI_SIGNAL_H_
#define _KIWI_SIGNAL_H_

#include <vector>
#include <memory>
#include <functional>
#include <mutex>

namespace Kiwi
{
	/*a SignalConnection provides a way for classes that register anonymous functions with a signal
	to disconnect those functions easily. a SignalConnection is generated for every registered function
	and calling the connection's Disconnect() method will ensure that the function is not called again*/
	struct SignalConnection
	{
		template<typename FunctionDef> friend class Signal;
	private:

		bool m_disconnect;

	public:

		SignalConnection()
		{
			m_disconnect = false;
		}

		void Disconnect() { m_disconnect = true; }

		void operator= ( const SignalConnection& con )
		{
			m_disconnect = con.m_disconnect;
		}
	};

	/*manager class that allows objects to store signal connections that will automatically 
	disconnect when they go out of scope*/
	struct ScopedSignalConnection
	{
	private:

		std::weak_ptr<SignalConnection> scopedConnection;

	public:

		ScopedSignalConnection() {}

		ScopedSignalConnection( std::weak_ptr<SignalConnection> connection )
		{
			scopedConnection = connection;
		}

		~ScopedSignalConnection()
		{
			if( auto sharedCon = scopedConnection.lock() )
			{
				sharedCon->Disconnect();
			}
		}

		void Disconnect()
		{
			if( auto sharedCon = scopedConnection.lock() )
			{
				sharedCon->Disconnect();
			}
		}

		ScopedSignalConnection& operator= ( const std::weak_ptr<SignalConnection> connection )
		{
			this->Disconnect();
			scopedConnection = connection;
			return *this;
		}
	};

	template<typename FunctionDef>
	class Signal
	{
		using Callback = std::function<FunctionDef>;
	private:

		struct SignalSlotConnectionPair
		{
			std::shared_ptr<SignalConnection> connection;
			Callback callback;

			SignalSlotConnectionPair( std::shared_ptr<SignalConnection> connection, Callback callback )
			{
				this->connection = connection;
				this->callback = callback;
			}

			SignalSlotConnectionPair( const SignalSlotConnectionPair& sscp )
			{
				connection = sscp.connection;
				callback = sscp.callback;
			}
		};

	protected:

		std::vector<SignalSlotConnectionPair> m_slots;

		std::vector<SignalSlotConnectionPair> m_slotBuffer;

		std::recursive_mutex m_signal_Mx;

	private:

		std::weak_ptr<SignalConnection> _Connect( std::function<FunctionDef> callback )
		{
			m_slotBuffer.push_back( SignalSlotConnectionPair( std::make_shared<SignalConnection>(), std::move( callback ) ) );
			return m_slotBuffer.back().connection;
		}

	public:

		Signal() {}

		/*registers a callback function that will be invoked whenever a new message is sent
		|-callback: call back function to register
		|-returns: weak pointer to a SignalConnection object that is used by the registering
			class to disconnect itself from this signal when needed*/
		std::weak_ptr<SignalConnection> Connect( std::function<FunctionDef> callback )
		{
			std::lock_guard<std::recursive_mutex> guard( m_signal_Mx );

			return this->_Connect( std::move( callback ) );
		}

		std::weak_ptr<SignalConnection> operator+=( std::function<FunctionDef> callback )
		{
			std::lock_guard<std::recursive_mutex> guard( m_signal_Mx );

			return this->_Connect( std::move( callback ) );
		}

		/*emits a message to all registered callbacks.
		newly added callbacks will be placed in the main receiver list before any message is sent
		disconnected callbacks will be removed automatically*/
		template<class ...Args>
		void Emit( Args... args )
		{
			std::lock_guard<std::recursive_mutex> guard( m_signal_Mx );

			/*add callbacks in the buffer to the main slot list*/
			for( auto itr = m_slotBuffer.begin(); itr != m_slotBuffer.end(); itr++ )
			{
				m_slots.push_back( *itr );
			}
			m_slotBuffer.clear();

			//send the message to the callback function of each slot
			for( auto itr = m_slots.begin(); itr != m_slots.end(); )
			{
				if( (*itr).connection->m_disconnect == true )
				{
					/*remove disconnected connections*/
					itr = m_slots.erase( itr );

				} else
				{
					(*itr).callback( std::forward<Args>( args )... );
					itr++;
				}
			}
		}

	};
}

#endif