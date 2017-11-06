#ifndef _KIWI_VECTOR3D_H_
#define _KIWI_VECTOR3D_H_

#include "Vector3.h"
#include "Vector3L.h"

#include <string>

namespace Kiwi
{

	class Vector3d
	{
	public:

		double x, y, z;

	public:

		Vector3d();
		Vector3d( double x, double y, double z );
		Vector3d( const Kiwi::Vector3d& vec );
		Vector3d( const Kiwi::Vector3L& vec );
		Vector3d( const Kiwi::Vector3& vec );
		~Vector3d() {}

		void Set( double x, double y, double z );

		//returns the magnitude of this vector
		double Magnitude()const;

		//returns the square magnitude of this vector, to avoid a square root
		double SquareMagnitude()const;

		/*returns the result of normalizing this vector*/
		Kiwi::Vector3d Normalized()const;

		/*returns the result of multiplying this vector by a scalar*/
		Kiwi::Vector3d Scaled( double scalar )const;

		/*returns the dot product of the two vectors: this * vector*/
		double Dot( const Kiwi::Vector3d& vector )const;

		/*returns the cross product of the two vectors: this x vector*/
		Kiwi::Vector3d Cross( const Kiwi::Vector3d& vector )const;

		void ClampMagnitude( double magnitude );

		//returns a string of the form 'x, y, z'
		std::wstring ToString()const;

		void operator= ( const Kiwi::Vector3d& vec );
		void operator= ( const Kiwi::Vector3& vec );

		void operator+= ( const Kiwi::Vector3d& vec );
		void operator-= ( const Kiwi::Vector3d& vec );

		/*returns true if the two vectors are equal*/
		bool operator== ( const Kiwi::Vector3d& vec )const;

		bool operator!= ( const Kiwi::Vector3d& vec )const { return !(*this == vec); }

		//adds the two vectors and returns the result
		Kiwi::Vector3d operator+ ( const Kiwi::Vector3d& vec )const;

		Kiwi::Vector3d operator+ ( const Kiwi::Vector3& vec )const;

		//subtracts the other vector from this one and returns the result
		Kiwi::Vector3d operator- ( const Kiwi::Vector3d& vec )const;
		Kiwi::Vector3d operator- ( const Kiwi::Vector3& vec )const;

		//divides each term of this vector by the amount and returns the result
		Kiwi::Vector3d operator/ ( double amount )const;

		/*returns the result of multiplying this vector by a scalar*/
		Kiwi::Vector3d operator* ( double scalar )const;

		static Kiwi::Vector3d forward() { return Kiwi::Vector3d( 0, 0, 1 ); }
		static Kiwi::Vector3d up() { return Kiwi::Vector3d( 0, 1, 0 ); }
		static Kiwi::Vector3d right() { return Kiwi::Vector3d( 1, 0, 0 ); }

		static double SquareDistance( const Kiwi::Vector3d& a, const Kiwi::Vector3d& b )
		{
			return (std::pow( (a.x - b.x), 2 ) +
					 std::pow( (a.y - b.y), 2 ) +
					 std::pow( (a.z - b.z), 2 ));
		}

		static Kiwi::Vector3d Lerp( const Kiwi::Vector3d& lower, const Kiwi::Vector3d& upper, double percentage )
		{
			return lower + ((upper - lower) * percentage);
		}

		/*clamps x,y, and z individually between lower and upper*/
		static void Clamp( Kiwi::Vector3d& vector, double lower, double upper )
		{
			vector.x = vector.x < lower ? lower : vector.x;
			vector.x = vector.x > upper ? upper : vector.x;

			vector.y = vector.y < lower ? lower : vector.y;
			vector.y = vector.y > upper ? upper : vector.y;

			vector.z = vector.z < lower ? lower : vector.z;
			vector.z = vector.z > upper ? upper : vector.z;
		}

	};

	struct Vector3dHash
	{
		size_t operator()( const Kiwi::Vector3d& v ) const
		{
			size_t h1 = std::hash<double>()(v.x);
			size_t h2 = std::hash<double>()(v.y);
			size_t h3 = std::hash<double>()(v.z);
			return (h1 ^ (h2 << 1)) ^ h3;
		}
	};

	struct Vector3dEquality
	{
		bool operator()( const Kiwi::Vector3d& lhs, const Kiwi::Vector3d& rhs ) const
		{
			return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
		}
	};

}

#endif