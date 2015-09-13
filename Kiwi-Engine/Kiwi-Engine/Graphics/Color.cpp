#include "Color.h"

#include "../Core/Math.h"

#include <sstream>

namespace Kiwi
{

	Color::Color()
	{

		red = green = blue = alpha = 0.0f;

	}

	Color::Color(float a, float g, float b, float r)
	{

		this->alpha = a;
		this->green = g;
		this->blue = b;
		this->red = r;

		this->_Clamp();

	}

	Color::Color(const Color& color)
	{

		this->alpha = color.alpha;
		this->green = color.green;
		this->blue = color.blue;
		this->red = color.red;

		this->_Clamp();

	}

	void Color::Set(float r, float g, float b, float a)
	{

		this->alpha = a;
		this->green = g;
		this->blue = b;
		this->red = r;

		this->_Clamp();

	}

	void Color::Set(const Color& color)
	{

		this->alpha = color.alpha;
		this->green = color.green;
		this->blue = color.blue;
		this->red = color.red;

		this->_Clamp();

	}

	void Color::Set(const Kiwi::Vector4& vector)
	{

		red = vector.x;
		green = vector.y;
		blue = vector.z;
		alpha = vector.w;

	}

	std::wstring Color::ToString()const
	{

		std::wstringstream str;
		str << "r:" << red << ", g:" << green << ", b:" << blue << ", a: "<< alpha;
		return str.str();

	}

	Color Color::operator+ (const Color& v)
	{

		float r = red + v.red;
		float g = green + v.green;
		float b = blue + v.blue;
		float a = alpha + v.alpha;

		Kiwi::clamp(a, 0.0f, 1.0f);
		Kiwi::clamp(r, 0.0f, 1.0f);
		Kiwi::clamp(g, 0.0f, 1.0f);
		Kiwi::clamp(b, 0.0f, 1.0f);

		return Color( r, g, b, a );

	}

	void Color::operator= (const Color& v)
	{

		alpha = v.alpha;
		green = v.green;
		blue = v.blue;
		red = v.red;
		
		this->_Clamp();

	}

	void Color::operator= (const Kiwi::Vector4& vector)
	{

		red = vector.x;
		green = vector.y;
		blue = vector.z;
		alpha = vector.w;

	}

	void Color::operator+= (const Color& v)
	{

		alpha += v.alpha;
		green += v.green;
		blue += v.blue;
		red += v.red;

		this->_Clamp();

	}

	void Color::_Clamp()
	{

		Kiwi::clamp(alpha, 0.0f, 1.0f);
		Kiwi::clamp(red, 0.0f, 1.0f);
		Kiwi::clamp(green, 0.0f, 1.0f);
		Kiwi::clamp(blue, 0.0f, 1.0f);

	}

};