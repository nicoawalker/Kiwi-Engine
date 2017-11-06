#ifndef _KIWI_MATH_H_
#define _KIWI_MATH_H_

#include "Vector2.h"
#include "Vector2d.h"
#include "Vector3.h"
#include "Vector3d.h"
#include "Vector3L.h"
#include "Vector4.h"
#include "Quaternion.h"
//#include "Matrix3.h"
#include "Matrix4.h"

//struct D3DXMATRIX;
namespace DirectX
{
	struct XMMATRIX;
	struct XMFLOAT2;
	struct XMFLOAT3;
	struct XMFLOAT4;
	struct XMFLOAT4X4;
}

namespace Kiwi
{
	class Color;

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

	/*rounds 'value' to the nearest multiple of 'nearest', and returns the result as a double*/
	double RoundToNearestd( double value, double nearest );

	template<typename T>
	void clamp(T& value, T lower, T upper)
	{
		value = value < lower ? lower : value;
		value = value > upper ? upper : value;
	}

	/*returns 0 if value if below the threshold, else returns value*/
	template<typename T>
	T ClipBelow( T value, T threshold )
	{
		return (value < threshold) ? static_cast<T>(0.0) : value;
	}

	/*returns value1 if value1 >= value2, otherwise returns value 2*/
	template<typename T>
	T Larger(T value1, T value2)
	{

		return (value1 >= value2) ? value1 : value2;

	}

	/*returns value1 if value1 <= value2, otherwise returns value 2*/
	template<typename T>
	T Smaller(T value1, T value2)
	{

		return (value1 <= value2) ? value1 : value2;

	}

	/*copies a Kiwi::Matrix4 into an XMMATRIX*/
	void Matrix4ToXMMATRIX( const Kiwi::Matrix4& kMat, DirectX::XMMATRIX& xmMat );
	void Matrix4ToXMFLOAT4X4( const Kiwi::Matrix4& kMat, DirectX::XMFLOAT4X4& xmMat );

	/*copies a Kiwi::Vector4 into an XMFLOAT4*/
	void Vector4ToXMFLOAT4( const Kiwi::Vector4& kVec, DirectX::XMFLOAT4& xmFloat );
	void ColorToXMFLOAT4( const Kiwi::Color& kColor, DirectX::XMFLOAT4& xmFloat );
	/*copies a Kiwi::Vector3 into an XMFLOAT4*/
	void Vector3ToXMFLOAT4( const Kiwi::Vector3& kVec, DirectX::XMFLOAT4& xmFloat );
	void Vector3LToXMFLOAT4( const Kiwi::Vector3L& kVec, DirectX::XMFLOAT4& xmFloat );

	void Vector3dToXMFLOAT3( const Kiwi::Vector3d& kVec, DirectX::XMFLOAT3& xmFloat );
	void Vector2dToXMFLOAT2( const Kiwi::Vector2d& kVec, DirectX::XMFLOAT2& xmFloat );

	/*if the magnitude of the vector is less than threshold, sets the vector to 0*/
	void ClipVector( Kiwi::Vector3d& vector, double threshold );

	/*returns the square of the straight-line distance between v1 and v2*/
	double SquareDistance( const Kiwi::Vector3d& v1, const Kiwi::Vector3d& v2 );

};

#endif