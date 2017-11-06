#include "Basic2DShaderFactory.h"

#include "../Graphics/Shaders/Basic2DShader.h"

namespace Kiwi
{

	std::wstring Basic2DShaderFactory::ObjectType = L"Basic2DShader";

	void Basic2DShaderFactory::_CreateAsset( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const Kiwi::StringPair& parameters )
	{

		std::wstring psFile;
		std::wstring vsFile;
		std::wstring renderer;

		auto itr = parameters.find( L"PS" );
		if( itr != parameters.end() )
		{
			psFile = itr->second;

		} else
		{
			throw Kiwi::Exception( L"DefaultShaderFactory::_CreateAsset", L"Failed to create shader '" + assetName + L"': invalid pixel shader", KIWI_EXCEPTION::INVALIDPARAMETER );
		}

		itr = parameters.find( L"VS" );
		if( itr != parameters.end() )
		{
			vsFile = itr->second;

		} else
		{
			throw Kiwi::Exception( L"DefaultShaderFactory::_CreateAsset", L"Failed to create shader '" + assetName + L"': invalid vertex shader", KIWI_EXCEPTION::INVALIDPARAMETER );
		}

		itr = parameters.find( L"Renderer" );
		if( itr != parameters.end() )
		{
			renderer = itr->second;

		} else
		{
			throw Kiwi::Exception( L"DefaultShaderFactory::_CreateAsset", L"Failed to create shader '" + assetName + L"': invalid renderer", KIWI_EXCEPTION::INVALIDPARAMETER );
		}

		Kiwi::Basic2DShader* shader = new Kiwi::Basic2DShader( assetName, renderer, vsFile, psFile );

		_createdAssets.push_back( std::unique_ptr<Kiwi::IAsset>( shader ) );

	}

}