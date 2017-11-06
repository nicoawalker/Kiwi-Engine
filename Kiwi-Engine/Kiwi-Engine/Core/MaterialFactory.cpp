#include "MaterialFactory.h"
#include "Scene.h"
#include "EngineRoot.h"

#include "../Graphics/MaterialImporter.h"

#include <sstream>
#include <iostream>
#include <fstream>

namespace Kiwi
{

	std::wstring MaterialFactory::ObjectType = L"Material";

	void MaterialFactory::_CreateAsset( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const Kiwi::StringPair& parameters )
	{
		auto itr = parameters.find( L"MaterialFile" );
		if( itr != parameters.end() )
		{
			MaterialImporter importer;
			std::vector<MaterialData> data;
			
			std::wstring error = importer.Import( data, itr->second );
			if( error.compare( L"" ) != 0 )
			{
				throw Kiwi::Exception( L"MaterialFactory::_CreateAsset", L"An error occured loading material \"" + assetName + L"\": " + error + L"\n" );
			}

			if( data.size() == 0 )
			{
				throw Kiwi::Exception( L"MaterialFactory::_CreateAsset", L"An error occured loading material \"" + assetName + L"\": no materials were found in target file\n" );
			}

			for( MaterialData& matData : data )
			{
				std::unique_ptr<Material> newMat = std::make_unique<Material>( matData.materialName, matData.textures, matData.colors, matData.shader,
																			   matData.reflectivity, matData.opticalDensity, matData.illum );
				if( newMat == nullptr )continue;

				newMat->AddProperty( L"MaterialFile", itr->second );

				_createdAssets.push_back( std::move( newMat ) );
			}

		} else
		{
			/*create an empty material*/
			_createdAssets.push_back( std::make_unique<Material>( assetName ) );
		}
	}

}