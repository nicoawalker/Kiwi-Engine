#include "InputManager.h"
#include "Exception.h"

#include "..\Graphics\RenderWindow.h"
#include "../Core/EngineRoot.h"

namespace Kiwi
{

	InputManager::InputManager( Kiwi::RenderWindow& renderWindow )
	{

		m_renderWindow = &renderWindow;

		m_mouseSpeed.Set( 1.0, 1.0 );

		/*create all key states, which are initialized to 'up'*/
		m_keyStates.resize( 256 );

	}

	void InputManager::_UpdateKeyStates()
	{
		/*go through and apply all changes to key states*/
		while( m_keyStateChanges.size() > 0 )
		{
			KeyStateChange change = m_keyStateChanges.front();

			KEY_STATE currentState = m_keyStates[change.key].current;
			KEY_STATE newState = currentState;

			if( change.state )
			{
				//key was released, only need to update if it was down previously
				if( currentState == KEY_STATE::KEY_PRESSED || currentState == KEY_STATE::KEY_HELD )
				{
					newState = KEY_STATE::KEY_RELEASED;
				}

			} else
			{
				//key was pressed or down
				if( currentState == KEY_STATE::KEY_UP || currentState == KEY_STATE::KEY_RELEASED )
				{
					newState = KEY_STATE::KEY_PRESSED;
				}
			}

			m_keyStates[change.key].current = newState;
			m_keyStates[change.key].previous = currentState;

			if( newState != currentState )
			{
				m_keyStates[change.key].changed = true;
			}

			m_keyStateChanges.pop_front();
		}

		//go through and update all keys that are being held or released
		unsigned int key = 0;
		for( auto& keyState : m_keyStates )
		{
			if( keyState.current == KEY_STATE::KEY_PRESSED && keyState.previous == KEY_STATE::KEY_PRESSED )
			{
				//if a key pressed last frame is still pressed, change the status to held
				keyState.current = KEY_STATE::KEY_HELD;

			} else if( keyState.current == KEY_STATE::KEY_RELEASED && keyState.previous == KEY_STATE::KEY_RELEASED )
			{
				//if a key released last frame is still released, change the status to up
				keyState.current = KEY_STATE::KEY_UP;
			}

			if( keyState.changed || keyState.current != keyState.previous )
			{
				_kInput.TranslateAndSend( *m_renderWindow, static_cast<Kiwi::KEY>(key), keyState.current );

			} else if( keyState.current == KEY_HELD ) //also want to constantly trigger events when a key is held
			{
				_kInput.TranslateAndSend( *m_renderWindow, static_cast<Kiwi::KEY>(key), keyState.current );
			}

			//set the previous state to the current state
			keyState.previous = keyState.current;

			keyState.changed = false;
			key++;
		}

	}

