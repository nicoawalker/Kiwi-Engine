#ifndef _KIWI_TEXTURE_H_
#define _KIWI_TEXTURE_H_

#include "../Core/IAsset.h"

#include <string>

struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;

namespace Kiwi
{

	class Renderer;

	class Texture :
		public Kiwi::IAsset
	{
	protected:

		std::wstring m_textureName;

		ID3D11Texture2D* m_texture;
		ID3D11ShaderResourceView* m_shaderResource;

		//store how many objects are currently using this texture
		int m_referenceCount;

		bool m_active;

	public:

		Texture( std::wstring name, std::wstring file, ID3D11Texture2D* texture, ID3D11ShaderResourceView* shaderResource );
		~Texture();

		void IncrementReferenceCount();
		void DecrementReferenceCount();

		int IsActive()const { return m_active; }

		ID3D11Texture2D* GetTexture()const { return m_texture; }
		ID3D11ShaderResourceView* GetShaderResourceView()const { return m_shaderResource; }

		std::wstring GetName()const { return m_textureName; }

	};
};

#endif
