#ifndef _KIWI_COLOR_H_
#define _KIWI_COLOR_H_

#include "../Core/Vector4.h"

#include <string>

namespace Kiwi
{

	class Color
	{
	public:

		double red, green, blue, alpha;

	private:

		//clamps the color values between 0.0f and 1.0f
		void _Clamp();

	public:

		Color();
		/*creates and initializes the color. color channels are clamped between 0.0f and 1.0f*/
		Color(double r, double g, double b, double a);
		Color(const Kiwi::Color& color);

		~Color() {}

		/*sets the color. color channels are clamped between 0.0f and 1.0f*/
		void Set(double r, double g, double b, double a);
		void Set(const Kiwi::Color& color);
		//sets this color to the values in a vector ( x:r, y:g, z:b, w:a)
		void Set(const Kiwi::Vector4& vector);

		std::wstring ToString()const;

		//returns the result of adding two colors
		Kiwi::Color operator+ (const Kiwi::Color& color);
		//sets this color
		void operator= (const Kiwi::Color& color);
		//sets this color to the values in a vector ( x:r, y:g, z:b, w:a)
		void operator= (const Kiwi::Vector4& vector);
		//adds to this color
		void operator+= (const Kiwi::Color& color);

		bool operator== ( const Kiwi::Color& color )const;
		bool operator!= ( const Kiwi::Color& other )const;

	};
};

#endif