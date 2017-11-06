#ifndef _KIWI_TEXTURE_H_
#define _KIWI_TEXTURE_H_

#include "../Core/IAsset.h"

#include <string>

struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;

namespace Kiwi
{

	class Renderer;
	class SceneLoader;

	class Texture :
		public Kiwi::IAsset
	{
		friend class SceneLoader;
	protected:

		ID3D11Texture2D* m_texture;
		ID3D11ShaderResourceView* m_shaderResource;

		//store how many objects are currently using this texture
		int m_referenceCount;

		bool m_active;
		bool m_hasTransparency;
		bool m_isRenderTexture; //whether or not the texture is used as a render target

	public:

		Texture( std::wstring name, std::wstring file = L"" );
		Texture( std::wstring name, std::wstring file, ID3D11Texture2D* texture, ID3D11ShaderResourceView* shaderResource, bool hasTransparency = false );
		~Texture();

		virtual std::unique_ptr<Kiwi::IAsset> Clone() { return nullptr; }

		void IncrementReferenceCount();
		void DecrementReferenceCount();

		int IsActive()const { return m_active; }
		bool IsRenderTexture()const { return m_isRenderTexture; }
		bool HasTransparency()const { return m_hasTransparency; }

		void SetTexture( ID3D11Texture2D* newTexture );
		void SetShaderResourceView( ID3D11ShaderResourceView* newSRV );

		ID3D11Texture2D* GetTexture()const { return m_texture; }
		ID3D11ShaderResourceView* GetShaderResourceView()const { return m_shaderResource; }
	};
};

#endif
