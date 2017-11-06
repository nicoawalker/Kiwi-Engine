#include "StaticModelFactory.h"
#include "TextureFactory.h"
#include "Scene.h"
#include "EngineRoot.h"

#include "../Graphics/StaticModel.h"
#include "..\Graphics\DirectX.h"
#include "../Graphics/OBJImporter.h"
#include "..\Graphics\Mesh.h"

#include "../DirectXTK/DDSTextureLoader.h"

#include <sstream>
#include <iostream>
#include <fstream>

namespace Kiwi
{

	std::wstring StaticModelFactory::ObjectType = L"StaticModel";

	void StaticModelFactory::_CreateAsset( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const Kiwi::StringPair& parameters )
	{

		bool shared = false;
		auto itr = parameters.find( L"IsShared" );
		if( itr != parameters.end() )
		{
			if( itr->second.compare( L"true" ) == 0 )
			{
				shared = true;
			}
		}

		itr = parameters.find( L"ModelFile" );
		if( itr != parameters.end() )
		{
			this->_LoadModel( _createdAssets, assetName, itr->second, shared );
			return;
		} 

		itr = parameters.find( L"PrimitiveType" );
		if( itr != parameters.end() )
		{
			/*create a primitive model*/
			if( itr->second.compare( L"quad" ) == 0 )
			{
				std::unique_ptr<Kiwi::StaticModel> model = std::make_unique<Kiwi::StaticModel>( assetName, MODEL_PRIMITIVE_TYPE::QUAD );
				if( model == nullptr ) return;

				model->SetShared( shared );
				_createdAssets.push_back( std::move( model ) );
				return;

			} else if( itr->second.compare( L"cube" ) == 0 )
			{
				std::unique_ptr<Kiwi::StaticModel> model = std::make_unique<Kiwi::StaticModel>( assetName, MODEL_PRIMITIVE_TYPE::CUBE );
				if( model == nullptr ) return;

				model->SetShared( shared );
				_createdAssets.push_back( std::move( model ) );
				return;
			}
		}

		/*create an empty model*/
		std::unique_ptr<Kiwi::StaticModel> model = std::make_unique<Kiwi::StaticModel>( assetName );
		if( model == nullptr ) return;

		model->SetShared( shared );
		_createdAssets.push_back( std::move( model ) );
	}

	void StaticModelFactory::_LoadModel( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const std::wstring& modelFile, bool isShared )
	{
		std::wstring objFile = modelFile;
		Kiwi::OBJImporter objImporter;
		OBJMeshData* objMesh = 0;
		std::unique_ptr<Kiwi::StaticModel> newModel = nullptr;
		std::vector<Kiwi::Vertex> vertices;
		std::vector<unsigned long> indices;

		//import the raw data from the OBJ File
		objMesh = objImporter.Import( objFile );

		std::vector<Kiwi::IModel::Subset> subsets;

		/*queue the required materials for loading*/
		for( std::wstring& matFile : objMesh->materialFiles )
		{
			_kEngine.GetAssetManager().EnqueueAsset( L"Material", Kiwi::StringPair { { L"MaterialFile", matFile } } );
		}
		_kEngine.GetAssetManager().Load();

		//for each materialdata in the mesh, create a material and load any textures
		//then place these in a MeshSubset, along with the vertices to load into the mesh
		unsigned int index = 0;
		for( unsigned int i = 0; i < objMesh->vGroups.size(); i++ )
		{
			OBJVertexGroup* data = &objMesh->vGroups[i];

			std::wstring ambientMapName;
			std::wstring diffuseMapName;
			std::wstring specularMapName;
			std::wstring bumpMapName;

			//any textures that aren't yet loaded, load them here
			if( data->mData.ambientMap.size() > 0 )
			{
				std::wstring filename = data->mData.ambientMap;
				//store the texture name without the path and file type extension
				size_t pos = filename.find_last_of( L"/" );
				if( pos != std::wstring::npos )
				{
					//remove the path and file extension, leaving just the name
					ambientMapName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

				} else
				{
					pos = filename.find_last_of( L"\\" );
					if( pos != std::wstring::npos )
					{
						//remove the path and file extension, leaving just the name
						ambientMapName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

					} else
					{
						//didnt find any '/' or '\' characters so there appears to be no path, just remove the extension
						ambientMapName = filename.substr( 0, filename.find_last_of( L"." ) );
					}
				}
			}

			if( data->mData.diffuseMap.size() > 0 )
			{
				std::wstring filename = data->mData.diffuseMap;
				//store the texture name without the path and file type extension
				size_t pos = filename.find_last_of( L"/" );
				if( pos != std::string::npos )
				{
					//remove the path and file extension, leaving just the name
					diffuseMapName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

				} else
				{
					pos = filename.find_last_of( L"\\" );
					if( pos != std::string::npos )
					{
						//remove the path and file extension, leaving just the name
						diffuseMapName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

					} else
					{
						//didnt find any '/' or '\' characters so there appears to be no path, just remove the extension
						diffuseMapName = filename.substr( 0, filename.find_last_of( L"." ) - (pos + 1) );
					}
				}
			}

			if( data->mData.specularMap.size() > 0 )
			{
				std::wstring filename = data->mData.specularMap;
				//store the texture name without the path and file type extension
				size_t pos = filename.find_last_of( L"/" );
				if( pos != std::wstring::npos )
				{
					//remove the path and file extension, leaving just the name
					specularMapName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

				} else
				{
					pos = filename.find_last_of( L"\\" );
					if( pos != std::wstring::npos )
					{
						//remove the path and file extension, leaving just the name
						specularMapName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

					} else
					{
						//didnt find any '/' or '\' characters so there appears to be no path, just remove the extension
						specularMapName = filename.substr( 0, filename.find_last_of( L"." ) );
					}
				}
			}

			if( data->mData.bumpMap.size() > 0 )
			{
				std::wstring filename = data->mData.bumpMap;
				//store the texture name without the path and file type extension
				size_t pos = filename.find_last_of( L"/" );
				if( pos != std::wstring::npos )
				{
					//remove the path and file extension, leaving just the name
					bumpMapName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

				} else
				{
					pos = filename.find_last_of( L"\\" );
					if( pos != std::wstring::npos )
					{
						//remove the path and file extension, leaving just the name
						bumpMapName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

					} else
					{
						//didnt find any '/' or '\' characters so there appears to be no path, just remove the extension
						bumpMapName = filename.substr( 0, filename.find_last_of( L"." ) );
					}
				}
			}

			subsets.push_back( Kiwi::IModel::Subset( data->mData.name, index, (unsigned int)data->vertices.size() - 1,
													 index, (unsigned int)data->vertices.size() - 1 ) );

			/*translate the obj vertices into model vertices and create the indices*/
			for( OBJVertex& v : data->vertices )
			{
				vertices.push_back( Kiwi::Vertex( Kiwi::Vector3d( v.position ), Kiwi::Vector2d( v.textureUV ), Kiwi::Vector3d( v.normals ) ) );
				indices.push_back( (unsigned long)index );
				index++;
			}
		}

		newModel = std::make_unique<Kiwi::StaticModel>( assetName, subsets, vertices, indices );

		if( newModel != nullptr )
		{
			newModel->SetShared( isShared );

			/*add the related files to the asset*/
			newModel->AddProperty( L"ModelFile", objFile );
		}

		SAFE_DELETE( objMesh );

		_createdAssets.push_back( std::move( newModel ) );
	}

}