#ifndef _KIWI_INPUTMANAGER_H_
#define _KIWI_INPUTMANAGER_H_

#define NUM_MOUSE_BUTTONS 5 //using buttons 1-5

#include "Vector2d.h"

#include "../Types.h"

#include <Windows.h>
#include <vector>
#include <deque>

namespace Kiwi
{
	class RenderWindow;

	class InputManager
	{
	protected:

		struct MouseState
		{
			MOUSE_BUTTON_STATE buttons[NUM_MOUSE_BUTTONS]; //stores the states of mouse buttons 1-5 + wheel
			short wheelDelta; //amount the scroll wheel was moved
			Kiwi::Vector2d delta; //change in position since last frame
			Kiwi::Vector2d pos; //current position of the mouse, relative to the center of the window
		};

		struct KeyState
		{
			KEY_STATE current; //the most recent state of the key
			KEY_STATE previous; //the previous state of the key
			bool changed; //whether the state changed in the last frame

			KeyState()
			{
				current = KEY_STATE::KEY_UP;
				previous = KEY_STATE::KEY_UP;
				changed = false;
			}
		};

		struct KeyStateChange
		{
			Kiwi::KEY key;
			bool state; //true if the key is up, false otherwise
		};

		Kiwi::RenderWindow* m_renderWindow;

		std::vector<KeyState> m_keyStates;

		/*stores changes to each key state which are then applyed to m_keyStates during the update cycle*/
		std::deque<KeyStateChange> m_keyStateChanges;

		Kiwi::Vector2d m_mouseSpeed;

		MouseState m_currentMouseState;

		MouseState m_mouseStateBuffer;

	protected:

		/*goes through all keys and updates the key states where needed*/
		void _UpdateKeyStates();

		void _UpdateMouseState();

	public:

		InputManager( Kiwi::RenderWindow& renderWindow );

		void InitializeRawInput();

		/*polls, updates, and resets the per-frame input data such as button held status, mouse position, delta movement, etc.*/
		void Update();

		void ProcessInput( const RAWINPUT& input );

		/*returns the current state of a mouse button
		possible values are:
		BUTTON_HELD: the button is currently being held down (and was down or pressed last frame)
		BUTTON_RELEASED: the button was released this frame
		BUTTON_PRESSED: the button was pressed this frame (and wasn't held last frame)
		BUTTON_UP: the button is not being pressed, and no button event occured this frame*/
		MOUSE_BUTTON_STATE GetMouseButtonState( MOUSE_BUTTON button ) { return m_currentMouseState.buttons[button]; }

		/*returns the current position of the mouse*/
		Kiwi::Vector2d GetMousePosition() { return m_currentMouseState.pos; }

		/*returns the amount the mouse has moved since the last tick*/
		const Kiwi::Vector2d& GetMouseDelta() { return m_currentMouseState.delta; }

		/*returns the amount the scroll wheel has turned since the last tick*/
		short GetMouseWheelDelta()const { return m_currentMouseState.wheelDelta; }

		/*returns true if the key has the specified state
		possible keyState values are:
		KEY_HELD: the key is currently being held down (and was down or pressed last frame)
		KEY_RELEASED: the key was released this frame
		KEY_PRESSED: the key was pressed this frame (and wasn't held last frame)
		KEY_UP: the key is not being pressed, and no key event occured this frame
		KEY_ACTIVE: the key is in either the KEY_PRESSED or KEY_HELD state
		KEY_INACTIVE: the key is in either the KEY_UP or KEY_RELEASED state*/
		bool GetKeyState( KEY key, KEY_STATE keyState );

	};

}

#endif