#ifndef _KIWI_TYPES_H_
#define _KIWI_TYPES_H_

#ifndef NDEBUG
#define KIWIDEBUGENABLED
#endif

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef KIWIDEBUGENABLED
#ifdef _WIN32
	#define KIWIDEBUGSTRING( str ) OutputDebugStringW( (str) )
#endif
#else
#define KIWIDEBUGSTRING( str )
#endif

#include "Graphics\DirectX.h"

#include <string>
#include <unordered_map>
#include <memory>

namespace Kiwi
{

	typedef unsigned int uint;

	typedef std::wstring String;
	typedef std::unordered_map<String, String> StringPair;

	struct MakeType
	{
		static Kiwi::StringPair StringPair( const StringPair& pair ) { return pair; }
	};

	enum PrimitiveTopology
	{
		POINT_LIST = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		LINE_LIST = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		LINE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		TRIANGLE_LIST = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		TRIANGLE_STRIP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	};

	enum TEXT_ALIGNMENT { ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTRE };

	enum MOUSE_BUTTON { MOUSE_LEFT = 0, MOUSE_RIGHT = 1, MOUSE_MIDDLE = 2, MOUSE_4 = 3, MOUSE_5 = 4, NO_BUTTON = 1000 };
	enum MOUSE_BUTTON_STATE { BUTTON_UP = 0, BUTTON_PRESSED, BUTTON_RELEASED, BUTTON_HELD, BUTTON_NONE = 1000 };

	enum KEY_STATE { KEY_PRESSED = 2, KEY_HELD = 4, KEY_ACTIVE = 8, KEY_RELEASED = 16, KEY_UP = 32, KEY_INACTIVE = 64, KEY_NONE = 128 };

	enum class ComponentType { Transform = 0x1, Mesh = 0x2, Rigidbody = 0x4, Collider = 0x8, SpriteAnimator = 0x10 };

	inline KEY_STATE operator|(KEY_STATE a, KEY_STATE b)
	{
		return static_cast<KEY_STATE>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline KEY_STATE operator&( KEY_STATE a, KEY_STATE b )
	{
		return static_cast<KEY_STATE>(static_cast<int>(a) & static_cast<int>(b));
	}

	//key enums
	/*these key codes correspond to the microsoft VK_ keycodes,
	and map to the corresponding ASCII characters*/
	enum KEY
	{
		Backspace = 0x08,
		Tab = 0x09,
		Clear = 0x0C,
		Enter = 0x0D,
		Shift = 0x10,
		Ctrl = 0x11,
		Pause = 0x13,
		CapsLock = 0x14,
		IME_Kana = 0x15, //also Hangul
		IME_Junja = 0x17,
		IME_Final = 0x18,
		IME_Kanji = 0x19, //also Hanja
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
		LAlt = 0xA4,
		RAlt = 0xA5,
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

		NumEnter = 0xF0, //custom, numpad enter
	};

}

#endif