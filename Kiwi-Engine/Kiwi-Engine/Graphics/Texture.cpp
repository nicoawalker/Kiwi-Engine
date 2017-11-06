#include "Texture.h"
#include "DirectX.h"
#include "Renderer.h"

#include "../Core/Utilities.h"
#include "..\Core\Exception.h"
#include "../Core/Assert.h"
#include "..\Core\EngineRoot.h"

namespace Kiwi
{

	Texture::Texture( std::wstring name, std::wstring file ) :
		IAsset( name, L"Texture" )
	{
		m_assetProperties.insert( std::make_pair( L"Texture", file ) );

		m_referenceCount = 0;
		m_shaderResource = 0;
		m_texture = 0;
		m_active = false;
		m_hasTransparency = false;
		m_isRenderTexture = false;

	}

	Texture::Texture( std::wstring name, std::wstring file, ID3D11Texture2D* texture, ID3D11ShaderResourceView* shaderResource, bool hasTransparency ) :
		IAsset( name, L"Texture" )
	{
		m_assetProperties.insert( std::make_pair( L"Texture", file ) );

		m_referenceCount = 0;
		m_shaderResource = shaderResource;
		m_texture = texture;
		m_active = false;
		m_isRenderTexture = false;
		m_hasTransparency = hasTransparency;

	}

	Texture::~Texture()
	{
		SAFE_RELEASE( m_texture );
		SAFE_RELEASE( m_shaderResource );
	}

	void Texture::IncrementReferenceCount()
	{

		m_referenceCount++;
		m_active = true;

	}

	void Texture::DecrementReferenceCount()
	{

		m_referenceCount--;
		if( m_referenceCount <= 0 )
		{
			m_referenceCount = 0;
			m_active = false;
		}

	}

	void Texture::SetTexture( ID3D11Texture2D* newTexture )
	{

		SAFE_RELEASE( m_texture );

		m_texture = newTexture;

	}

	void Texture::SetShaderResourceView( ID3D11ShaderResourceView* newSRV )
	{

		SAFE_RELEASE( m_shaderResource );

		m_shaderResource = newSRV;

	}

};