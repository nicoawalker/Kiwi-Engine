#include "Vector4.h"

#include <sstream>

namespace Kiwi
{

	Vector4::Vector4()
	{

		this->Set(0.0f, 0.0f, 0.0f, 0.0f);

	}

	Vector4::Vector4(float x, float y, float z, float w)
	{

		this->Set(x, y, z, w);

	}

	Vector4::Vector4(const Vector4& newVec)
	{

		this->Set(newVec);

	}

	void Vector4::Set(float x, float y, float z, float w)
	{

		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;

	}

	void Vector4::Set(const Vector4& newVector)
	{

		this->x = newVector.x;
		this->y = newVector.y;
		this->z = newVector.z;
		this->w = newVector.w;

	}

	float Vector4::Magnitude()const
	{

		return std::sqrt( x*x + y*y + z*z + w*w );

	}

	float Vector4::SquareMagnitude()const
	{

		return ( x*x + y*y + z*z + w*w );

	}

	std::wstring Vector4::ToString()const
	{

		std::wstringstream str;
		str << "x:" << x << ", y:" << y << ", z:" << z;
		return str.str();

	}

	Vector4 Vector4::operator+ (const Vector4& v)
	{

		return Vector4( x+v.x, y+v.y, z+v.z, w+v.w );

	}

	void Vector4::operator= (const Vector4& v)
	{

		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;

	}

	void Vector4::operator+= (const Vector4& v)
	{

		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
	}

};