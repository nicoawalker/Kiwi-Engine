#ifndef _KIWI_ISHADER_H_
#define _KIWI_ISHADER_H_

#include "DirectX.h"

#include "../Core/IAsset.h"

#include <string>
#include <vector>

namespace Kiwi
{
	class Scene;
	class Renderer;
	class Viewport;
	class IShaderEffect;
	class Material;
	class IModel;

	class IShader:
		public Kiwi::IAsset
	{
	friend class Renderer;
	protected:

		Kiwi::Renderer* m_renderer;

		std::wstring m_shaderName;

		//shader objects
		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;

		//raw shader data loaded from compiled shader .cso files
		ID3D10Blob* m_vertexBlob;
		ID3D10Blob* m_pixelBlob;

		std::vector<ID3D11Buffer*> m_vBuffers;
		std::vector<ID3D11Buffer*> m_pBuffers;
		std::vector<ID3D11SamplerState*> m_samplerStates;

		ID3D11InputLayout* m_inputLayout;

		std::wstring m_vertexShaderFile;
		std::wstring m_pixelShaderFile;

	public:

		IShader( const std::wstring shaderName, const std::wstring assetSubcategory, const std::wstring renderer, const std::wstring vertexShaderFile, const std::wstring pixelShaderFile );
		virtual ~IShader() = 0;

		virtual std::unique_ptr<Kiwi::IAsset> Clone() { return nullptr; }

		/*sets parameters that are only updated once per scene*/
		virtual void SetPerScene( Kiwi::Scene& scene ) {}

		/*sets parameters that are updated once each frame*/
		virtual void SetPerFrame( Kiwi::Scene& scene ) {}

		/*sets parameters that are updated for each unique material*/
		virtual void SetPerMaterial( Kiwi::Scene& scene, Kiwi::Material& material ) {}

		/*sets parameters that are updated for each entity*/
		virtual void SetPerObject( Kiwi::Scene& scene, Kiwi::Viewport& viewport, Kiwi::IModel& model, unsigned int modelSubsetIndex ) {}

		std::wstring GetName()const { return m_shaderName; }

	};
}

#endif