	void InputManager::_UpdateMouseState()
	{

		//first get and set the current position of the mouse
		POINT mousePos;
		GetCursorPos( &mousePos ); //get cursor position in screen coords
		ScreenToClient( m_renderWindow->GetHandle(), &mousePos ); //transform screen coords into local window coords

		//convert so that 0,0 is at the center of the window
		mousePos.y -= LONG( m_renderWindow->GetClientSize().y / 2.0 );
		mousePos.y *= -1;
		mousePos.x -= LONG( m_renderWindow->GetClientSize().x / 2.0 );

		MouseState lastMouseState = m_currentMouseState;

		//set the delta movement
		m_currentMouseState.delta.Set( m_mouseStateBuffer.delta.x, m_mouseStateBuffer.delta.y );

		//set the new position
		m_currentMouseState.pos.Set( (double)mousePos.x, (double)mousePos.y );

		if( m_currentMouseState.delta != lastMouseState.delta || m_currentMouseState.pos != lastMouseState.pos )
		{
			_kInput.TranslateAndSend( *m_renderWindow, m_currentMouseState.pos, m_currentMouseState.delta );
		}

		//set the wheel delta
		m_currentMouseState.wheelDelta = m_mouseStateBuffer.wheelDelta;

		if( m_currentMouseState.wheelDelta != lastMouseState.wheelDelta )
		{
			_kInput.TranslateAndSend( *m_renderWindow, m_currentMouseState.wheelDelta );
		}

		//update the mouse buttons
		for( unsigned int i = 0; i < NUM_MOUSE_BUTTONS; i++ )
		{
			if( m_mouseStateBuffer.buttons[i] == MOUSE_BUTTON_STATE::BUTTON_PRESSED )
			{
				if( m_currentMouseState.buttons[i] == MOUSE_BUTTON_STATE::BUTTON_PRESSED || m_currentMouseState.buttons[i] == MOUSE_BUTTON_STATE::BUTTON_HELD )
				{
					m_currentMouseState.buttons[i] = MOUSE_BUTTON_STATE::BUTTON_HELD;

				} else
				{
					m_currentMouseState.buttons[i] = MOUSE_BUTTON_STATE::BUTTON_PRESSED;
				}

			} else
			{
				if( m_currentMouseState.buttons[i] == MOUSE_BUTTON_STATE::BUTTON_RELEASED || m_currentMouseState.buttons[i] == MOUSE_BUTTON_STATE::BUTTON_UP )
				{
					m_currentMouseState.buttons[i] = MOUSE_BUTTON_STATE::BUTTON_UP;

				} else
				{
					m_currentMouseState.buttons[i] = MOUSE_BUTTON_STATE::BUTTON_RELEASED;
				}
			}

			/*send a mouse event for any mouse buttons that have changed*/
			if( m_currentMouseState.buttons[i] != lastMouseState.buttons[i] )
			{
				_kInput.TranslateAndSend( *m_renderWindow, static_cast<MOUSE_BUTTON>(i), m_currentMouseState.buttons[i], m_currentMouseState.pos );
			}
		}

		//reset the buffer to begin again each frame
		m_mouseStateBuffer.delta.Set( 0.0, 0.0 );
		m_mouseStateBuffer.wheelDelta = 0;

	}

	void InputManager::InitializeRawInput()
	{

		//initialize the window to use raw input
		RAWINPUTDEVICE rids[2];

		//register the mouse
		rids[0].usUsage = 0x02;    // mouse
		rids[0].usUsagePage = 0x01;    // top-level
		rids[0].dwFlags = 0;    // do not generate legacy messages
		rids[0].hwndTarget = m_renderWindow->GetHandle();    // handle to a window
															 //register the keyboard
		rids[1].usUsage = 0x06;    // keyboard
		rids[1].usUsagePage = 0x01;    // top-level
		rids[1].dwFlags = 0;    // do not generate legacy messages such as WM_KEYDOWN
		rids[1].hwndTarget = m_renderWindow->GetHandle();    // handle to a window

		if( !RegisterRawInputDevices( rids, 2, sizeof( RAWINPUTDEVICE ) ) )   // register the device
		{
			throw Kiwi::Exception( L"InputManager::InputManager", L"Call to RegisterRawInputDevices failed", KIWI_EXCEPTION::OBJECTCREATIONFAILED );
		}

	}

	void InputManager::Update()
	{

		this->_UpdateMouseState();

		this->_UpdateKeyStates();

	}

