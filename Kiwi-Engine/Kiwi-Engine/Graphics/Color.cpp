#include "Color.h"

#include "../Core/Math.h"

#include <sstream>

namespace Kiwi
{

	Color::Color()
	{
		red = green = blue = alpha = 0.0f;
	}

	Color::Color(double r, double g, double b, double a)
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

	void Color::Set(double r, double g, double b, double a)
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
		red = (double)vector.x;
		green = (double)vector.y;
		blue = (double)vector.z;
		alpha = (double)vector.w;
	}

	std::wstring Color::ToString()const
	{
		std::wstringstream str;
		str << "r:" << red << ", g:" << green << ", b:" << blue << ", a: "<< alpha;
		return str.str();
	}

	Color Color::operator+ (const Color& v)
	{
		double r = red + v.red;
		double g = green + v.green;
		double b = blue + v.blue;
		double a = alpha + v.alpha;

		Kiwi::clamp(a, 0.0, 1.0);
		Kiwi::clamp(r, 0.0, 1.0);
		Kiwi::clamp(g, 0.0, 1.0);
		Kiwi::clamp(b, 0.0, 1.0);

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
		Kiwi::clamp(alpha, 0.0, 1.0);
		Kiwi::clamp(red, 0.0, 1.0);
		Kiwi::clamp(green, 0.0, 1.0);
		Kiwi::clamp(blue, 0.0, 1.0);
	}

	bool Color::operator== ( const Kiwi::Color& color )const
	{
		return red == color.red && green == color.green && blue == color.blue && alpha == color.alpha;
	}

	bool Color::operator!= ( const Kiwi::Color& other )const
	{
		return red != other.red || green != other.green || blue != other.blue || alpha != other.alpha;
	}

};