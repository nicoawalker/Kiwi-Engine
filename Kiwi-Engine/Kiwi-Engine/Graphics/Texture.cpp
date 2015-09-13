#include "Texture.h"
#include "DirectX.h"
#include "Renderer.h"

#include "../Core/Utilities.h"
#include "../Core/Logger.h"
#include "../Core/Exception.h"
#include "../Core/Assert.h"

namespace Kiwi
{

	Texture::Texture( std::wstring name, std::wstring file, ID3D11Texture2D* texture, ID3D11ShaderResourceView* shaderResource ) :
		IAsset( name, L"Texture" )
	{

		m_textureName = name;

		m_assetFiles.push_back( file );

		m_referenceCount = 0;
		m_shaderResource = shaderResource;
		m_texture = texture;
		m_active = false;

	}

	Texture::~Texture()
	{

		if( m_referenceCount > 0 )
		{
			_Logger.Log( L"Texture '" + m_textureName + L"' was destroyed with reference count " + Kiwi::ToWString( m_referenceCount ) );
		}

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

};