	void InputManager::ProcessInput( const RAWINPUT& input )
	{

		if( input.header.dwType == RIM_TYPEMOUSE )
		{
			// read the mouse data
			m_mouseStateBuffer.delta.x += (float)input.data.mouse.lLastX;
			m_mouseStateBuffer.delta.y += (float)input.data.mouse.lLastY;

			if( input.data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_DOWN )
			{
				m_mouseStateBuffer.buttons[MOUSE_LEFT] = MOUSE_BUTTON_STATE::BUTTON_PRESSED;

			} else if( input.data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_UP )
			{
				m_mouseStateBuffer.buttons[MOUSE_LEFT] = MOUSE_BUTTON_STATE::BUTTON_RELEASED;
			}

			if( input.data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_DOWN )
			{
				m_mouseStateBuffer.buttons[MOUSE_RIGHT] = MOUSE_BUTTON_STATE::BUTTON_PRESSED;

			} else if( input.data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_UP )
			{
				m_mouseStateBuffer.buttons[MOUSE_RIGHT] = MOUSE_BUTTON_STATE::BUTTON_RELEASED;
			}

			if( input.data.mouse.ulButtons & RI_MOUSE_MIDDLE_BUTTON_DOWN )
			{
				m_mouseStateBuffer.buttons[MOUSE_MIDDLE] = MOUSE_BUTTON_STATE::BUTTON_PRESSED;

			} else if( input.data.mouse.ulButtons & RI_MOUSE_MIDDLE_BUTTON_UP )
			{
				m_mouseStateBuffer.buttons[MOUSE_MIDDLE] = MOUSE_BUTTON_STATE::BUTTON_RELEASED;
			}

			if( input.data.mouse.ulButtons & RI_MOUSE_BUTTON_4_DOWN )
			{
				m_mouseStateBuffer.buttons[MOUSE_4] = MOUSE_BUTTON_STATE::BUTTON_PRESSED;

			} else if( input.data.mouse.ulButtons & RI_MOUSE_BUTTON_4_UP )
			{
				m_mouseStateBuffer.buttons[MOUSE_4] = MOUSE_BUTTON_STATE::BUTTON_RELEASED;
			}

			if( input.data.mouse.ulButtons & RI_MOUSE_BUTTON_5_DOWN )
			{
				m_mouseStateBuffer.buttons[MOUSE_5] = MOUSE_BUTTON_STATE::BUTTON_PRESSED;

			} else if( input.data.mouse.ulButtons & RI_MOUSE_BUTTON_5_UP )
			{
				m_mouseStateBuffer.buttons[MOUSE_5] = MOUSE_BUTTON_STATE::BUTTON_RELEASED;
			}

			if( input.data.mouse.ulButtons & RI_MOUSE_WHEEL )
			{
				m_mouseStateBuffer.wheelDelta += input.data.mouse.usButtonData;
			}

		} else if( input.header.dwType == RIM_TYPEKEYBOARD )
		{
			/*from tutorials
			http://codereal.blogspot.com/2009/11/raw-input-introduction-part-1-keyboard.html
			https://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/
			*/

			//process keyboard input
			UINT vKey = input.data.keyboard.VKey;
			UINT scanCode = input.data.keyboard.MakeCode;
			UINT flags = input.data.keyboard.Flags;

			// e0 and e1 are escape sequences used for certain special keys, such as PRINT and PAUSE/BREAK.
			// see http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
			const bool isE0 = ((flags & RI_KEY_E0) != 0);
			const bool isE1 = ((flags & RI_KEY_E1) != 0);

			KeyStateChange stateChange;

			/*correct for different special keys on the keyboard*/
			if( vKey == 255 )
			{
				// discard "fake keys" which are part of an escaped sequence
				return;

			} else if( vKey == VK_SHIFT )
			{
				// correct for left-hand / right-hand SHIFT
				vKey = MapVirtualKey( scanCode, MAPVK_VSC_TO_VK_EX );
				stateChange.key = (Kiwi::KEY)vKey;

			} else if( vKey == VK_NUMLOCK )
			{
				// correct PAUSE/BREAK and NUM LOCK silliness, and set the extended bit
				scanCode = (MapVirtualKey( vKey, MAPVK_VK_TO_VSC ) | 0x100);
				stateChange.key = Kiwi::KEY::Numlock;

			} else
			{
				if( isE1 )
				{
					// for escaped sequences, turn the virtual key into the correct scan code using MapVirtualKey.
					// however, MapVirtualKey is unable to map VK_PAUSE (this is a known bug), hence we map that by hand.
					if( vKey == VK_PAUSE )
						scanCode = 0x45;
					else
						scanCode = MapVirtualKey( vKey, MAPVK_VK_TO_VSC );
				}

				switch( vKey )
				{
					// right-hand CONTROL and ALT have their e0 bit set
					case VK_CONTROL:
						{
							if( isE0 )
								stateChange.key = Kiwi::KEY::RControl;
							else
								stateChange.key = Kiwi::KEY::LControl;
							break;
						}
					case VK_MENU:
						if( isE0 )
							stateChange.key = Kiwi::KEY::RAlt;
						else
							stateChange.key = Kiwi::KEY::LAlt;
						break;

						// NUMPAD ENTER has its e0 bit set
					case VK_RETURN:
						if( isE0 )
							stateChange.key = Kiwi::KEY::NumEnter;
						else
							stateChange.key = Kiwi::KEY::Enter;
						break;

						// the standard INSERT, DELETE, HOME, END, PRIOR and NEXT keys will always have their e0 bit set, but the
						// corresponding keys on the NUMPAD will not.
					case VK_INSERT:
						if( !isE0 )
							stateChange.key = Kiwi::KEY::Numpad0;
						else
							stateChange.key = Kiwi::KEY::Insert;
						break;

					case VK_DELETE:
						if( !isE0 )
							stateChange.key = Kiwi::KEY::Decimal;
						else
							stateChange.key = Kiwi::KEY::Delete;
						break;

					case VK_HOME:
						if( !isE0 )
							stateChange.key = Kiwi::KEY::Numpad7;
						else
							stateChange.key = Kiwi::KEY::Home;
						break;

					case VK_END:
						if( !isE0 )
							stateChange.key = Kiwi::KEY::Numpad1;
						else
							stateChange.key = Kiwi::KEY::End;
						break;

					case VK_PRIOR:
						if( !isE0 )
							stateChange.key = Kiwi::KEY::Numpad9;
						else
							stateChange.key = Kiwi::KEY::PgUp;
						break;

					case VK_NEXT:
						if( !isE0 )
							stateChange.key = Kiwi::KEY::Numpad3;
						else
							stateChange.key = Kiwi::KEY::PgDown;
						break;

						// the standard arrow keys will always have their e0 bit set, but the
						// corresponding keys on the NUMPAD will not.
					case VK_LEFT:
						if( !isE0 )
							stateChange.key = Kiwi::KEY::Numpad4;
						else
							stateChange.key = Kiwi::KEY::LeftArrow;
						break;

					case VK_RIGHT:
						if( !isE0 )
							stateChange.key = Kiwi::KEY::Numpad6;
						else
							stateChange.key = Kiwi::KEY::RightArrow;
						break;

					case VK_UP:
						if( !isE0 )
							stateChange.key = Kiwi::KEY::Numpad8;
						else
							stateChange.key = Kiwi::KEY::UpArrow;
						break;

					case VK_DOWN:
						if( !isE0 )
							stateChange.key = Kiwi::KEY::Numpad2;
						else
							stateChange.key = Kiwi::KEY::DownArrow;
						break;

						// NUMPAD 5 doesn't have its e0 bit set
					case VK_CLEAR:
						if( !isE0 )
							stateChange.key = Kiwi::KEY::Numpad5;
						else
							stateChange.key = Kiwi::KEY::Clear;
						break;
					default:
						{
							stateChange.key = (Kiwi::KEY)vKey;
							break;
						}
				}
			}

			stateChange.state = ((flags & RI_KEY_BREAK) != 0); //true if key is up

			m_keyStateChanges.push_back( stateChange );

			//method to get a human-readable string for testing
			/*UINT key = (scanCode << 16) | (isE0 << 24);
			wchar_t buffer[512] = {};
			GetKeyNameText( (LONG)key, buffer, 512 );*/
		}

	}

	bool InputManager::GetKeyState( KEY key, KEY_STATE keyState )
	{

		KEY_STATE currentState = m_keyStates[key].current;

		if( currentState == keyState ||
			( keyState == KEY_ACTIVE && (currentState == KEY_PRESSED || currentState == KEY_HELD) ) ||
			( keyState == KEY_INACTIVE && (currentState == KEY_RELEASED || currentState == KEY_UP) ) )
		{
			return true;
		}

		return false;

	}

}