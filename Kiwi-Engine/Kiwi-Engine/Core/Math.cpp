#include "Math.h"

#include "../Graphics/DirectX.h"
#include "..\Graphics\Color.h"

#include <math.h>

namespace Kiwi
{

	double RoundToNearestd( double value, double nearest )
	{
		return ( round( value * (1.0 / nearest) ) * nearest);
	}

	void Matrix4ToXMMATRIX( const Kiwi::Matrix4& kMat, DirectX::XMMATRIX& xmMat )
	{

		xmMat = DirectX::XMMATRIX( kMat.a1, kMat.a2, kMat.a3, kMat.a4,
								   kMat.b1, kMat.b2, kMat.b3, kMat.b4,
								   kMat.c1, kMat.c2, kMat.c3, kMat.c4,
								   kMat.d1, kMat.d2, kMat.d3, kMat.d4 );

	}

	void Matrix4ToXMFLOAT4X4( const Kiwi::Matrix4& kMat, DirectX::XMFLOAT4X4& xmMat )
	{
		xmMat = DirectX::XMFLOAT4X4( kMat.a1, kMat.a2, kMat.a3, kMat.a4,
								   kMat.b1, kMat.b2, kMat.b3, kMat.b4,
								   kMat.c1, kMat.c2, kMat.c3, kMat.c4,
								   kMat.d1, kMat.d2, kMat.d3, kMat.d4 );
	}

	void Vector4ToXMFLOAT4( const Kiwi::Vector4& kVec, DirectX::XMFLOAT4& xmFloat )
	{

		xmFloat = DirectX::XMFLOAT4( kVec.x, kVec.y, kVec.z, kVec.w );

	}

	void ColorToXMFLOAT4( const Kiwi::Color& kColor, DirectX::XMFLOAT4& xmFloat )
	{
		xmFloat = DirectX::XMFLOAT4( kColor.red, kColor.green, kColor.blue, kColor.alpha );
	}
	
	void Vector3ToXMFLOAT4( const Kiwi::Vector3& kVec, DirectX::XMFLOAT4& xmFloat )
	{

		xmFloat = DirectX::XMFLOAT4( kVec.x, kVec.y, kVec.z, 0.0f );

	}

	void Vector3LToXMFLOAT4( const Kiwi::Vector3L& kVec, DirectX::XMFLOAT4& xmFloat )
	{

		xmFloat = DirectX::XMFLOAT4( (float)kVec.x, (float)kVec.y, (float)kVec.z, 0.0f );

	}

	void Vector3dToXMFLOAT3( const Kiwi::Vector3d& kVec, DirectX::XMFLOAT3& xmFloat )
	{

		xmFloat = DirectX::XMFLOAT3( (float)kVec.x, (float)kVec.y, (float)kVec.z );

	}

	void Vector2dToXMFLOAT2( const Kiwi::Vector2d& kVec, DirectX::XMFLOAT2& xmFloat )
	{

		xmFloat = DirectX::XMFLOAT2( (float)kVec.x, (float)kVec.y );

	}

	void ClipVector( Kiwi::Vector3d& vector, double threshold )
	{

		if( vector.Magnitude() < threshold )
		{
			vector = Kiwi::Vector3d();
		}

	}

	double SquareDistance( const Kiwi::Vector3d& v1, const Kiwi::Vector3d& v2 )
	{
		return (std::pow( (v2.x - v1.x), 2 ) +
				 std::pow( (v2.y - v1.y), 2 ) +
				 std::pow( (v2.z - v1.z), 2 ));
	}

};