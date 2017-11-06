#include "MD5ModelFactory.h"
#include "TextureFactory.h"
#include "Scene.h"
#include "EngineRoot.h"

#include "..\Graphics\DirectX.h"
#include "../Graphics/OBJImporter.h"
#include "..\Graphics\Mesh.h"
#include "../Graphics/MD5Model.h"
#include "..\Graphics\MD5Importer.h"

#include "../DirectXTK/DDSTextureLoader.h"

#include <sstream>
#include <iostream>
#include <fstream>

namespace Kiwi
{

	std::wstring MD5ModelFactory::ObjectType = L"MD5Model";

	void MD5ModelFactory::_CreateAsset( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const Kiwi::StringPair& parameters )
	{
		auto itr = parameters.find( L"ModelFile" );
		if( itr != parameters.end() )
		{
			MD5Importer importer;
			MD5Data data;
			std::unique_ptr<MD5Model> loadedModel = nullptr;
			std::wstring error = importer.Import( data, itr->second );
			if( error.compare( L"" ) != 0 )
			{
				throw Kiwi::Exception( L"MD5ModelFactory::_CreateAsset", L"An error occured loading MD5 model \"" + assetName + L"\": " + error + L"\n" );
			}

			loadedModel = std::make_unique<MD5Model>( assetName, data.vertices, data.indices, data.subsets, data.joints, data.weights );
			loadedModel->AddProperty( L"ModelFile", itr->second );

			_createdAssets.push_back( std::move( loadedModel ) );
		}
	}

}