#include "Vector3L.h"

#include <sstream>

namespace Kiwi
{

	Vector3L::Vector3L()
	{

		x = y = z = 0;

	}

	Vector3L::Vector3L( long long x, long long y, long long z )
	{

		this->x = x;
		this->y = y;
		this->z = z;

	}

	Vector3L::Vector3L( const Kiwi::Vector3L& newVec )
	{

		x = newVec.x;
		y = newVec.y;
		z = newVec.z;

	}

	void Vector3L::Set( long long x, long long y, long long z )
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	long long Vector3L::Magnitude()const
	{

		return (long long)std::sqrtl( (long double)(x*x + y*y + z*z) );

	}

	long long Vector3L::SquareMagnitude()const
	{

		return (x*x + y*y + z*z);

	}

	Kiwi::Vector3L Vector3L::Normalized()const
	{

		long long mag = this->Magnitude();

		if( mag != 0 )
		{
			return Kiwi::Vector3L( x / mag, y / mag, z / mag );
		} else
		{
			return Kiwi::Vector3L( 0, 0, 0 );
		}

	}

	Kiwi::Vector3L Vector3L::Scaled( long long scalar )const
	{

		return Kiwi::Vector3L( this->x * scalar, this->y * scalar, this->z * scalar );
	}

	long long Vector3L::Dot( const Kiwi::Vector3L& vector )const
	{

		return (x*vector.x + y*vector.y + z*vector.z);

	}

	Kiwi::Vector3L Vector3L::Cross( const Kiwi::Vector3L& vector )const
	{

		return Kiwi::Vector3L( y*vector.z - z*vector.y,
							  z*vector.x - x*vector.z,
							  x*vector.y - y*vector.x );

	}

	std::wstring Vector3L::ToString()const
	{

		std::wstringstream str;
		str << "x:" << x << ", y:" << y << ", z:" << z;
		return str.str();

	}

	void Vector3L::operator= ( const Kiwi::Vector3L& vec )
	{
		this->x = vec.x;
		this->y = vec.y;
		this->z = vec.z;
	}

	void Vector3L::operator+= ( const Kiwi::Vector3L& vec )
	{

		this->x += vec.x;
		this->y += vec.y;
		this->z += vec.z;

	}

	bool Vector3L::operator== ( const Kiwi::Vector3L& vec )const
	{
		return (x == vec.x && y == vec.y && z == vec.z);
	}

	Vector3L Vector3L::operator+ ( const Kiwi::Vector3L& vec )const
	{

		return Kiwi::Vector3L( this->x + vec.x, this->y + vec.y, this->z + vec.z );

	}

	Vector3L Vector3L::operator- ( const Kiwi::Vector3L& vec )const
	{

		return Kiwi::Vector3L( this->x - vec.x, this->y - vec.y, this->z - vec.z );

	}

	Vector3L Vector3L::operator/ ( long long amount )const
	{

		if( amount == 0 ) return *this;

		return Kiwi::Vector3L( this->x / amount, this->y / amount, this->z / amount );

	}

	Vector3L Vector3L::operator* ( long long scalar )const
	{

		return Kiwi::Vector3L( this->x * scalar, this->y * scalar, this->z * scalar );

	}

};