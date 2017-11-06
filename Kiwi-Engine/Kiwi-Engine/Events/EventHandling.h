#ifndef _KIWI_EVENTHANDLING_H_
#define _KIWI_EVENTHANDLING_H_

#include "../Types.h"

#include "../Core/Exception.h"

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <deque>
#include <mutex>

namespace Kiwi
{

	class AssetEvent;
	class SceneEvent;
	class WindowEvent;
	class TransformEvent;
	class FrameEvent;
	class CollisionEvent;
	class InputEvent;
	class UIButtonEvent;
	class RenderEvent;

	enum EventResponse { NONE, DISCONNECT_THIS };

	template<typename EventType>
	class IEventReceiver
	{
		template<typename> friend class IEventEmitter;
	private:

		std::unordered_set<IEventEmitter<EventType>*> m_connectedEmitters;

		std::mutex m_eventReceiver_M;

		bool m_isShutdown;

	private:

		void _ConnectEmitter( IEventEmitter<EventType>& emitter );

		void _DisconnectEmitter( IEventEmitter<EventType>& emitter );

	public:

		IEventReceiver():
			m_isShutdown(false){ }

		virtual ~IEventReceiver();

	};

	using IAssetEventReceiver = Kiwi::IEventReceiver<AssetEvent>;
	using ISceneEventReceiver = Kiwi::IEventReceiver<SceneEvent>;
	using IWindowEventReceiver = Kiwi::IEventReceiver<WindowEvent>;
	using ITransformEventReceiver = Kiwi::IEventReceiver<TransformEvent>;
	using IFrameEventReceiver = Kiwi::IEventReceiver<FrameEvent>;
	using ICollisionEventReceiver = Kiwi::IEventReceiver<CollisionEvent>;
	using IInputEventReceiver = Kiwi::IEventReceiver<InputEvent>;
	using IUIButtonEventReceiver = Kiwi::IEventReceiver<UIButtonEvent>;
	using IUIRenderEventReceiver = Kiwi::IEventReceiver<RenderEvent>;

	template<typename EventType>
	class IEventEmitter
	{
		template<typename> friend class IEventReceiver;

		using CallbackFunction = std::function<EventResponse( std::shared_ptr<EventType> )>;
		using ReceiverCallbackPair = std::pair<IEventReceiver<EventType>*, CallbackFunction>;

	private:

		struct CallbackInfo
		{
			std::vector<std::wstring> categories;
			IEventReceiver<EventType>* receiver;
			CallbackFunction callback;
			bool toBeRemoved;

			CallbackInfo( const std::vector<std::wstring> categories, IEventReceiver<EventType>* receiver, CallbackFunction callback, bool remove )
			{
				this->categories = categories;
				this->receiver = receiver;
				this->callback = callback;
				toBeRemoved = remove;
			}
		};

		/*stores the registered function pointers for each event type
		the map key is a string representing the event type
		the map value is an unordered_map with a pointer to an event receiver as a key, and that receivers callback function pointer as the value*/
		std::unordered_map<std::wstring, std::unordered_map<IEventReceiver<EventType>*, CallbackFunction>> m_eventCallbacks;

		std::vector<CallbackInfo> m_callbackBuffer;

		std::deque<std::pair<std::wstring, std::shared_ptr<EventType>>> m_eventQueue;

		std::recursive_mutex m_eventEmitter_M;

		/*set to true when the emitter is sending events to disable the ability to disconnect receivers*/
		bool m_emitting;
		bool m_updateBuffer;

	private:

		/*stores a pointer to a receiver along with its callback function, if the receiver isn't already registered for this event type*/
		//void _ConnectReceiver( IEventReceiver<EventType>* receiver, const std::vector<std::wstring>& eventCategories, CallbackFunction eventCallback );

		void _ProcessBuffer();

	protected:

		/*adds an event category that receivers can connect to*/
		void _AddCategory( const std::wstring& cat );

		/*emits all events currently in the event queue*/
		void _EmitEventQueue();

