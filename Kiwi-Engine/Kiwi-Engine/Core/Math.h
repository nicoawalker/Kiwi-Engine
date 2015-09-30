#ifndef _KIWI_MATH_H_
#define _KIWI_MATH_H_

#include "Vector2.h"
#include "Vector3.h"
#include "Vector3L.h"
#include "Vector4.h"
#include "Quaternion.h"
//#include "Matrix3.h"
#include "Matrix4.h"

//struct D3DXMATRIX;
namespace DirectX
{
	struct XMMATRIX;
	struct XMFLOAT4;
}

namespace Kiwi
{

	#define KIWI_PI 3.14159265358979323846
	#define KIWI_PIOVER180 0.0174532925199432
	#define KIWI_180OVERPI 57.2957795130823209

	template<typename T>
	float ToRadians(T degrees)
	{
		return (float)degrees * (float)KIWI_PIOVER180;
	}

	template<typename T>
	float ToDegrees(T radians)
	{
		return (float)radians * (float)KIWI_180OVERPI;
	}

	template<typename T>
	void clamp(T& value, T lower, T upper)
	{
		if(value < (T)lower)
		{
			value = lower;
		}else if(value > (T)upper)
		{
			value = upper;
		}
	}

	/*returns value1 if value1 >= value2, otherwise returns value 2*/
	template<typename T>
	T LargerOf(T value1, T value2)
	{

		return (value1 >= value2) ? value1 : value2;

	}

	/*returns value1 if value1 <= value2, otherwise returns value 2*/
	template<typename T>
	T SmallerOf(T value1, T value2)
	{

		return (value1 <= value2) ? value1 : value2;

	}

	/*copies a Kiwi::Matrix4 into an XMMATRIX*/
	void Matrix4ToXMMATRIX( const Kiwi::Matrix4& kMat, DirectX::XMMATRIX& xmMat );

	/*copies a Kiwi::Vector4 into an XMFLOAT4*/
	void Vector4ToXMFLOAT4( const Kiwi::Vector4& kVec, DirectX::XMFLOAT4& xmFloat );
	/*copies a Kiwi::Vector3 into an XMFLOAT4*/
	void Vector3ToXMFLOAT4( const Kiwi::Vector3& kVec, DirectX::XMFLOAT4& xmFloat );

	void Vector3LToXMFLOAT4( const Kiwi::Vector3L& kVec, DirectX::XMFLOAT4& xmFloat );

};

#endif