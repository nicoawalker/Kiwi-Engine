#include "InputTranslator.h"

namespace Kiwi
{

	InputTranslator::InputTranslator()
	{
		this->AddMouseButtonMapping( Kiwi::MOUSE_RIGHT, Kiwi::BUTTON_PRESSED, L"MouseRightPressed" );
		this->AddMouseButtonMapping( Kiwi::MOUSE_RIGHT, Kiwi::BUTTON_RELEASED, L"MouseRightReleased" );
		this->AddMouseButtonMapping( Kiwi::MOUSE_LEFT, Kiwi::BUTTON_PRESSED, L"MouseLeftPressed" );
		this->AddMouseButtonMapping( Kiwi::MOUSE_LEFT, Kiwi::BUTTON_RELEASED, L"MouseLeftReleased" );
		this->AddMouseMoveMapping( L"MouseMove" );
		this->AddMouseWheelMapping( L"MouseWheel" );
	}

	void InputTranslator::Update()
	{
		this->_EmitInputEventQueue();
	}

	void InputTranslator::AddKeyMapping( Kiwi::KEY key, Kiwi::KEY_STATE state, std::wstring evt )
	{
		/*state may be ORed so extract and add a mapping for any and all states*/
		int checkState = 2;
		while( checkState <= 128 )
		{
			KEY_STATE curState = state & static_cast<KEY_STATE>(checkState);
			checkState = (int)std::pow( checkState, 2 );
			if( curState == 0 ) continue;

			auto itr = m_keyboardMappings.find( std::make_pair( key, curState ) );
			if( itr != m_keyboardMappings.end() )
			{
				itr->second.insert( evt );

			} else
			{
				std::unordered_set<std::wstring> temp = { evt };
				m_keyboardMappings.insert( std::make_pair( std::make_pair( key, curState ), temp ) );
			}
		}
	}

	/*void InputTranslator::AddKeyMapping( Kiwi::KEY key, const std::vector<Kiwi::KEY_STATE>& keyStates, std::wstring evt )
	{
		for( auto stateItr = keyStates.begin(); stateItr != keyStates.end(); stateItr++ )
		{
			Kiwi::KEY_STATE state = *stateItr;
			auto itr = m_keyboardMappings.find( std::make_pair( key, state ) );
			if( itr != m_keyboardMappings.end() )
			{
				itr->second.insert( evt );

			} else
			{
				std::unordered_set<std::wstring> temp = { evt };
				m_keyboardMappings.insert( std::make_pair( std::make_pair( key, state ), temp ) );
			}
		}
	}*/

	void InputTranslator::AddMouseButtonMapping( Kiwi::MOUSE_BUTTON button, Kiwi::MOUSE_BUTTON_STATE state, std::wstring evt )
	{
		auto itr = m_mouseMappings.find( std::make_pair( button, state ) );
		if( itr != m_mouseMappings.end() )
		{
			itr->second.insert( evt );

		} else
		{
			std::unordered_set<std::wstring> temp = { evt };
			m_mouseMappings.insert( std::make_pair( std::make_pair( button, state ), temp ) );
		}
	}

	void InputTranslator::AddMouseMoveMapping( const std::wstring& evt )
	{
		m_mouseMoveMappings.insert( evt );
	}

	void InputTranslator::AddMouseWheelMapping( const std::wstring& evt )
	{
		m_mouseWheelMappings.insert( evt );
	}

	void InputTranslator::TranslateAndSend( Kiwi::RenderWindow& source, Kiwi::KEY key, Kiwi::KEY_STATE state )
	{
		auto itr = m_keyboardMappings.find( std::make_pair( key, state ) );
		if( itr != m_keyboardMappings.end() )
		{
			for( auto& eventString : itr->second )
			{
				this->EnqueueInputEvent( std::make_shared<Kiwi::InputEvent>( &source, eventString ) );
			}
		}
	}

	void InputTranslator::TranslateAndSend( Kiwi::RenderWindow& source, Kiwi::MOUSE_BUTTON button, Kiwi::MOUSE_BUTTON_STATE state, const Kiwi::Vector2d& position )
	{
		auto itr = m_mouseMappings.find( std::make_pair( button, state ) );
		if( itr != m_mouseMappings.end() )
		{
			for( auto& eventString : itr->second )
			{
				this->EnqueueInputEvent( std::make_shared<Kiwi::MouseEvent>( &source, eventString, position, Kiwi::Vector2d() ) );
			}
		}
	}

	void InputTranslator::TranslateAndSend( Kiwi::RenderWindow& source, const Kiwi::Vector2d& position, const Kiwi::Vector2d& deltaPos )
	{
		for( auto& eventString : m_mouseMoveMappings )
		{
			this->EnqueueInputEvent( std::make_shared<Kiwi::MouseEvent>( &source, eventString, position, Kiwi::Vector2d( deltaPos.x * m_mouseSpeed.x, deltaPos.y * m_mouseSpeed.y ) ) );
		}
	}

	void InputTranslator::TranslateAndSend( Kiwi::RenderWindow& source, const short wheelDelta )
	{
		for( auto& eventString : m_mouseMoveMappings )
		{
			this->EnqueueInputEvent( std::make_shared<Kiwi::MouseEvent>( &source, eventString, Kiwi::Vector2d(), Kiwi::Vector2d() ) );
		}
	}
}