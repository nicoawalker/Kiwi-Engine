#ifndef _KIWI_VECTOR4_H_
#define _KIWI_VECTOR4_H_

#include <string>

namespace Kiwi
{

	class Vector4
	{
	public:

		float x, y, z, w;

	public:

		Vector4();
		Vector4(float x, float y, float z, float w);
		Vector4(const Vector4& newVec);

		~Vector4(){}

		void Set(float x, float y, float z, float w);
		void Set(const Vector4& newVector);

		//returns the magnitude of this vector
		float Magnitude()const;

		//returns the square magnitude of this vector, to avoid a square root
		float SquareMagnitude()const;

		std::wstring ToString()const;

		Vector4 operator+ (const Vector4& v);

		void operator= (const Vector4& v);
		void operator+= (const Vector4& v);

	};
};

#endif