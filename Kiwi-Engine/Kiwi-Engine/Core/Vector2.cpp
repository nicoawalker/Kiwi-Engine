#include "Vector2.h"
#include "Vector2d.h"

#include <cmath>
#include <string>
#include <sstream>

namespace Kiwi
{

	Vector2::Vector2()
	{
		
		x = y = 0.0f;

	}

	Vector2::Vector2(float x, float y)
	{
		
		this->x = x;
		this->y = y;

	}

	Vector2::Vector2(const Vector2& newVec)
	{

		x = newVec.x;
		y = newVec.y;

	}

	void Vector2::Set(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	void Vector2::Set(const Vector2& newVector)
	{

		this->x = newVector.x;
		this->y = newVector.y;

	}

	Vector2 Vector2::Add(float x, float y)const
	{

		return Vector2(this->x + x, this->y + y);

	}

	Vector2 Vector2::Add(const Vector2& v2)const
	{

		return Vector2(this->x + v2.x, this->y + v2.y);

	}

	float Vector2::Magnitude()const
	{

		return std::sqrt( x*x + y*y );

	}

	float Vector2::SquareMagnitude()const
	{

		return ( x*x + y*y );

	}

	Vector2 Vector2::Normalized()const
	{

		float mag = this->Magnitude();

		if(mag != 0.0f)
		{
			return Vector2( x / mag, y / mag );
		}else
		{
			return Vector2( 0.0f, 0.0f );
		}

	}

	Vector2 Vector2::Scaled(float scalar)const
	{

		return Vector2(this->x * scalar, this->y * scalar);
	}

	float Vector2::Dot(const Vector2& vector)const
	{

		return ( x*vector.x + y*vector.y );

	}

	std::wstring Vector2::ToString()const
	{

		std::wstringstream str;
		str << "x:" << x << ", y:" << y;
		return str.str();

	}

	void Vector2::operator= (const Vector2& vec)
	{
		this->x = vec.x;
		this->y = vec.y;
	}

	void Vector2::operator= ( const Vector2d& vec )
	{
		this->x = (float)vec.x;
		this->y = (float)vec.y;
	}

	bool Vector2::operator== (const Vector2& vec)const
	{
		return ( x == vec.x && y == vec.y );
	}

	Vector2 Vector2::operator+ (const Vector2& vec)const
	{

		return Vector2( this->x + vec.x, this->y + vec.y );

	}

	Vector2 Vector2::operator- (const Vector2& vec)const
	{

		return Vector2( this->x - vec.x, this->y - vec.y );

	}

	Vector2 Vector2::operator/ (float amount)const
	{

		if(amount == 0) return *this;

		return Vector2( this->x / amount, this->y / amount );

	}

	Vector2 Vector2::operator* ( float scalar )const
	{

		return Vector2( this->x * scalar, this->y * scalar );

	}

};