		/*disconnects the receiver from all events it may be connected to*/
		void _Disconnect( IEventReceiver<EventType>& receiver );

		/*disconnects the receiver from a specific event it may be connected to*/
		void _Disconnect( IEventReceiver<EventType>& receiver, const std::wstring& eventCategory );

		/*disconnects the receiver each event in a list of events it may be connected to*/
		void _Disconnect( IEventReceiver<EventType>& receiver, const std::vector<std::wstring>& eventCategories );

		/*immediately sends an event of a certain type to all callback functions connected to that event type*/
		void _EmitEvent( const std::wstring eventCategory, std::shared_ptr<EventType> evt );

		void _EnqueueEvent( const std::wstring eventCategory, std::shared_ptr<EventType> evt );

		void _Connect( IEventReceiver<EventType>& receiver, const std::wstring& eventCategory, std::function<Kiwi::EventResponse( std::shared_ptr<EventType> )> func );

		void _Connect( IEventReceiver<EventType>& receiver, const std::vector<std::wstring>& eventCategories, std::function<Kiwi::EventResponse( std::shared_ptr<EventType> )> func );

		/*connects a member function of class CallbackClass to a specific event type so that this callback function will be evoked whenever
		an event of this type is emitted
		>receiver: reference to the object that owns the callback function
		>eventCategory: type of event the receiver will be connected to
		>callbackFunction: callback member function of form: Kiwi::EventResponse CallbackClass::CallbackFunction(std::shared_ptr<EventType>)*/
		template<typename CallbackClass>
		void _Connect( CallbackClass& receiver, const std::wstring& eventCategory, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<EventType>) )
		{
			std::unique_lock<std::recursive_mutex> ulock( m_eventEmitter_M );

			Kiwi::IEventReceiver<EventType>* recCast = dynamic_cast<IEventReceiver<EventType>*>(&receiver);
			if( recCast == 0 ) return;

			m_callbackBuffer.push_back( CallbackInfo( std::initializer_list<std::wstring>( { eventCategory } ), &receiver, std::bind( callbackFunction, &receiver, std::placeholders::_1 ), false ) );
			m_updateBuffer = true;
			recCast->_ConnectEmitter( *this );
		}

		/*connects a member function of class CallbackClass to a list of event types so that this callback function will be evoked whenever
		any event in the list is emitted
		>receiver: reference to the object that owns the callback function
		>eventCategorys: a vector containing all of the event types the receiver will be connected to
		>callbackFunction: callback member function of form: Kiwi::EventResponse CallbackClass::CallbackFunction(std::shared_ptr<EventType>)*/
		template<typename CallbackClass>
		void _Connect( CallbackClass& receiver, const std::vector<std::wstring>& eventCategories, EventResponse( CallbackClass::*callbackFunction )(std::shared_ptr<EventType>) )
		{
			std::unique_lock<std::recursive_mutex> ulock( m_eventEmitter_M );

			Kiwi::IEventReceiver<EventType>* recCast = dynamic_cast<IEventReceiver<EventType>*>(&receiver);
			if( recCast == 0 ) return;

			m_callbackBuffer.push_back( CallbackInfo( eventCategories, &receiver, std::bind( callbackFunction, &receiver, std::placeholders::_1 ), false ) );
			m_updateBuffer = true;
			recCast->_ConnectEmitter( *this );
		}

	public:

