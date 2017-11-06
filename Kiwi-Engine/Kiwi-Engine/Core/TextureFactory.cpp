#include "TextureFactory.h"

#include "EngineRoot.h"

#include "..\Graphics\DirectX.h"

#include "../DirectXTK/DDSTextureLoader.h"

namespace Kiwi
{

	std::wstring TextureFactory::ObjectType = L"Texture";

	void TextureFactory::_CreateAsset( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const Kiwi::StringPair& parameters )
	{

		Kiwi::Texture* texture = nullptr;
		ID3D11ShaderResourceView* newShaderRes = nullptr;
		ID3D11Resource* resource = nullptr;
		ID3D11Texture2D* d3dTexture = nullptr;
		DirectX::DDS_ALPHA_MODE alphaMode;
		bool usesTransparency = false;

		std::wstring rendererName;
		Kiwi::Renderer* renderer = _kGraphics.GetActiveRenderer();

		if( renderer == nullptr ) return;

		//d3d device in directx11 is thread safe, can use it here to create the texture
		ID3D11Device* device = renderer->GetDevice();

		std::wstring path;
		auto pItr = parameters.find( L"Path" );
		if( pItr != parameters.end() )
		{
			path = pItr->second;
		}

		if(path.length() == 0) throw Kiwi::Exception( L"TextureFactory::_CreateObject", L"Failed to load texture '" + assetName + L"': invalid file path", KIWI_EXCEPTION::INVALIDPARAMETER );

		HRESULT hr = DirectX::CreateDDSTextureFromFile( device, path.c_str(), &resource, &newShaderRes, 0, &alphaMode );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"TextureFactory::_CreateObject", L"Failed to create shader resource view for texture '" + assetName + L"': " + Kiwi::GetD3DErrorString(hr) + L"\nFile path was: '" + path + L"'", KIWI_EXCEPTION::OBJECTCREATIONFAILED );
		}

		/*check for transparency in the texture parameters*/
		auto itr = parameters.find( L"Transparency" );
		if( itr != parameters.end() )
		{
			if( itr->second.compare( L"0" ) == 0 )
			{
				usesTransparency = false;

			} else
			{
				usesTransparency = true;
			}
		}

		switch( alphaMode )
		{
			/*check for transparency from the loaded dds file. possible values are:
			DDS_ALPHA_MODE_UNKNOWN (0) - This is the default for most .DDS files if the specific metadata isn't present, and it's up to the application to know
			if it's really something else. Viewers should assume the alpha channel is intended for 'normal' alpha blending.
			DDS_ALPHA_MODE_STRAIGHT (1) - This indicates that the alpha channel if present is assumed to be using 'straight' alpha. Viewers should use the alpha channel with 'normal' alpha blending.
			DDS_ALPHA_MODE_PREMULTIPLIED (2) - This indicates the alpha channel if present is premultiplied alpha. This information is only present if the file is written using the
			latest version of the "DX10" extended header, or if the file is BC2/BC3 with the "DXT2"/"DXT4" FourCC which are explicitly stored as
			premultiplied alpha. Viewers should use the alpha channel with premultiplied alpha blending.
			DDS_ALPHA_MODE_OPAQUE (3) - This indicates that the alpha channel if present is fully opaque for all pixels. Viewers can assume there is no alpha blending.
			DDS_ALPHA_MODE_CUSTOM (4) - This indicates the alpha channel if present does not contain transparency (neither straight or premultiplied alpha) and instead is encoding some other
			channel of information. Viewers should not use the alpha channel for blending, and should instead view it as a distinct image channel.
			*/
			case DirectX::DDS_ALPHA_MODE_UNKNOWN:
				{
					KIWIDEBUGSTRING( L"DDS_ALPHA_MODE_UNKNOWN\n" );
					break;
				}
			case DirectX::DDS_ALPHA_MODE_STRAIGHT:
				{
					KIWIDEBUGSTRING( L"DDS_ALPHA_MODE_STRAIGHT\n" );
					usesTransparency = true;
					break;
				}
			case DirectX::DDS_ALPHA_MODE_PREMULTIPLIED:
				{
					KIWIDEBUGSTRING( L"DDS_ALPHA_MODE_PREMULTIPLIED\n" );
					break;
				}
			case DirectX::DDS_ALPHA_MODE_OPAQUE:
				{
					KIWIDEBUGSTRING( L"DDS_ALPHA_MODE_OPAQUE\n" );
					break;
				}
			case DirectX::DDS_ALPHA_MODE_CUSTOM:
				{
					KIWIDEBUGSTRING( L"DDS_ALPHA_MODE_CUSTOM\n" );
					break;
				}
			default:break;
		}

		texture = new Kiwi::Texture( assetName, path, d3dTexture, newShaderRes, usesTransparency );

		_createdAssets.push_back( std::unique_ptr<Kiwi::IAsset>( texture ) );

	}

}