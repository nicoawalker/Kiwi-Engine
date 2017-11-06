#include "StaticMeshFactory.h"
#include "TextureFactory.h"
#include "Scene.h"
#include "EngineRoot.h"

#include "..\Graphics\DirectX.h"
#include "../Graphics/OBJImporter.h"

#include "..\DirectXTK\DDSTextureLoader.h"

namespace Kiwi
{

	std::wstring StaticMeshFactory::ObjectType = L"StaticMesh";

	std::unique_ptr<Kiwi::IAsset> StaticMeshFactory::_CreateAsset( std::wstring assetName, const Kiwi::StringPair& parameters )
	{

		Kiwi::StaticMeshAsset* meshAsset = 0;
		std::wstring objFile;
		std::wstring path;
		Kiwi::OBJImporter objImporter;
		OBJMeshData* objMesh = 0;
		std::vector<Kiwi::Vector3d> vertices;
		std::vector<Kiwi::Vector2d> uvs;
		std::vector<Kiwi::Vector3d> normals;

		auto pItr = parameters.find( L"Path" );
		if( pItr != parameters.end() )
		{
			path = pItr->second;
		}

		if( path.length() == 0 ) throw Kiwi::Exception( L"StaticMeshFactory::_CreateAsset", L"Failed to load static mesh '" + assetName + L"': invalid file path", KIWI_EXCEPTION::INVALIDPARAMETER );

		objFile = path;

		//import the raw data from the OBJ File
		objMesh = objImporter.Import( objFile );

		std::vector<Kiwi::Submesh> subsets( objMesh->vGroups.size() );

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

			//now create the material
			Kiwi::Material newMaterial( diffuseMapName, bumpMapName, ambientMapName, specularMapName );
			newMaterial.SetColor( Material::COLOR_TYPE::DIFFUSE, data->mData.diffuseColor );
			newMaterial.SetColor( Material::COLOR_TYPE::AMBIENT, data->mData.ambientColor );
			newMaterial.SetColor( Material::COLOR_TYPE::SPECULAR, data->mData.specularColor );
			newMaterial.SetColor( Material::COLOR_TYPE::EMISSIVE, data->mData.emissiveColor );
			newMaterial.SetColor( Material::COLOR_TYPE::TRANSMISSION_FILTER, data->mData.transmissionFilter );
			newMaterial.SetIllumination( data->mData.illum );
			newMaterial.SetOpticalDensity( data->mData.opticalDensity );
			newMaterial.SetReflectivity( data->mData.reflectivity );

			subsets[i].material = newMaterial;
			subsets[i].startIndex = index;
			subsets[i].endIndex = index + (unsigned int)data->vertices.size() - 1;

			index += (unsigned int)data->vertices.size();

			//convert the raw vertex data into MeshVertex structs for the mesh
			for( unsigned int a = 0; a < data->vertices.size(); a++ )
			{
				vertices.push_back( Kiwi::Vector3d( data->vertices[a].position.x, data->vertices[a].position.y, data->vertices[a].position.z ) );
				uvs.push_back( Kiwi::Vector2d( data->vertices[a].textureUV.x, data->vertices[a].textureUV.y ) );
				normals.push_back( Kiwi::Vector3d( data->vertices[a].normals.x, data->vertices[a].normals.y, data->vertices[a].normals.z ) );
			}
		}

		meshAsset = new Kiwi::StaticMeshAsset( assetName, subsets, vertices, uvs, normals );

		/*add the related files to the asset*/
		meshAsset->AddProperty( L"OBJ", objFile );

		SAFE_DELETE( objMesh );

		return std::unique_ptr<Kiwi::IAsset>( meshAsset );

	}

}