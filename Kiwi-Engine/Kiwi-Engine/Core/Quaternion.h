#ifndef _KIWI_QUATERNION_H_
#define _KIWI_QUATERNION_H_

#include "Vector3.h"
#include "Matrix4.h"

#include <string>

namespace Kiwi
{

	class Quaternion
	{
	private:

		//normalizes the quaternion
		void _Normalize();

	public:

		float w, x, y, z;

	public:

		/*creates the identity quaternion*/
		Quaternion();

		/*creates a quaternion initialized to the given values*/
		Quaternion(float s, float x, float y, float z);

		/*converts a quaternion rotated around the axis by rotationAmount (in degrees)*/
		Quaternion(const Kiwi::Vector3& axis, float angle);

		/*copies the other quaternion into this one*/
		Quaternion(const Quaternion& otherQuat);

		~Quaternion();

		//manually set new values for the quaternion
		void Set(float w, float x, float y, float z);
		//sets the quaternion to be a rotation of 'angle' degrees around 'axis'
		void Set(const Kiwi::Vector3& axis, float angle);

		/*returns a new quaternion equal to this quaternion scaled by a value*/
		Quaternion Scaled(float scale)const;

		/*returns a new quaternion equal to a quaternion added to this quaternion*/
		Quaternion Sum(const Quaternion& quat)const;

		/*returns a new quaternion equal to the cross product of this quaternion and another*/
		Quaternion Cross(const Quaternion& quat)const;

		/*returns this quaternion, normalized*/
		Quaternion Normalized()const;

		/*returns the conjugate of this quaternion*/
		Quaternion Conjugate()const;

		/*returns the inverse of this quaternion*/
		Quaternion Inverse()const;

		/*returns the dot product of this quaternion and the other one*/
		float Dot(const Quaternion& quat)const;

		/*rotates a point around this quaternion and returns the result*/
		Kiwi::Vector3 RotatePoint(const Kiwi::Vector3& point)const;

		//returns the norm of the quaternion
		float Norm()const;

		Kiwi::Vector3 GetEulerAngles()const;

		/*recalculates the rotation matrix*/
		Kiwi::Matrix4 ToRotationMatrix()const;

		//returns a formatted string of this quaternion: "w, x, y, z"
		std::wstring ToString()const;

		/*returns the result of the cross product between the two quaternions*/
		Quaternion operator* (const Quaternion& quat)const;

		/*makes a copy of the given quaternion*/
		void operator= (const Quaternion& quat);

		bool operator== (const Quaternion& quat)const;

		//returns the identity quaternion (AKA quaternion which represents default/no rotation)
		static Quaternion Identity() { return Quaternion(1.0f, 0.0f, 0.0f, 0.0f); }

		/*returns a quaternion created by:
		1. rotating about the z-axis by vec.z radians
		2. rotating about the x-axis by vec.x radians
		3. rotating about the y-axis by vec.y radians*/
		static Quaternion FromEuler(const Kiwi::Vector3& vec);

		/*Returns a quaternion q such that v1*q = v2; the quaternion that rotates v1 to v2
		Both v1 and v2 are assumed to be normalized*/
		static Quaternion RotateTo(const Kiwi::Vector3& v1, const Kiwi::Vector3& v2);

	};
};

#endif