#ifndef _KIWI_ISHADER_H_
#define _KIWI_ISHADER_H_

#include "DirectX.h"

#include <string>

namespace Kiwi
{

	class Scene;
	class Renderer;
	class IRenderable;
	class RenderTarget;
	class IShaderEffect;

	class IShader
	{
	protected:

		std::wstring m_shaderName;

		//shader objects
		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;
		//raw shader data loaded from compiled shader .cso files
		ID3D10Blob* m_vertexBlob;
		ID3D10Blob* m_pixelBlob;
		//array of samplers used by the shader
		ID3D11SamplerState** m_samplers;
		unsigned int m_numSamplers;
		//array of pixel buffers used
		ID3D11Buffer** m_pixelBuffers;
		unsigned int m_numPixelBuffers;
		//array of vertex buffers used
		ID3D11Buffer** m_vertexBuffers;
		unsigned int m_numVertexBuffers;

		ID3D11InputLayout* m_inputLayout;

		std::wstring m_vertexShaderFile;
		std::wstring m_pixelShaderFile;

	protected:

		virtual void _CreateInputLayout( Kiwi::Renderer* renderer, D3D11_INPUT_ELEMENT_DESC polygonLayout[], unsigned int layoutSize, ID3D11InputLayout** layout );
		virtual void _CreateSampler( Kiwi::Renderer* renderer, D3D11_SAMPLER_DESC samplerDesc, ID3D11SamplerState** sampler );
		virtual void _CreateBuffer( Kiwi::Renderer* renderer, D3D11_BUFFER_DESC bufferDesc, ID3D11Buffer** buffer );

	public:

		IShader( std::wstring shaderName, Kiwi::Renderer* renderer, std::wstring vertexShaderFile, std::wstring pixelShaderFile );
		virtual ~IShader() = 0;

		/*binds the shader on the rendering pipeline so that all subsequent
		draw calls will be drawn using this shader*/
		virtual void Bind( Kiwi::Renderer* renderer );

		/*sets parameters that are only updated once per scene*/
		virtual void SetSceneParameters( Kiwi::Scene* scene ) {}
		/*sets parameters that are updated once each frame*/
		virtual void SetFrameParameters( Kiwi::Scene* scene ) {}
		/*sets parameters that are updated for each entity*/
		virtual void SetObjectParameters( Kiwi::Scene* scene, Kiwi::RenderTarget* renderTarget, Kiwi::IRenderable* renderable ) {}

		std::wstring GetName()const { return m_shaderName; }

	};
}

#endif