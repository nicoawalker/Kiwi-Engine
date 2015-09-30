#ifndef _KIWI_VECTOR3L_H_
#define _KIWI_VECTOR3L_H_

#include <string>

namespace Kiwi
{

	class Vector3L
	{
	public:

		long long x, y, z;

	public:

		Vector3L();
		Vector3L( long long x, long long y, long long z );
		Vector3L( const Kiwi::Vector3L& vec );
		~Vector3L() {}

		void Set( long long x, long long y, long long z );

		//returns the magnitude of this vector
		long long Magnitude()const;

		//returns the square magnitude of this vector, to avoid a square root
		long long SquareMagnitude()const;

		/*returns the result of normalizing this vector*/
		Kiwi::Vector3L Normalized()const;

		/*returns the result of multiplying this vector by a scalar*/
		Kiwi::Vector3L Scaled( long long scalar )const;

		/*returns the dot product of the two vectors: this * vector*/
		long long Dot( const Kiwi::Vector3L& vector )const;

		/*returns the cross product of the two vectors: this x vector*/
		Kiwi::Vector3L Cross( const Kiwi::Vector3L& vector )const;

		//returns a string of the form 'x, y, z'
		std::wstring ToString()const;

		void operator= ( const Kiwi::Vector3L& vec );
		void operator+= ( const Kiwi::Vector3L& vec );

		/*returns true if the two vectors are equal*/
		bool operator== ( const Kiwi::Vector3L& vec )const;

		//adds the two vectors and returns the result
		Kiwi::Vector3L operator+ ( const Kiwi::Vector3L& vec )const;

		//subtracts the other vector from this one and returns the result
		Kiwi::Vector3L operator- ( const Kiwi::Vector3L& vec )const;

		//divides each term of this vector by the amount and returns the result
		Kiwi::Vector3L operator/ ( long long amount )const;

		/*returns the result of multiplying this vector by a scalar*/
		Kiwi::Vector3L operator* ( long long scalar )const;

		static Kiwi::Vector3L forward() { return Kiwi::Vector3L( 0, 0, 1 ); }
		static Kiwi::Vector3L up() { return Kiwi::Vector3L( 0, 1, 0 ); }
		static Kiwi::Vector3L right() { return Kiwi::Vector3L( 1, 0, 0 ); }

	};

}

#endif