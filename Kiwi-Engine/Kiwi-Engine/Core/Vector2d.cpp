#include "Vector2d.h"

#include <cmath>
#include <string>
#include <sstream>

namespace Kiwi
{

	Vector2d::Vector2d()
	{
		x = y = 0.0f;
	}

	Vector2d::Vector2d( double x, double y )
	{
		this->x = x;
		this->y = y;
	}

	Vector2d::Vector2d( const Vector2d& newVec )
	{
		x = newVec.x;
		y = newVec.y;
	}

	Vector2d::Vector2d( const Vector2& source )
	{
		x = (double)source.x;
		y = (double)source.y;
	}

	void Vector2d::Set( double x, double y )
	{
		this->x = x;
		this->y = y;
	}

	void Vector2d::Set( const Vector2d& newVector )
	{

		this->x = newVector.x;
		this->y = newVector.y;

	}

	Vector2d Vector2d::Add( double x, double y )const
	{

		return Vector2d( this->x + x, this->y + y );

	}

	Vector2d Vector2d::Add( const Vector2d& v2 )const
	{

		return Vector2d( this->x + v2.x, this->y + v2.y );

	}

	double Vector2d::Magnitude()const
	{

		return std::sqrt( x*x + y*y );

	}

	double Vector2d::SquareMagnitude()const
	{

		return (x*x + y*y);

	}

	Vector2d Vector2d::Normalized()const
	{

		double mag = this->Magnitude();

		if( mag != 0.0f )
		{
			return Vector2d( x / mag, y / mag );
		} else
		{
			return Vector2d( 0.0f, 0.0f );
		}

	}

	Vector2d Vector2d::Scaled( double scalar )const
	{

		return Vector2d( this->x * scalar, this->y * scalar );
	}

	double Vector2d::Dot( const Vector2d& vector )const
	{

		return (x*vector.x + y*vector.y);

	}

	std::wstring Vector2d::ToString()const
	{

		std::wstringstream str;
		str << "x:" << x << ", y:" << y;
		return str.str();

	}

	void Vector2d::operator= ( const Vector2d& vec )
	{
		this->x = vec.x;
		this->y = vec.y;
	}

	void Vector2d::operator= ( const Vector2& vec )
	{
		this->x = (double)vec.x;
		this->y = (double)vec.y;
	}

	bool Vector2d::operator== ( const Vector2d& vec )const
	{
		return (x == vec.x && y == vec.y);
	}

	bool Vector2d::operator!= ( const Vector2d& vec )const
	{
		return (x != vec.x || y != vec.y);
	}

	Vector2d Vector2d::operator+ ( const Vector2d& vec )const
	{

		return Vector2d( this->x + vec.x, this->y + vec.y );

	}

	Vector2d Vector2d::operator+ ( const Vector2& vec )const
	{

		return Vector2d( this->x + (double)vec.x, this->y + (double)vec.y );

	}

	Vector2d Vector2d::operator- ( const Vector2d& vec )const
	{

		return Vector2d( this->x - vec.x, this->y - vec.y );

	}

	Vector2d Vector2d::operator/ ( double amount )const
	{

		if( amount == 0 ) return *this;

		return Vector2d( this->x / amount, this->y / amount );

	}

	Vector2d Vector2d::operator* ( double scalar )const
	{

		return Vector2d( this->x * scalar, this->y * scalar );

	}

};