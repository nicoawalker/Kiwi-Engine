#ifndef _KIWI_VECTOR4D_H_
#define _KIWI_VECTOR4D_H_

#include <string>

namespace Kiwi
{

	class Vector4d
	{
	public:

		double x, y, z, w;

	public:

		Vector4d();
		Vector4d( double x, double y, double z, double w );
		Vector4d( const Vector4d& newVec );

		~Vector4d() {}

		void Set( double x, double y, double z, double w );
		void Set( const Vector4d& newVector );

		//returns the magnitude of this vector
		double Magnitude()const;

		//returns the square magnitude of this vector, to avoid a square root
		double SquareMagnitude()const;

		std::wstring ToString()const;

		Vector4d operator+ ( const Vector4d& v )const;

		void operator= ( const Vector4d& v );
		void operator+= ( const Vector4d& v );
		
		bool operator!= ( const Vector4d& other )const;

	};
};

#endif