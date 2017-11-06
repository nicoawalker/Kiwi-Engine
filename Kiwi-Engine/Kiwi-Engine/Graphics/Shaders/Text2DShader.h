#ifndef _KIWI_TEXT2DSHADER_H_
#define _KIWI_TEXT2DSHADER_H_

#include "..\DirectX.h"
#include "../IShader.h"

#include <string>

namespace Kiwi
{

	class Text2DShader :
		public Kiwi::IShader
	{

		//friend class DefaultEffect;

	protected:

		//matches the "PerObjectData" constant buffer in the vertex shader (per object data)
		struct PerObjectData_VS
		{
			DirectX::XMMATRIX wvp;
		};

		/*matches the TextProperties constant buffer in the pixel shader (per object data)*/
		struct TextProperties_PS
		{
			DirectX::XMFLOAT4 properties; // x = sharpening: clips any pixel below this alpha value
		};

	public:

		Text2DShader( const std::wstring shaderName, const std::wstring renderer, const std::wstring vsFile, const std::wstring psFile );
		~Text2DShader() {}

		void SetPerMaterial( Kiwi::Scene& scene, Kiwi::Material& material );
		void SetPerObject( Kiwi::Scene& scene, Kiwi::Viewport& viewport, Kiwi::IModel& model, unsigned int modelSubsetIndex );

	};

}

#endif