#include "Vector4d.h"

#include <sstream>

namespace Kiwi
{

	Vector4d::Vector4d()
	{
		this->Set( 0.0, 0.0, 0.0, 0.0 );
	}

	Vector4d::Vector4d( double x, double y, double z, double w )
	{
		this->Set( x, y, z, w );
	}

	Vector4d::Vector4d( const Vector4d& newVec )
	{
		this->Set( newVec );
	}

	void Vector4d::Set( double x, double y, double z, double w )
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	void Vector4d::Set( const Vector4d& newVector )
	{
		this->x = newVector.x;
		this->y = newVector.y;
		this->z = newVector.z;
		this->w = newVector.w;
	}

	double Vector4d::Magnitude()const
	{
		return std::sqrt( x*x + y*y + z*z + w*w );
	}

	double Vector4d::SquareMagnitude()const
	{
		return (x*x + y*y + z*z + w*w);
	}

	std::wstring Vector4d::ToString()const
	{
		std::wstringstream str;
		str << "x:" << x << ", y:" << y << ", z:" << z;
		return str.str();
	}

	Vector4d Vector4d::operator+ ( const Vector4d& v )const
	{
		return Vector4d( x + v.x, y + v.y, z + v.z, w + v.w );
	}

	void Vector4d::operator= ( const Vector4d& v )
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
	}

	void Vector4d::operator+= ( const Vector4d& v )
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
	}

	bool Vector4d::operator!= ( const Vector4d& other )const
	{
		return x != other.x || y != other.y || z != other.z || w != other.w;
	}

};