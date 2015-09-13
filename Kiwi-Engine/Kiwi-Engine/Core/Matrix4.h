#ifndef _KIWI_MATRIX4_H_
#define _KIWI_MATRIX4_H_

#include "Vector3.h"

#include <string>

namespace Kiwi
{

	class Matrix4
	{
	public:

		float a1, a2, a3, a4;
		float b1, b2, b3, b4;
		float c1, c2, c3, c4;
		float d1, d2, d3, d4;

	public:

		Matrix4();
		~Matrix4() {}

		Kiwi::Matrix4 Transpose()const;

		/*returns a string representation of the matrix*/
		std::wstring ToString()const;

		//fills the upper-left 3x3 of this matrix with the 3x3 matrix
		//fills the last row and column with 0s
		//void operator= (const Kiwi::Matrix3& matrix3);

		void operator= (const Kiwi::Matrix4& m4);
		Kiwi::Matrix4 operator* (const Kiwi::Matrix4& m4)const;

		static Matrix4 Translation(const Kiwi::Vector3& translation);
		static Matrix4 Scaling(const Kiwi::Vector3& scaling);
		static Matrix4 RotationZXY(const Kiwi::Vector3& eulerAngles){}
		static Matrix4 Projection(float fovy, float aspectRatio, float nearView, float farView);
		static Matrix4 Orthographic(float screenWidth, float screenHeight, float nearView, float farView);

	};
};

#endif