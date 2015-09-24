#ifndef _KIWI_UTILITIES_H_
#define _KIWI_UTILITIES_H_

#include <string>
#include <sstream>
#include <math.h>
#include <Windows.h>

#define SAFE_RELEASE(p) { if(p){ (p)->Release(); (p) = 0; } }
#define SAFE_DELETE(p) { if(p){ delete (p); (p) = 0; } }

namespace Kiwi
{

	std::wstring s2ws(const std::string& s1);
	std::wstring c2ws(const char cString[], int length);

	//takes the hresult error from a D3D call and converts it to a string representation
	std::wstring GetD3DErrorString(HRESULT hr);

	/*returns a string containing the hex representation of the hresult*/
	std::wstring HRToHexString( HRESULT hr );

	template<typename T>
	std::wstring ToWString(T input)
	{
		std::wstringstream wstr;
		wstr << input;
		return wstr.str();
	}

	template<typename T>
	std::string ToString(T input)
	{
		std::stringstream sStream;
		sStream<<input;
		return sStream.str();
	}

	template<typename T>
	std::string Concat(std::string input1, T input2)
	{
		std::stringstream sStream;
		sStream << input1; 
		sStream << input2;
		return sStream.str();
	}

	template<typename T>
	std::wstring Concat(std::wstring input1, T input2)
	{
		std::wstringstream wstr;
		wstr << input1;
		wstr << input2;
		return wstr.str();
	}

	/*frees all memory used by an std structure (vector, map, etc)*/
	template<typename T>
	void FreeMemory(T& t)
	{
		T tmp;
		t.swap(tmp);
	}

	//reinterprets any type that supports streaming as another
	template<typename ReturnType, typename InputType>
	ReturnType Stream_Cast( const InputType& val )
	{
		std::wstringstream interpreter;
		interpreter << val;
		ReturnType result = ReturnType();
		interpreter >> result;
		return result;
	}

};

#endif