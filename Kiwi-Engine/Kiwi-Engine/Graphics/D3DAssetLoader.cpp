#include "D3DAssetLoader.h"

#include "DirectX.h"
#include "Texture.h"
#include "Renderer.h"

#include "../Core/Exception.h"
#include "../Core/Utilities.h"
#include "../Core/Assert.h"
#include "../Core/Scene.h"

namespace Kiwi
{

	D3DAssetLoader::D3DAssetLoader()
	{

		//using DX 11
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

		//create the D3D device object that will be used to load D3D assets
		HRESULT hr = D3D11CreateDevice( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel,
										1, D3D11_SDK_VERSION, &m_device, NULL, &m_deviceContext );
		if( FAILED( hr ) )
		{

			std::wstring error = Kiwi::GetD3DErrorString( hr );

			throw Kiwi::Exception( L"D3DAssetLoader", L"Failed to create D3D device (" + error + L")" );
			return;
		}

	}

	D3DAssetLoader::~D3DAssetLoader()
	{

		SAFE_RELEASE( m_device );
		SAFE_RELEASE( m_deviceContext );

	}

	Kiwi::Texture* D3DAssetLoader::LoadTexture( Kiwi::Renderer* renderer, std::wstring textureName, std::wstring filename )
	{

		assert( m_device != 0 );

		try
		{
			Kiwi::Texture* texture = 0;

			//ID3D11Texture2D* texture = 0;
			ID3D11ShaderResourceView* newShaderRes = 0;

			HRESULT hr = D3DX11CreateShaderResourceViewFromFileW( renderer->GetDevice(), filename.c_str(), NULL, NULL, &newShaderRes, NULL );
			if( FAILED( hr ) )
			{
				MessageBox( NULL, L"B", L"A", MB_OK );
				throw Kiwi::Exception( L"D3DAssetLoader::LoadTexture", L"Failed to create shader resource view for texture '" + filename + L"'" );
			}

			texture = new Kiwi::Texture( textureName, filename, 0, newShaderRes );

			return texture;

		} catch( ... )
		{
			throw;
		}

	}

};