#ifndef _KIWI_INPUTTRANSLATOR_H_
#define _KIWI_INPUTTRANSLATOR_H_

#include "Vector2d.h"

#include "../Events/IInputEventEmitter.h"

#include "../Types.h"

#include <unordered_map>
#include <unordered_set>

namespace Kiwi
{

	class RenderWindow;

	class InputTranslator:
		public Kiwi::IInputEventEmitter
	{
	protected:

		using KeyStatePair = std::pair<Kiwi::KEY, Kiwi::KEY_STATE>;
		using ButtonStatePair = std::pair<Kiwi::MOUSE_BUTTON, Kiwi::MOUSE_BUTTON_STATE>;

		struct pair_hash
		{
			template<class T1, class T2>
			std::size_t operator() ( const std::pair<T1, T2>& pair )const
			{
				auto h1 = std::hash<T1>()(pair.first);
				auto h2 = std::hash<T2>()(pair.second);

				return h1 ^ (h2 << 1);
			}
		};

		/*the key is a keyboard event, and the value is a set of strings that key is mapped to*/
		std::unordered_map<KeyStatePair, std::unordered_set<std::wstring>, pair_hash> m_keyboardMappings;

		/*the key is a mouse button, the value is is a set of strings that button is mapped to*/
		std::unordered_map<ButtonStatePair, std::unordered_set<std::wstring>, pair_hash> m_mouseMappings;

		/*set containing all the events that each mouse move event will generate*/
		std::unordered_set<std::wstring> m_mouseMoveMappings;

		/*set containing all the events that each mouse wheel event will generate*/
		std::unordered_set<std::wstring> m_mouseWheelMappings;

		Kiwi::Vector2d m_mouseSpeed;

	public:

		InputTranslator();

		void Update();

		/*maps a certain key state of a certain key to a specific event
		|-key: the key to be mapped
		|-keyState: the key state to be mapped (the event will be sent whenever the key is in this state)
			key states may be ORed together to add mappings for multiple states simultaneously
		|-evt: name of the mapping*/
		void AddKeyMapping( Kiwi::KEY key, Kiwi::KEY_STATE state, std::wstring evt );

		void AddMouseButtonMapping( Kiwi::MOUSE_BUTTON button, Kiwi::MOUSE_BUTTON_STATE state, std::wstring evt ); 

		void AddMouseMoveMapping( const std::wstring& evt );

		/*adds a mapping for the mouse wheel*/
		void AddMouseWheelMapping( const std::wstring& evt );

		void SetMouseSpeed( const Kiwi::Vector2d& speed ) { m_mouseSpeed = speed; }

		/*translates keyboard input into the correct mapping then creates and enqueues an event to be sent out next frame*/
		void TranslateAndSend( Kiwi::RenderWindow& source, Kiwi::KEY key, Kiwi::KEY_STATE state );

		/*translates mouse button input into the correct mapping then creates and enqueues an event to be sent out next frame*/
		void TranslateAndSend( Kiwi::RenderWindow& source, Kiwi::MOUSE_BUTTON button, Kiwi::MOUSE_BUTTON_STATE state, const Kiwi::Vector2d& position );

		/*translates mouse move input into the correct mapping then creates and enqueues an event to be sent out next frame*/
		void TranslateAndSend( Kiwi::RenderWindow& source, const Kiwi::Vector2d& position, const Kiwi::Vector2d& deltaPos );

		/*translates mouse wheel input into the correct mapping then creates and enqueues an event to be sent out next frame*/
		void TranslateAndSend( Kiwi::RenderWindow& source, const short wheelDelta );

		const Kiwi::Vector2d& GetMouseSpeed()const { return m_mouseSpeed; }

	};
}

#endif