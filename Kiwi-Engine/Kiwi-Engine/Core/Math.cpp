#include "Math.h"

#include "../Graphics/DirectX.h"

namespace Kiwi
{

	/*void KiwiMatrix3x3ToD3DXMatrix(const Kiwi::Matrix3& kMat, D3DXMATRIX& matrix)
	{

		matrix._11 = kMat.a1;
		matrix._12 = kMat.a2;
		matrix._13 = kMat.a3;
		matrix._14 = 0.0f;

		matrix._21 = kMat.b1;
		matrix._22 = kMat.b2;
		matrix._23 = kMat.b3;
		matrix._24 = 0.0f;

		matrix._31 = kMat.c1;
		matrix._32 = kMat.c2;
		matrix._33 = kMat.c3;
		matrix._34 = 0.0f;

		matrix._41 = 0.0f;
		matrix._42 = 0.0f;
		matrix._43 = 0.0f;
		matrix._44 = 1.0f;

	}*/

	/*void KiwiMatrix4x4ToD3DXMatrix(const Kiwi::Matrix4& kMat, D3DXMATRIX& matrix)
	{

		matrix._11 = kMat.a1;
		matrix._12 = kMat.a2;
		matrix._13 = kMat.a3;
		matrix._14 = kMat.a4;

		matrix._21 = kMat.b1;
		matrix._22 = kMat.b2;
		matrix._23 = kMat.b3;
		matrix._24 = kMat.b4;

		matrix._31 = kMat.c1;
		matrix._32 = kMat.c2;
		matrix._33 = kMat.c3;
		matrix._34 = kMat.c4;

		matrix._41 = kMat.d1;
		matrix._42 = kMat.d2;
		matrix._43 = kMat.d3;
		matrix._44 = kMat.d4;

	}*/

	void Matrix4ToXMMATRIX( const Kiwi::Matrix4& kMat, DirectX::XMMATRIX& xmMat )
	{

		xmMat = DirectX::XMMATRIX( kMat.a1, kMat.a2, kMat.a3, kMat.a4,
								   kMat.b1, kMat.b2, kMat.b3, kMat.b4,
								   kMat.c1, kMat.c2, kMat.c3, kMat.c4,
								   kMat.d1, kMat.d2, kMat.d3, kMat.d4 );

	}

	void Vector4ToXMFLOAT4( const Kiwi::Vector4& kVec, DirectX::XMFLOAT4& xmFloat )
	{

		xmFloat = DirectX::XMFLOAT4( kVec.x, kVec.y, kVec.z, kVec.w );

	}
	
	void Vector3ToXMFLOAT4( const Kiwi::Vector3& kVec, DirectX::XMFLOAT4& xmFloat )
	{

		xmFloat = DirectX::XMFLOAT4( kVec.x, kVec.y, kVec.z, 0.0f );

	}

};