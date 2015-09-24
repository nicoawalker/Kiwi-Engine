#ifndef _KIWI_RAWINPUTWRAPPER_H_
#define _KIWI_RAWINPUTWRAPPER_H_

#define NUM_MOUSE_BUTTONS 5 //using left, right, middle, and buttons 1-5

#include "Vector2.h"

#include <Windows.h>
#include <vector>

namespace Kiwi
{

	//key enums
	/*these key codes correspond to the microsoft VK_ keycodes,
	and map to the corresponding ASCII characters*/
	enum KEY
	{
		Backspace = 0x08,
		Tab = 0x09,
		Enter = 0x0D,
		Shift = 0x10,
		Ctrl = 0x11,
		Alt = 0x12,
		CapsLock = 0x14,
		Escape = 0x1B,
		Space = 0x20,
		PgUp = 0x21,
		PgDown = 0x22,
		End = 0x23,
		Home = 0x24,
		LeftArrow = 0x25,
		UpArrow = 0x26,
		RightArrow = 0x27,
		DownArrow = 0x28,
		Select = 0x29,
		PrintScreen = 0x2C,
		Insert = 0x2D,
		Delete = 0x2E,
		Num0 = 0x30,
		Num1,
		Num2,
		Num3,
		Num4,
		Num5,
		Num6,
		Num7,
		Num8,
		Num9,
		A = 0x41,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z = 0x5A,
		LWin = 0x5B,
		RWin = 0x5C,
		Numpad0 = 0x60,
		Numpad1,
		Numpad2,
		Numpad3,
		Numpad4,
		Numpad5,
		Numpad6,
		Numpad7,
		Numpad8,
		Numpad9 = 0x69,
		Multiply = 0x6A,
		Add = 0x6B,
		Separator = 0x6C,
		Subtract = 0x6D,
		Decimal = 0x6E,
		Divide = 0x6F,
		F1 = 0x70,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24 = 0x87,
		Numlock = 0x90,
		ScrollLock = 0x91,
		LShift = 0xA0,
		RShift = 0xA1,
		LControl = 0xA2,
		RControl = 0xA3,
		LMenu = 0xA4,
		RMenu = 0xA5,
		OEM1 = 0xBA, // : or ;
		Plus = 0xBB,
		Comma = 0xBC,
		Minus = 0xBD,
		Period = 0xBE,
		OEM2 = 0xBF, // / or ?
		OEM3 = 0xC0, // ` or ~
		OEM4 = 0xDB, // [ or {
		OEM5 = 0xDC, // \ or |
		OEM6 = 0xDD, // ] or }
		OEM7 = 0xDE, // ' or "

	};

	class RenderWindow;

	enum MOUSE_BUTTON{ MOUSE_LEFT = 0, MOUSE_RIGHT = 1, MOUSE_MIDDLE = 2, MOUSE_4 = 3, MOUSE_5 = 4 };
	enum MOUSE_BUTTON_STATE{ BUTTON_UP = 0, BUTTON_PRESSED, BUTTON_RELEASED, BUTTON_HELD };
	enum KEY_STATE{ KEY_UP = 0, KEY_PRESSED, KEY_RELEASED, KEY_HELD };

	class RawInputWrapper
	{
	protected:

		struct MouseState
		{
			MOUSE_BUTTON_STATE mouseStates[NUM_MOUSE_BUTTONS]; //stores the states of mouse buttons 1-5 + wheel
			short wheelDelta; //amount the scroll wheel was moved
			Kiwi::Vector2 deltaPos; //change in position since last frame
			Kiwi::Vector2 pos; //current position of the mouse
		};

		MouseState m_currentMouseState;
		MouseState m_lastMouseState;

		std::vector<KEY_STATE> m_keyboardState;
		std::vector<KEY_STATE> m_lastKeyboardState;

		//stores the total amount the mouse has moved since the last call to Update()
		Kiwi::Vector2 m_deltaMouse;
		//stores the total amount of wheel motion since the last call to Update()
		short m_wheelDelta;

	public:

		RawInputWrapper( Kiwi::RenderWindow* window );
		~RawInputWrapper();

		void Update();

		void ProcessInput( WPARAM wParam, LPARAM lParam );

		/*returns the current state of a mouse button
		possible values are:
		BUTTON_HELD: the button is currently being held down (and was down or pressed last frame)
		BUTTON_RELEASED: the button was released this frame
		BUTTON_PRESSED: the button was pressed this frame (and wasn't held last frame)
		BUTTON_UP: the button is not being pressed, and no button event occured this frame*/
		MOUSE_BUTTON_STATE QueryMouseState( MOUSE_BUTTON button ) { return m_currentMouseState.mouseStates[button]; }

		Kiwi::Vector2& GetMousePosition() { return m_currentMouseState.pos; }
		Kiwi::Vector2& GetMouseDeltaPosition() { return m_currentMouseState.deltaPos; }
		short GetMouseWheelDelta()const { return m_currentMouseState.wheelDelta; }

		/*returns the current state of a keyboard key
		possible values are:
		KEY_HELD: the key is currently being held down (and was down or pressed last frame)
		KEY_RELEASED: the key was released this frame
		KEY_PRESSED: the key was pressed this frame (and wasn't held last frame)
		KEY_UP: the key is not being pressed, and no key event occured this frame*/
		KEY_STATE QueryKeyState( KEY key ) { return m_keyboardState[key]; }

	};
}

#endif