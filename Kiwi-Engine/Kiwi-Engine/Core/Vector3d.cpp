#include "Vector3d.h"

#include <sstream>

namespace Kiwi
{

	Vector3d::Vector3d()
	{
		x = y = z = 0.0;
	}

	Vector3d::Vector3d( double x, double y, double z )
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3d::Vector3d( const Kiwi::Vector3d& newVec )
	{
		x = newVec.x;
		y = newVec.y;
		z = newVec.z;
	}

	Vector3d::Vector3d( const Kiwi::Vector3L& newVec )
	{
		x = (double)newVec.x;
		y = (double)newVec.y;
		z = (double)newVec.z;
	}

	Vector3d::Vector3d( const Kiwi::Vector3& newVec )
	{
		x = (double)newVec.x;
		y = (double)newVec.y;
		z = (double)newVec.z;
	}

	void Vector3d::Set( double x, double y, double z )
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	double Vector3d::Magnitude()const
	{
		return (double)std::sqrtl( (long double)(x*x + y*y + z*z) );
	}

	double Vector3d::SquareMagnitude()const
	{
		return (x*x + y*y + z*z);
	}

	Kiwi::Vector3d Vector3d::Normalized()const
	{
		double mag = this->Magnitude();

		if( mag != 0 )
		{
			return Kiwi::Vector3d( x / mag, y / mag, z / mag );

		} else
		{
			return Kiwi::Vector3d( 0, 0, 0 );
		}
	}

	Kiwi::Vector3d Vector3d::Scaled( double scalar )const
	{
		return Kiwi::Vector3d( this->x * scalar, this->y * scalar, this->z * scalar );
	}

	double Vector3d::Dot( const Kiwi::Vector3d& vector )const
	{
		return (x*vector.x + y*vector.y + z*vector.z);
	}

	Kiwi::Vector3d Vector3d::Cross( const Kiwi::Vector3d& vector )const
	{
		return Kiwi::Vector3d( y*vector.z - z*vector.y,
							   z*vector.x - x*vector.z,
							   x*vector.y - y*vector.x );
	}

	void Vector3d::ClampMagnitude( double magnitude )
	{
		double sqMag = this->SquareMagnitude();
		if( sqMag != 0 && sqMag  > magnitude * magnitude )
		{
			double mag = magnitude / this->Magnitude();
			x *= mag;
			y *= mag;
			z *= mag;
		}
	}

	std::wstring Vector3d::ToString()const
	{
		std::wstringstream str;
		str << "x:" << x << ", y:" << y << ", z:" << z;
		return str.str();
	}

	void Vector3d::operator= ( const Kiwi::Vector3d& vec )
	{
		this->x = vec.x;
		this->y = vec.y;
		this->z = vec.z;
	}

	void Vector3d::operator= ( const Kiwi::Vector3& vec )
	{
		this->x = (double)vec.x;
		this->y = (double)vec.y;
		this->z = (double)vec.z;
	}

	void Vector3d::operator+= ( const Kiwi::Vector3d& vec )
	{
		this->x += vec.x;
		this->y += vec.y;
		this->z += vec.z;
	}

	void Vector3d::operator-= ( const Kiwi::Vector3d& vec )
	{
		this->x -= vec.x;
		this->y -= vec.y;
		this->z -= vec.z;
	}

	bool Vector3d::operator== ( const Kiwi::Vector3d& vec )const
	{
		return (x == vec.x && y == vec.y && z == vec.z);
	}

	Vector3d Vector3d::operator+ ( const Kiwi::Vector3d& vec )const
	{
		return Kiwi::Vector3d( this->x + vec.x, this->y + vec.y, this->z + vec.z );
	}

	Vector3d Vector3d::operator+ ( const Kiwi::Vector3& vec )const
	{
		return Kiwi::Vector3d( this->x + (double)vec.x, this->y + (double)vec.y, this->z + (double)vec.z );
	}

	Vector3d Vector3d::operator- ( const Kiwi::Vector3d& vec )const
	{
		return Kiwi::Vector3d( this->x - vec.x, this->y - vec.y, this->z - vec.z );
	}

	Vector3d Vector3d::operator- ( const Kiwi::Vector3& vec )const
	{
		return Kiwi::Vector3d( this->x - (double)vec.x, this->y - (double)vec.y, this->z - (double)vec.z );
	}

	Vector3d Vector3d::operator/ ( double amount )const
	{
		if( amount == 0 ) return *this;

		return Kiwi::Vector3d( this->x / amount, this->y / amount, this->z / amount );
	}

	Vector3d Vector3d::operator* ( double scalar )const
	{
		return Kiwi::Vector3d( this->x * scalar, this->y * scalar, this->z * scalar );
	}

};