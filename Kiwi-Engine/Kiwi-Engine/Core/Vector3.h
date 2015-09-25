#ifndef _KIWI_VECTOR3_H_
#define _KIWI_VECTOR3_H_

#include <string>

namespace Kiwi
{

	class Vector3
	{
	public:

		float x, y, z;

	public:

		Vector3();
		Vector3(float x, float y, float z);
		Vector3(const Kiwi::Vector3& vec);
		~Vector3(){}

		void Set(float x, float y, float z);

		//returns the magnitude of this vector
		float Magnitude()const;

		//returns the square magnitude of this vector, to avoid a square root
		float SquareMagnitude()const;

		/*returns the result of normalizing this vector*/
		Kiwi::Vector3 Normalized()const;

		/*returns the result of multiplying this vector by a scalar*/
		Kiwi::Vector3 Scaled(float scalar)const;

		/*returns the dot product of the two vectors: this * vector*/
		float Dot(const Kiwi::Vector3& vector)const;

		/*returns the cross product of the two vectors: this x vector*/
		Kiwi::Vector3 Cross(const Kiwi::Vector3& vector)const;

		//returns a string of the form 'x, y, z'
		std::wstring ToString()const;

		void operator= (const Kiwi::Vector3& vec);
		void operator+= (const Kiwi::Vector3& vec);

		/*returns true if the two vectors are equal*/
		bool operator== (const Kiwi::Vector3& vec)const;

		//adds the two vectors and returns the result
		Kiwi::Vector3 operator+ (const Kiwi::Vector3& vec)const;

		//subtracts the other vector from this one and returns the result
		Kiwi::Vector3 operator- (const Kiwi::Vector3& vec)const;

		//divides each term of this vector by the amount and returns the result
		Kiwi::Vector3 operator/ (float amount)const;

		/*returns the result of multiplying this vector by a scalar*/
		Kiwi::Vector3 operator* ( float scalar )const;

		static Kiwi::Vector3 forward() { return Kiwi::Vector3(0.0f, 0.0f, 1.0f); }
		static Kiwi::Vector3 up() { return Kiwi::Vector3(0.0f, 1.0f, 0.0f); }
		static Kiwi::Vector3 right() { return Kiwi::Vector3(1.0f, 0.0f, 0.0f); }

	};
};

#endif