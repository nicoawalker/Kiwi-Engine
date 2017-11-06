#include "Vector3.h"
#include "Vector3d.h"

#include <sstream>

namespace Kiwi
{

	Vector3::Vector3()
	{

		x = y = z = 0.0f;

	}

	Vector3::Vector3( float x, float y, float z )
	{

		this->x = x;
		this->y = y;
		this->z = z;

	}

	Vector3::Vector3( const Kiwi::Vector3& newVec )
	{

		x = newVec.x;
		y = newVec.y;
		z = newVec.z;

	}

	void Vector3::Set( float x, float y, float z )
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	float Vector3::Magnitude()const
	{

		return std::sqrt( x*x + y*y + z*z );

	}

	float Vector3::SquareMagnitude()const
	{

		return (x*x + y*y + z*z);

	}

	Kiwi::Vector3 Vector3::Normalized()const
	{

		float mag = this->Magnitude();

		if( mag != 0.0f )
		{
			return Kiwi::Vector3( x / mag, y / mag, z / mag );
		} else
		{
			return Kiwi::Vector3( 0.0f, 0.0f, 0.0f );
		}

	}

	Kiwi::Vector3 Vector3::Scaled( float scalar )const
	{

		return Kiwi::Vector3( this->x * scalar, this->y * scalar, this->z * scalar );
	}

	float Vector3::Dot( const Kiwi::Vector3& vector )const
	{

		return (x*vector.x + y*vector.y + z*vector.z);

	}

	Kiwi::Vector3 Vector3::Cross( const Kiwi::Vector3& vector )const
	{

		return Kiwi::Vector3( y*vector.z - z*vector.y,
							  z*vector.x - x*vector.z,
							  x*vector.y - y*vector.x );

	}

	std::wstring Vector3::ToString()const
	{

		std::wstringstream str;
		str << "x:" << x << ", y:" << y << ", z:" << z;
		return str.str();

	}

	void Vector3::operator= ( const Kiwi::Vector3& vec )
	{
		this->x = vec.x;
		this->y = vec.y;
		this->z = vec.z;
	}

	void Vector3::operator= ( const Kiwi::Vector3d& vec )
	{
		this->x = (float)vec.x;
		this->y = (float)vec.y;
		this->z = (float)vec.z;
	}

	void Vector3::operator+= ( const Kiwi::Vector3& vec )
	{

		this->x += vec.x;
		this->y += vec.y;
		this->z += vec.z;

	}

	void Vector3::operator+= ( const Kiwi::Vector3d& vec )
	{

		this->x += (float)vec.x;
		this->y += (float)vec.y;
		this->z += (float)vec.z;

	}

	bool Vector3::operator== ( const Kiwi::Vector3& vec )const
	{
		return (x == vec.x && y == vec.y && z == vec.z);
	}

	Kiwi::Vector3 Vector3::operator+ ( const Kiwi::Vector3& vec )const
	{

		return Kiwi::Vector3( this->x + vec.x, this->y + vec.y, this->z + vec.z );

	}

	Kiwi::Vector3 Vector3::operator+ ( const Kiwi::Vector3d& vec )const
	{

		return Kiwi::Vector3( this->x + (float)vec.x, this->y + (float)vec.y, this->z + (float)vec.z );

	}

	Kiwi::Vector3 Vector3::operator- ( const Kiwi::Vector3& vec )const
	{

		return Kiwi::Vector3( this->x - vec.x, this->y - vec.y, this->z - vec.z );

	}

	Kiwi::Vector3d Vector3::operator- ( const Kiwi::Vector3d& vec )const
	{

		return Kiwi::Vector3d( this->x - vec.x, this->y - vec.y, this->z - vec.z );

	}

	Kiwi::Vector3 Vector3::operator/ ( float amount )const
	{

		if( amount == 0 ) return *this;

		return Kiwi::Vector3( this->x / amount, this->y / amount, this->z / amount );

	}

	Kiwi::Vector3 Vector3::operator* ( float scalar )const
	{

		return Kiwi::Vector3( this->x * scalar, this->y * scalar, this->z * scalar );

	}

};