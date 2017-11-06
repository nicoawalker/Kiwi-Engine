#include "BasicShaderFactory.h"

#include "../Graphics/Shaders/BasicShader.h"

namespace Kiwi
{

	std::wstring BasicShaderFactory::ObjectType = L"BasicShader";

	void BasicShaderFactory::_CreateAsset( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const Kiwi::StringPair& parameters )
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
			throw Kiwi::Exception( L"BasicShaderFactory::_CreateAsset", L"Failed to create shader '" + assetName + L"': invalid pixel shader", KIWI_EXCEPTION::INVALIDPARAMETER );
		}

		itr = parameters.find( L"VS" );
		if( itr != parameters.end() )
		{
			vsFile = itr->second;

		} else
		{
			throw Kiwi::Exception( L"BasicShaderFactory::_CreateAsset", L"Failed to create shader '" + assetName + L"': invalid vertex shader", KIWI_EXCEPTION::INVALIDPARAMETER );
		}

		itr = parameters.find( L"Renderer" );
		if( itr != parameters.end() )
		{
			renderer = itr->second;

		} else
		{
			throw Kiwi::Exception( L"BasicShaderFactory::_CreateAsset", L"Failed to create shader '" + assetName + L"': invalid renderer", KIWI_EXCEPTION::INVALIDPARAMETER );
		}

		Kiwi::BasicShader* shader = new Kiwi::BasicShader( assetName, renderer, vsFile, psFile );
		_createdAssets.push_back( std::unique_ptr<Kiwi::IAsset>( shader ) );
	}

}