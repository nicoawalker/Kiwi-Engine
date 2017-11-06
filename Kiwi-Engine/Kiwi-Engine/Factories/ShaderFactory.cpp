#include "ShaderFactory.h"

#include "../Graphics/Shaders/BasicShader.h"
#include "..\Graphics\Shaders\Basic2DShader.h"
#include "../Graphics/Shaders/Text2DShader.h"

namespace Kiwi
{

	std::wstring ShaderFactory::ObjectType = L"Shader";

	void ShaderFactory::_CreateAsset( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const Kiwi::StringPair& parameters )
	{

		std::wstring psFile;
		std::wstring vsFile;
		std::wstring renderer;
		std::wstring shaderType;

		auto itr = parameters.find( L"PS" );
		if( itr != parameters.end() )
		{
			psFile = itr->second;

		} else
		{
			throw Kiwi::Exception( L"ShaderFactory::_CreateAsset", L"Failed to create shader '" + assetName + L"': invalid pixel shader", KIWI_EXCEPTION::INVALIDPARAMETER );
		}

		itr = parameters.find( L"VS" );
		if( itr != parameters.end() )
		{
			vsFile = itr->second;

		} else
		{
			throw Kiwi::Exception( L"ShaderFactory::_CreateAsset", L"Failed to create shader '" + assetName + L"': invalid vertex shader", KIWI_EXCEPTION::INVALIDPARAMETER );
		}

		itr = parameters.find( L"Renderer" );
		if( itr != parameters.end() )
		{
			renderer = itr->second;

		} else
		{
			throw Kiwi::Exception( L"ShaderFactory::_CreateAsset", L"Failed to create shader '" + assetName + L"': invalid renderer", KIWI_EXCEPTION::INVALIDPARAMETER );
		}

		itr = parameters.find( L"ShaderType" );
		if( itr != parameters.end() )
		{
			shaderType = itr->second;

		} else
		{
			throw Kiwi::Exception( L"ShaderFactory::_CreateAsset", L"Failed to create shader '" + assetName + L"': invalid shader type", KIWI_EXCEPTION::INVALIDPARAMETER );
		}

		if( shaderType.compare( L"Text2D" ) == 0 )
		{
			Kiwi::Text2DShader* shader = new Kiwi::Text2DShader( assetName, renderer, vsFile, psFile );
			_createdAssets.push_back( std::unique_ptr<Kiwi::IAsset>( shader ) );

		} else if( shaderType.compare( L"Basic" ) == 0 )
		{
			Kiwi::BasicShader* shader = new Kiwi::BasicShader( assetName, renderer, vsFile, psFile );
			_createdAssets.push_back( std::unique_ptr<Kiwi::IAsset>( shader ) );

		} else if( shaderType.compare( L"Basic2D" ) == 0 )
		{
			Kiwi::Basic2DShader* shader = new Kiwi::Basic2DShader( assetName, renderer, vsFile, psFile );
			_createdAssets.push_back( std::unique_ptr<Kiwi::IAsset>( shader ) );

		}

		throw Kiwi::Exception( L"ShaderFactory::_CreateAsset", L"Failed to create shader '" + assetName + L"': invalid shader type", KIWI_EXCEPTION::INVALIDPARAMETER );
	}

}