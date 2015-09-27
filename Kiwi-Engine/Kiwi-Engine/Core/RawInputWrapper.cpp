#include "RawInputWrapper.h"
#include "Exception.h"
#include "Assert.h"

#include "../Graphics/RenderWindow.h"

#include <Windows.h>

namespace Kiwi
{

	RawInputWrapper::RawInputWrapper( Kiwi::RenderWindow* targetWindow )
	{

		assert( targetWindow != 0 );

		m_wheelDelta = 0;
		m_mouseSpeed = Kiwi::Vector2( 1.0f, 1.0f );

		RAWINPUTDEVICE Mouse;
		Mouse.usUsage = 0x02;    // register mouse
		Mouse.usUsagePage = 0x01;    // top-level mouse
		Mouse.dwFlags = NULL;    // flags
		Mouse.hwndTarget = targetWindow->GetHandle();    // handle to a window

		if( !RegisterRawInputDevices( &Mouse, 1, sizeof( RAWINPUTDEVICE ) ) )   // register the device
		{
			throw Kiwi::Exception( L"RawInputWrapper", L"[Mouse] Call to RegisterRawInputDevices failed" );
		}

		/*intialize all key values to the default key_up state*/
		m_keyboardState.resize( 256 );
		std::fill( m_keyboardState.begin(), m_keyboardState.end(), KEY_UP );
		m_lastKeyboardState.resize( 256 );
		std::fill( m_lastKeyboardState.begin(), m_lastKeyboardState.end(), KEY_UP );

	}

	RawInputWrapper::~RawInputWrapper()
	{
	}

	void RawInputWrapper::OnUpdate( float deltaTime )
	{

		//first get and set the current position of the mouse
		POINT mousePos;
		GetCursorPos( &mousePos );
		m_currentMouseState.pos.Set( (float)mousePos.x, (float)mousePos.y );

		//store the delta mouse from last frame, and reset the deltaMouse counter
		m_currentMouseState.deltaPos.x = m_deltaMouse.x * m_mouseSpeed.x;
		m_currentMouseState.deltaPos.y = m_deltaMouse.y * m_mouseSpeed.y;
		//m_currentMouseState.deltaPos = m_deltaMouse * deltaTime;
		m_deltaMouse = Kiwi::Vector2( 0.0f, 0.0f );

		//same for the wheel delta
		m_currentMouseState.wheelDelta = m_wheelDelta * deltaTime;
		m_wheelDelta = 0;

		//update all of the mouse button states
		for( unsigned int i = 0; i < NUM_MOUSE_BUTTONS; i++ )
		{
			if( ( m_currentMouseState.mouseStates[i] != BUTTON_RELEASED && m_currentMouseState.mouseStates[i] != BUTTON_UP ) && m_lastMouseState.mouseStates[i] == BUTTON_PRESSED )
			{
				//if the button was pressed last frame and has not been released, set the status to held
				m_currentMouseState.mouseStates[i] = BUTTON_HELD;

			} else if( m_currentMouseState.mouseStates[i] == BUTTON_RELEASED && m_lastMouseState.mouseStates[i] == BUTTON_RELEASED )
			{
				//if the button was released last frame, and hasn't been pressed, set the status to up
				m_currentMouseState.mouseStates[i] = BUTTON_UP;
			}
		}

		m_lastMouseState = m_currentMouseState;

		//get the current keyboard key states as false or true values
		std::vector<unsigned char> newKeyboardState(256);
		GetKeyboardState( &newKeyboardState[0] );

		//copy the current key states into the last buffer
		m_lastKeyboardState.swap( m_keyboardState );

		for( unsigned int i = 0; i < m_lastKeyboardState.size(); i++ )
		{
			//will set m_keyboardState[i] to either KEY_UP or KEY_PRESSED (false or true)
			//if the high order bit is 1, the key is down, otherwise it is up
			m_keyboardState[i] = (KEY_STATE)(newKeyboardState[i] >> 7 );

			if( ( m_lastKeyboardState[i] == KEY_PRESSED || m_lastKeyboardState[i] == KEY_HELD ) && m_keyboardState[i] == KEY_PRESSED )
			{
				//if the key was pressed or held last frame, and is still being held change the state to KEY_HELD
				m_keyboardState[i] = KEY_HELD;

			} else if( m_lastKeyboardState[i] == KEY_HELD && m_keyboardState[i] == KEY_UP )
			{
				//if the key was held last frame and has now been released, change the state to KEY_RELEASED
				m_keyboardState[i] = KEY_RELEASED;
			} else if( m_lastKeyboardState[i] == KEY_RELEASED && m_keyboardState[i] == KEY_UP )
			{
				m_keyboardState[i] = KEY_UP;
			}
		}

	}

	void RawInputWrapper::ProcessInput( WPARAM wParam, LPARAM lParam )
	{

		RAWINPUT InputData;

		UINT DataSize = sizeof( RAWINPUT );
		GetRawInputData( (HRAWINPUT)lParam,
						 RID_INPUT,
						 &InputData,
						 &DataSize,
						 sizeof( RAWINPUTHEADER ) );

		if( InputData.header.dwType == RIM_TYPEMOUSE )
		{
			// read the mouse data
			m_deltaMouse.x += (float)InputData.data.mouse.lLastX;
			m_deltaMouse.y += (float)InputData.data.mouse.lLastY;
			
			if( InputData.data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_DOWN )
			{
				m_currentMouseState.mouseStates[MOUSE_LEFT] = BUTTON_PRESSED;

			} else if( InputData.data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_UP )
			{
				m_currentMouseState.mouseStates[MOUSE_LEFT] = BUTTON_RELEASED;
			}

			if( InputData.data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_DOWN )
			{
				m_currentMouseState.mouseStates[MOUSE_RIGHT] = BUTTON_PRESSED;

			} else if( InputData.data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_UP )
			{
				m_currentMouseState.mouseStates[MOUSE_RIGHT] = BUTTON_RELEASED;
			}

			if( InputData.data.mouse.ulButtons & RI_MOUSE_MIDDLE_BUTTON_DOWN )
			{
				m_currentMouseState.mouseStates[MOUSE_MIDDLE] = BUTTON_PRESSED;

			} else if( InputData.data.mouse.ulButtons & RI_MOUSE_MIDDLE_BUTTON_UP )
			{
				m_currentMouseState.mouseStates[MOUSE_MIDDLE] = BUTTON_RELEASED;
			}

			if( InputData.data.mouse.ulButtons & RI_MOUSE_BUTTON_4_DOWN )
			{
				m_currentMouseState.mouseStates[MOUSE_4] = BUTTON_PRESSED;

			} else if( InputData.data.mouse.ulButtons & RI_MOUSE_BUTTON_4_UP )
			{
				m_currentMouseState.mouseStates[MOUSE_4] = BUTTON_RELEASED;
			}

			if( InputData.data.mouse.ulButtons & RI_MOUSE_BUTTON_5_DOWN )
			{
				m_currentMouseState.mouseStates[MOUSE_5] = BUTTON_PRESSED;

			} else if( InputData.data.mouse.ulButtons & RI_MOUSE_BUTTON_5_UP )
			{
				m_currentMouseState.mouseStates[MOUSE_5] = BUTTON_RELEASED;
			}

			if( InputData.data.mouse.ulButtons & RI_MOUSE_WHEEL )
			{
				m_wheelDelta += InputData.data.mouse.usButtonData;
			}

		}

	}

}