		IEventEmitter():
			m_updateBuffer(false){ }
		IEventEmitter( const std::vector<std::wstring>& eventCategories );
		virtual ~IEventEmitter();

	};

	template<typename EventType>
	IEventReceiver<EventType>::~IEventReceiver()
	{
		m_isShutdown = true;

		for( auto* emitter : m_connectedEmitters )
		{
			emitter->_Disconnect( *this );
		}
	}

	template<typename EventType>
	void IEventReceiver<EventType>::_ConnectEmitter( IEventEmitter<EventType>& emitter )
	{
		std::unique_lock<std::mutex> ulock( m_eventReceiver_M );

		m_connectedEmitters.insert( &emitter );
	}

	template<typename EventType>
	void IEventReceiver<EventType>::_DisconnectEmitter( IEventEmitter<EventType>& emitter )
	{
		std::unique_lock<std::mutex> ulock( m_eventReceiver_M );

		if( m_isShutdown ) return;

		auto itr = m_connectedEmitters.find( &emitter );
		if( itr != m_connectedEmitters.end() )
		{
			m_connectedEmitters.erase( &emitter );
		}
	}

	template<typename EventType>
	IEventEmitter<EventType>::IEventEmitter( const std::vector<std::wstring>& eventCategories )
	{
		for( auto& category : eventCategories )
		{
			std::unordered_map<IEventReceiver<EventType>*, CallbackFunction> temp;
			m_eventCallbacks.insert( std::make_pair( category, temp ) );
		}

		m_updateBuffer = false;
	}

	template<typename EventType>
	IEventEmitter<EventType>::~IEventEmitter() 
	{
		for( auto& eventMapPair : m_eventCallbacks )
		{
			for( auto& recvCallbackPair : eventMapPair.second )
			{
				recvCallbackPair.first->_DisconnectEmitter( *this );
			}
		}
	}

	template<typename EventType>
	void IEventEmitter<EventType>::_ProcessBuffer()
	{
		for( auto& info : m_callbackBuffer )
		{
			if( info.toBeRemoved )
			{/*remove the receiver*/
				if( info.categories.size() == 0 )
				{/*no categories specified so the receiver is to be removed from all of them
				 loop through every event type and remove all of the receiver's callbacks*/
					for( auto& callbackMap : m_eventCallbacks )
					{
						auto recvItr = callbackMap.second.find( info.receiver );
						if( recvItr != callbackMap.second.end() )
						{
							callbackMap.second.erase( recvItr );
						}
					}
					
				} else
				{/*remove the receiver only from the specified categories*/
					for( auto& category : info.categories )
					{
						bool allErased = true;

						auto itr = m_eventCallbacks.find( category );
						if( itr != m_eventCallbacks.end() )
						{
							auto recvItr = itr->second.find( info.receiver );
							if( recvItr != itr->second.end() )
							{
								itr->second.erase( recvItr );
							}
						}
					}
				}

			} else
			{/*add the receiver to any category it isn't already attached to*/
				for( auto& category : info.categories )
				{
					auto itr = m_eventCallbacks.find( category );
					if( itr != m_eventCallbacks.end() )
					{
						auto callbackItr = itr->second.find( info.receiver );
						if( callbackItr == itr->second.end() )
						{
							itr->second.insert( std::make_pair( info.receiver, info.callback ) );
						}

					} else
					{
						std::unordered_map<IEventReceiver<EventType>*, CallbackFunction> temp;
						temp.insert( std::make_pair( info.receiver, info.callback ) );
						m_eventCallbacks.insert( std::make_pair( category, temp ) );
					}
				}
			}
		}

		m_callbackBuffer.clear();

		m_updateBuffer = false;
	}

	template<typename EventType>
	void IEventEmitter<EventType>::_AddCategory( const std::wstring& cat )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_eventEmitter_M );

		std::unordered_map<IEventReceiver<EventType>*, CallbackFunction> temp;
		m_eventCallbacks.insert( std::make_pair( cat, temp ) );
	}

	template<typename EventType>
	void IEventEmitter<EventType>::_EmitEventQueue()
	{
		std::unique_lock<std::recursive_mutex> ulock( m_eventEmitter_M );

		if( m_updateBuffer ) this->_ProcessBuffer();

		while( m_eventQueue.size() > 0 )
		{
			this->_EmitEvent( m_eventQueue.front().first, m_eventQueue.front().second );
			m_eventQueue.pop_front();
		}
	}

	template<typename EventType>
	void IEventEmitter<EventType>::_Disconnect( IEventReceiver<EventType>& receiver )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_eventEmitter_M );

		std::vector<std::wstring> temp;
		m_callbackBuffer.push_back( CallbackInfo( temp, &receiver, nullptr, true ) );
		m_updateBuffer = true;
		receiver._DisconnectEmitter( *this );
	}

	template<typename EventType>
	void IEventEmitter<EventType>::_Disconnect( IEventReceiver<EventType>& receiver, const std::wstring& eventCategory )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_eventEmitter_M );

		m_callbackBuffer.push_back( CallbackInfo( std::initializer_list<std::wstring>( { eventCategory } ), &receiver, nullptr, true ) );
		m_updateBuffer = true;

		/*do a search to find out if there are more event categories the receiver is connected to. if not, disconnect the receiver*/
		bool allErased = true;
		for( auto& keyValue : m_eventCallbacks )
		{
			if( keyValue.first.compare( eventCategory ) == 0 ) continue; //skip the category it's being removed from

			auto itr = keyValue.second.find( &receiver );
			if( itr != keyValue.second.end() )
			{
				allErased = false;
			}
		}
		if( allErased ) receiver._DisconnectEmitter( *this );
	}

	template<typename EventType>
	void IEventEmitter<EventType>::_Disconnect( IEventReceiver<EventType>& receiver, const std::vector<std::wstring>& eventCategories )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_eventEmitter_M );

		m_callbackBuffer.push_back( CallbackInfo( eventCategories, &receiver, nullptr, true ) );
		m_updateBuffer = true;

		/*do a search to find out if there are more event categories the receiver is connected to. if not, disconnect the receiver*/
		bool allErased = true;
		for( auto& keyValue : m_eventCallbacks )
		{
			bool skip = false;
			for( auto& cat : eventCategories )
			{
				if( keyValue.first.compare( cat ) == 0 ) skip = true; //skip the category it's being removed from
			}
			if( skip ) continue;

			auto itr = keyValue.second.find( &receiver );
			if( itr != keyValue.second.end() )
			{
				allErased = false;
			}
		}
		if( allErased ) receiver._DisconnectEmitter( *this );
	}

	template<typename EventType>
	void IEventEmitter<EventType>::_EmitEvent( const std::wstring eventCategory, std::shared_ptr<EventType> evt )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_eventEmitter_M );

		if( evt == nullptr ) return;
		
		if( m_updateBuffer ) this->_ProcessBuffer();

		auto itr = m_eventCallbacks.find( eventCategory );
		if( itr != m_eventCallbacks.end() )
		{
			for( auto callbackItr = itr->second.begin(); callbackItr != itr->second.end(); )
			{
				if( callbackItr->second( evt ) == EventResponse::DISCONNECT_THIS )
				{
					callbackItr = itr->second.erase( callbackItr );
					continue;
				}
				callbackItr++;
			}
		}
	}

	template<typename EventType>
	void IEventEmitter<EventType>::_EnqueueEvent( const std::wstring eventCategory, std::shared_ptr<EventType> evt )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_eventEmitter_M );

		m_eventQueue.push_back( std::make_pair( eventCategory, evt ) );
	}

	template<typename EventType>
	void IEventEmitter<EventType>::_Connect( IEventReceiver<EventType>& receiver, const std::wstring& eventCategory, std::function<Kiwi::EventResponse( std::shared_ptr<EventType> )> func )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_eventEmitter_M );

		m_callbackBuffer.push_back( CallbackInfo( std::initializer_list<std::wstring>( { eventCategory } ), &receiver, func, false ) );
		m_updateBuffer = true;
		receiver._ConnectEmitter( *this );
	}

	template<typename EventType>
	void IEventEmitter<EventType>::_Connect( IEventReceiver<EventType>& receiver, const std::vector<std::wstring>& eventCategories, std::function<Kiwi::EventResponse( std::shared_ptr<EventType> )> func )
	{
		std::unique_lock<std::recursive_mutex> ulock( m_eventEmitter_M );

		m_callbackBuffer.push_back( CallbackInfo( eventCategories, &receiver, func, false ) );
		m_updateBuffer = true;
		receiver._ConnectEmitter( *this );
	}

}

#endif