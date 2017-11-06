#ifndef _KIWI_VECTOR2_H_
#define _KIWI_VECTOR2_H_

#include <string>

namespace Kiwi
{

	class Vector2d;

	class Vector2
	{
	public:

		float x, y;

	public:

		Vector2();
		Vector2(float x, float y);
		Vector2(const Vector2& newVec);

		~Vector2(){}

		void Set(float x, float y);
		void Set(const Vector2& newVector);

		/*returns the result of adding to the x,y values*/
		Vector2 Add(float x, float y)const;
		/*returns the result of adding to the x,y values*/
		Vector2 Add(const Vector2& v2)const;

		//returns the magnitude of this vector
		float Magnitude()const;

		//returns the square magnitude of this vector, to avoid a square root
		float SquareMagnitude()const;

		/*returns the result of normalizing this vector*/
		Vector2 Normalized()const;

		/*returns the result of multiplying this vector by a scalar*/
		Vector2 Scaled(float scalar)const;

		/*returns the dot product of the two vectors: this * vector*/
		float Dot( const Vector2& vector )const;

		//returns a string of the form 'x, y'
		std::wstring ToString()const;

		void operator= ( const Vector2& vec );
		void operator= ( const Vector2d& vec );

		/*returns true if the two vectors are equal*/
		bool operator== ( const Vector2& vec )const;

		//adds the two vectors and returns the result
		Vector2 operator+ ( const Vector2& vec )const;

		//subtracts the other vector from this one and returns the result
		Vector2 operator- ( const Vector2& vec )const;

		//divides each term of this vector by the amount and returns the result
		Vector2 operator/ ( float amount )const;
		
		//returns the result of multiplying this vector by a scalar
		Vector2 operator* ( float scalar )const;

	};

};

#endif