#ifndef _KIWI_BASIC2DSHADER_H_
#define _KIWI_BASIC2DSHADER_H_

#include "..\DirectX.h"
#include "../IShader.h"

#include <string>

namespace Kiwi
{

	class Scene;
	class Material;
	class Renderer;

	struct Submesh;

	class Basic2DShader :
		public Kiwi::IShader
	{

		//friend class DefaultEffect;

	protected:

		//matches the "ObjectBuffer" constant buffer in the vertex shader (per object data)
		struct Vertex_ObjectBuffer
		{
			DirectX::XMMATRIX wvp;
		};

		struct MaterialSettings_PS
		{
			DirectX::XMFLOAT4 diffuseColor;
			float hasDiffuse; //set to 1.0 if the material has a diffuse texture
			float diffuseIsRenderTexture; //set to 1.0 if the diffuse texture is a render target
		};

		//matches the "ObjectBuffer" constant buffer in the pixel shader (per object data)
		struct ObjectSettings_PS
		{
			float renderingText; //set to 1.0 if rendering 2D text
			float textSharpening; //alpha threshold for rendering text, any alpha under this value will be clipped
			float usePerVertexColor; //if set to 1.0, the shader will use the per-vertex color as the diffuse color, otherwise it will use the above diffuseColor
		};

	public:

		Basic2DShader( const std::wstring shaderName, const std::wstring renderer, const std::wstring vsFile, const std::wstring psFile );
		~Basic2DShader() {}

		void SetPerMaterial( Kiwi::Scene& scene, Kiwi::Material& material );
		void SetPerObject( Kiwi::Scene& scene, Kiwi::Viewport& viewport, Kiwi::IModel& model, unsigned int modelSubsetIndex );

	};

}

#endif