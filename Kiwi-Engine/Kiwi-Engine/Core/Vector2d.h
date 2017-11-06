#ifndef _KIWI_Vector2dD_H_
#define _KIWI_Vector2dD_H_

#include "Vector2.h"

#include <string>

namespace Kiwi
{

	class Vector2d
	{
	public:

		double x, y;

	public:

		Vector2d();
		Vector2d( double x, double y );
		Vector2d( const Vector2d& newVec );
		Vector2d( const Vector2& source );

		~Vector2d() {}

		void Set( double x, double y );
		void Set( const Vector2d& newVector );

		/*returns the result of adding to the x,y values*/
		Vector2d Add( double x, double y )const;
		/*returns the result of adding to the x,y values*/
		Vector2d Add( const Vector2d& v2 )const;

		//returns the magnitude of this vector
		double Magnitude()const;

		//returns the square magnitude of this vector, to avoid a square root
		double SquareMagnitude()const;

		/*returns the result of normalizing this vector*/
		Vector2d Normalized()const;

		/*returns the result of multiplying this vector by a scalar*/
		Vector2d Scaled( double scalar )const;

		/*returns the dot product of the two vectors: this * vector*/
		double Dot( const Vector2d& vector )const;

		//returns a string of the form 'x, y'
		std::wstring ToString()const;

		void operator= ( const Vector2d& vec );
		void operator= ( const Vector2& vec );

		/*returns true if the two vectors are equal*/
		bool operator== ( const Vector2d& vec )const;

		bool operator!= ( const Vector2d& vec )const;

		//adds the two vectors and returns the result
		Vector2d operator+ ( const Vector2d& vec )const;

		//adds the two vectors and returns the result
		Vector2d operator+ ( const Vector2& vec )const;

		//subtracts the other vector from this one and returns the result
		Vector2d operator- ( const Vector2d& vec )const;

		//divides each term of this vector by the amount and returns the result
		Vector2d operator/ ( double amount )const;

		//returns the result of multiplying this vector by a scalar
		Vector2d operator* ( double scalar )const;

	};

};

#endif