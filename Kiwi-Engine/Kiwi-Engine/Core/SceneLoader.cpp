#include "SceneLoader.h"

#include "Scene.h"
#include "Utilities.h"
#include "IAsset.h"

#include "../Graphics/D3DAssetLoader.h"
#include "../Graphics/Texture.h"
#include "../Graphics/DirectX.h"
#include "../Graphics/OBJImporter.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/Material.h"
#include "../Graphics/Renderer.h"

#include <future>
#include <fstream>

namespace Kiwi
{

	unsigned long SceneLoader::AssetID::id = 0;

	SceneLoader::SceneLoader( Kiwi::Scene* scene )
	{

		if( !scene ) { throw Kiwi::Exception( L"SceneLoader", L"Invalid scene" ); }

		try
		{
			m_scene = scene;

			m_totalQueuedAssets = 0;
			m_totalSizeInBytes = 0;
			m_numAssetsLoaded = 0;
			m_loadedSizeInBytes = 0;
			m_isLoading = false;

		} catch( ... )
		{
			throw;
		}

	}

	SceneLoader::~SceneLoader()
	{

		m_scene = 0;

	}

	void SceneLoader::LoadFromFile( std::wstring filename )
	{



	}

	void SceneLoader::Start( unsigned long maxThreads )
	{

		if( !m_isLoading )
		{
			m_isLoading = true;
			m_totalQueuedAssets = (unsigned int)m_loadQueue.size();
			m_numAssetsLoaded = 0;
			m_loadedSizeInBytes = 0;
			m_loadQueueThread = std::thread( &SceneLoader::_LoadQueue, this, maxThreads );
		}

	}

	void SceneLoader::_LoadQueue( unsigned long maxThreads )
	{

		std::vector<std::future<Kiwi::IAsset*>> assetFutures;

		//first swap the contents of the load queue into a local queue to avoid lots of un/locking
		std::deque<Kiwi::AssetDesc> assetQueue;
		m_loadQueueMutex.lock();
			assetQueue.swap( m_loadQueue );
		m_loadQueueMutex.unlock();

		//start the first threads, up to maxThreads
		for( unsigned long thread = 0; thread < maxThreads && assetQueue.size() > 0; thread++ )
		{
			//launch a new loader thread, passing in the front of the asset queue
			//the future is then stored so we can retrieve the returned asset later
			if( this->_CheckType( assetQueue.front().assetType ) )
			{
				assetFutures.push_back( std::async( std::launch::async, &SceneLoader::_LoadAsset, this, assetQueue.front() ) );
			} else
			{
				assetFutures.push_back( std::async( std::launch::async, &SceneLoader::_LoadCustomAsset, this, assetQueue.front() ) );
			}

			assetQueue.pop_front();
		}

		while( true )
		{
			//if there are no more futures, loading is done
			if( assetFutures.size() == 0 )
			{
				m_isLoading = false;
				break;
			}

			//loop through the futures and check for any ready/finished ones
			for( auto itr = assetFutures.begin(); itr != assetFutures.end(); )
			{
				auto fStatus = itr->wait_for( std::chrono::milliseconds( 0 ) );
				if( fStatus == std::future_status::ready )
				{
					try
					{
						//this thread has ended, and the loaded asset is in the future
						//attempt to get the value (if the thread threw this will throw the exception)
						Kiwi::IAsset* asset = itr->get();

						//lock the scene, and add the new loaded asset
						std::unique_lock<std::mutex> sLock(*m_scene->m_mutex);
						m_scene->AddAsset( asset );

					} catch( const Kiwi::Exception& e )
					{
						//store the exception
						m_exceptionMutex.lock();
							m_caughtExceptions.push_back( e );
						m_exceptionMutex.unlock();
					}

					//remove this future, as we are done with it
					itr = assetFutures.erase( itr );

				} else
				{
					//future is not ready, skip to the next one
					itr++;
				}
			}

			//if required, spawn new threads
			while( assetQueue.size() > 0 && assetFutures.size() < maxThreads )
			{
				if( this->_CheckType( assetQueue.front().assetType ) )
				{
					//built-in type
					assetFutures.push_back( std::async( std::launch::async, &SceneLoader::_LoadAsset, this, assetQueue.front() ) );

				} else
				{
					//custom type
					assetFutures.push_back( std::async( std::launch::async, &SceneLoader::_LoadCustomAsset, this, assetQueue.front() ) );
				}

				assetQueue.pop_front();
			}

			std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );

		}

	}

	Kiwi::IAsset* SceneLoader::_LoadAsset( Kiwi::AssetDesc asset )
	{

		std::wstring name = asset.name;
		std::wstring type = asset.assetType;

		if( asset.files.size() == 0 ) throw Kiwi::Exception( L"SceneLoader_LoadAsset", L"No file specified" );

		try
		{

			if( type.compare( L"Texture" ) == 0 )
			{
				std::wstring filepath = asset.files[0];

				return this->_LoadTexture( name, filepath );

			} else if( type.compare( L"StaticMesh" ) == 0 )
			{
				std::wstring filepath = asset.files[0];

				return this->_LoadOBJMesh( name, filepath );

			} else if( type.compare( L"AnimatedMesh" ) == 0 )
			{
				return 0;

			} else if( type.compare( L"Font" ) == 0 )
			{
				if( asset.files.size() < 2 ) throw Kiwi::Exception( L"SceneLoader_LoadAsset", L"Wrong number of files for font loading" );

				std::wstring fontFile = asset.files[0];
				std::wstring textureFile = asset.files[1];

				return this->_LoadFont( name, fontFile, textureFile );

			} else if( type.compare( L"Shader" ) == 0 )
			{
				if( asset.files.size() < 2 ) throw Kiwi::Exception( L"SceneLoader_LoadAsset", L"Wrong number of files for shader loading" );

				std::wstring vsFile = asset.files[0];
				std::wstring psFile = asset.files[1];

				return this->_LoadShader( name, vsFile, psFile );

			} else
			{
				return 0;
			}

		} catch( ... )
		{
			throw;
		}

	}

	Kiwi::IAsset* SceneLoader::_LoadTexture( std::wstring name, std::wstring textureFile )
	{

		assert( m_scene != 0 && m_scene->GetRenderer() != 0 && m_scene->GetRenderer()->GetDevice() != 0 );

		try
		{

			Kiwi::Texture* texture = 0;

			ID3D11ShaderResourceView* newShaderRes = 0;
			//d3d device in directx11 is thread safe, can use it here to create the texture
			ID3D11Device* device = m_scene->GetRenderer()->GetDevice();

			HRESULT hr = D3DX11CreateShaderResourceViewFromFileW( device, textureFile.c_str(), NULL, NULL, &newShaderRes, NULL );
			if( FAILED( hr ) )
			{
				throw Kiwi::Exception( L"D3DAssetLoader::LoadTexture", L"Failed to create shader resource view for texture '" + name + L"'" );
			}

			texture = new Kiwi::Texture( name, textureFile, 0, newShaderRes );

			if( texture != 0 )
			{
				m_byteMutex.lock();
					m_loadedSizeInBytes += sizeof( Kiwi::Texture );
				m_byteMutex.unlock();
			}

			return texture;

		} catch( ... )
		{
			throw;
		}

	}

	Kiwi::IAsset* SceneLoader::_LoadOBJMesh( std::wstring name, std::wstring objFile )
	{

		try
		{
			Kiwi::OBJImporter objImporter;
			OBJMeshData* objMesh = 0;

			//import the raw data from the OBJ File
			objMesh = objImporter.Import( objFile );

			std::vector<Kiwi::Mesh::Subset> subsets( objMesh->vGroups.size() );

			//for each materialdata in the mesh, create a material and load any textures
			//then place these in a MeshSubset, along with the vertices to load into the mesh
			for( unsigned int i = 0; i < objMesh->vGroups.size(); i++ )
			{

				OBJVertexGroup* data = &objMesh->vGroups[i];

				Kiwi::Texture* ambientMap = 0;
				Kiwi::Texture* diffuseMap = 0;
				Kiwi::Texture* specularMap = 0;
				Kiwi::Texture* bumpMap = 0;

				//any textures that aren't yet loaded, load them here
				if( data->mData.ambientMap.size() > 0 )
				{

					std::wstring filename = data->mData.ambientMap;
					//store the texture name without the path and file type extension
					std::wstring texName;
					size_t pos = filename.find_last_of( L"/" ); 
					if( pos != std::wstring::npos )
					{
						//remove the path and file extension, leaving just the name
						texName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

					} else
					{
						pos = filename.find_last_of( L"\\" );
						if( pos != std::wstring::npos )
						{
							//remove the path and file extension, leaving just the name
							texName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

						} else
						{
							//didnt find any '/' or '\' characters so there appears to be no path, just remove the extension
							texName = filename.substr( 0, filename.find_last_of( L"." ) );
						}
					}

					if( texName.size() == 0 )
					{
						//no name was generated, so there is nothing to load or search for
						break;
					}

					texName += L"_ambient";

					//check if the texture is already loaded and in the scene
					m_scene->m_mutex->lock();
						ambientMap = m_scene->FindAssetWithName<Kiwi::Texture>( texName );
					m_scene->m_mutex->unlock();

					if( ambientMap == 0 )
					{
						Kiwi::IAsset* ambientMapAsset = this->_LoadTexture( texName, filename );
						if( ambientMapAsset != 0 )
						{
							//texture not found, try to load it now	
							ambientMap = dynamic_cast<Kiwi::Texture*>(ambientMapAsset);
						} else
						{
							//failed to load texture
						}
					}
				}	

				if( data->mData.diffuseMap.size() > 0 )
				{

					std::wstring filename = data->mData.diffuseMap;
					//store the texture name without the path and file type extension
					std::wstring texName;
					size_t pos = filename.find_last_of( L"/" );
					if( pos != std::string::npos )
					{
						//remove the path and file extension, leaving just the name
						texName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

					} else
					{
						pos = filename.find_last_of( L"\\" );
						if( pos != std::string::npos )
						{
							//remove the path and file extension, leaving just the name
							texName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

						} else
						{
							//didnt find any '/' or '\' characters so there appears to be no path, just remove the extension
							texName = filename.substr( 0, filename.find_last_of( L"." ) - (pos + 1) );
						}
					}

					if( texName.size() == 0 )
					{
						//no name was generated, so there is nothing to load or search for
						break;
					}

					texName += L"_diffuse";

					//check if the texture is already loaded and in the scene
					m_scene->m_mutex->lock();
						diffuseMap = m_scene->FindAssetWithName<Kiwi::Texture>( texName );
					m_scene->m_mutex->unlock();

					if( diffuseMap == 0 )
					{
						Kiwi::IAsset* diffuseMapAsset = this->_LoadTexture( texName, filename );
						if( diffuseMapAsset != 0 )
						{
							//texture not found, try to load it now	
							diffuseMap = dynamic_cast<Kiwi::Texture*>( diffuseMapAsset );
						} else
						{
							//failed to load texture
						}
					}
				}

				if( data->mData.specularMap.size() > 0)
				{

					std::wstring filename = data->mData.specularMap;
					//store the texture name without the path and file type extension
					std::wstring texName;
					size_t pos = filename.find_last_of( L"/" );
					if( pos != std::wstring::npos )
					{
						//remove the path and file extension, leaving just the name
						texName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

					} else
					{
						pos = filename.find_last_of( L"\\" );
						if( pos != std::wstring::npos )
						{
							//remove the path and file extension, leaving just the name
							texName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

						} else
						{
							//didnt find any '/' or '\' characters so there appears to be no path, just remove the extension
							texName = filename.substr( 0, filename.find_last_of( L"." ) );
						}
					}

					if( texName.size() == 0 )
					{
						//no name was generated, so there is nothing to load or search for
						break;
					}

					texName += L"_specular";

					//check if the texture is already loaded and in the scene
					m_scene->m_mutex->lock();
						specularMap = m_scene->FindAssetWithName<Kiwi::Texture>( texName );
					m_scene->m_mutex->unlock();

					if( specularMap == 0 )
					{
						Kiwi::IAsset* specMapAsset = this->_LoadTexture( texName, filename );
						if( specMapAsset != 0 )
						{
							//texture not found, try to load it now	
							specularMap = dynamic_cast<Kiwi::Texture*>(specMapAsset);
						} else
						{
							//failed to load texture
						}
					}
				}

				if( data->mData.bumpMap.size() > 0 )
				{

					std::wstring filename = data->mData.bumpMap;
					//store the texture name without the path and file type extension
					std::wstring texName;
					size_t pos = filename.find_last_of( L"/" );
					if( pos != std::wstring::npos )
					{
						//remove the path and file extension, leaving just the name
						texName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

					} else
					{
						pos = filename.find_last_of( L"\\" );
						if( pos != std::wstring::npos )
						{
							//remove the path and file extension, leaving just the name
							texName = filename.substr( pos + 1, filename.find_last_of( L"." ) - (pos + 1) );

						} else
						{
							//didnt find any '/' or '\' characters so there appears to be no path, just remove the extension
							texName = filename.substr( 0, filename.find_last_of( L"." ) );
						}
					}

					if( texName.size() == 0 )
					{
						//no name was generated, so there is nothing to load or search for
						break;
					}

					texName += L"_bump";

					//check if the texture is already loaded and in the scene
					m_scene->m_mutex->lock();
						bumpMap = m_scene->FindAssetWithName<Kiwi::Texture>( texName );
					m_scene->m_mutex->unlock();

					if( bumpMap == 0 )
					{
						Kiwi::IAsset* bumpMapAsset = this->_LoadTexture( texName, filename );
						if( bumpMapAsset != 0 )
						{
							//texture not found, try to load it now	
							bumpMap = dynamic_cast<Kiwi::Texture*>(bumpMapAsset);
						} else
						{
							//failed to load texture
						}
					}
				}

				//now create the material
				Kiwi::Material newMaterial(data->mData.name, diffuseMap, bumpMap, ambientMap, specularMap);
				newMaterial.SetColor( L"Diffuse", data->mData.diffuseColor );
				newMaterial.SetColor( L"Ambient", data->mData.ambientColor );
				newMaterial.SetColor( L"Specular", data->mData.specularColor );
				newMaterial.SetColor( L"Emissive", data->mData.emissiveColor );
				newMaterial.SetColor( L"Transmission", data->mData.transmissionFilter );
				newMaterial.SetIllumination( data->mData.illum );
				newMaterial.SetOpticalDensity( data->mData.opticalDensity );
				newMaterial.SetReflectivity( data->mData.reflectivity );

				subsets[i].material = newMaterial;
				subsets[i].startIndex = 0;
				subsets[i].endIndex = 0;

				//convert the raw vertex data into MeshVertex structs for the mesh
				for( unsigned int a = 0; a < data->vertices.size(); a++ )
				{
					Kiwi::Mesh::Vertex v;
					v.position = data->vertices[a].position;
					v.normals = data->vertices[a].normals;
					v.textureUV = data->vertices[a].textureUV;

					subsets[i].vertices.push_back( v );
				}

			}

			SAFE_DELETE( objMesh );

			//the subsets, vertices, and materials are loaded and created, now create the mesh
			std::unique_lock<std::mutex> sLock( *m_scene->m_mutex );
				Kiwi::Renderer* renderer = m_scene->GetRenderer();
			sLock.unlock();

			Kiwi::Mesh* mesh = new Kiwi::Mesh( name, objFile, renderer, subsets );

			Kiwi::FreeMemory( subsets );

			m_byteMutex.lock();
				m_loadedSizeInBytes += sizeof( Kiwi::Mesh );
			m_byteMutex.unlock();

			return mesh;

		} catch( ... )
		{
			throw;
		}

	}

	Kiwi::IAsset* SceneLoader::_LoadFont( std::wstring name, std::wstring fontFile, std::wstring textureFile )
	{
		return 0;
		//std::wifstream fin;

		//std::vector<Kiwi::Font::FontCharacter> characterSet;
		//int imageWidth, imageHeight;
		//int startChar;
		//int cellWidth, cellHeight;
		//int fontHeight;
		//wchar_t temp;

		//fin.open( filepath );
		//if( fin.fail() )
		//{
		//	throw Kiwi::Exception(L"SceneLoader::_LoadFont", L"Failed to open font file '" + filepath + L"'" );
		//}

		////get the image width
		//fin.get( temp );
		//while( temp != ',' )
		//{
		//	fin.get( temp );
		//}
		//fin >> imageWidth;

		////get the image height
		//fin.get( temp );
		//while( temp != ',' )
		//{
		//	fin.get( temp );
		//}
		//fin >> imageHeight;

		////get the cell width
		//fin.get( temp );
		//while( temp != ',' )
		//{
		//	fin.get( temp );
		//}
		//fin >> cellWidth;

		////get the cell height
		//fin.get( temp );
		//while( temp != ',' )
		//{
		//	fin.get( temp );
		//}
		//fin >> cellHeight;

		////get the ascii value of the first letter
		//fin.get( temp );
		//while( temp != ',' )
		//{
		//	fin.get( temp );
		//}
		//fin >> startChar;

		////get the name of the font
		//fin.get( temp );
		//while( temp != ',' )
		//{
		//	fin.get( temp );
		//}
		//fin >> name;

		////get the font height
		//fin.get( temp );
		//while( temp != ',' )
		//{
		//	fin.get( temp );
		//}
		//fin >> fontHeight;

		////add the font height to the end of the font name
		//name += L"_" + ToWString( fontHeight );

		////skip font width
		//fin.get( temp );
		//while( temp != ',' )
		//{
		//	fin.get( temp );
		//}
		//fin.get( temp );

		///*amount to multiply the position in the texture with in order
		//to get the texture coordinates between 0 and 1*/
		//float texU = 1.0f / (float)imageWidth;
		//float texV = 1.0f / (float)imageHeight;

		////how many cells/characters there are vertically and horizontally in the texture
		//int numCellsX = imageWidth / cellWidth;
		//int numCellsY = imageHeight / cellHeight;

		////now read in the widths of each of the 256 letters
		////and calculate the U,V coordinates of each within the texture
		//for( int i = 0; i < 256; i++ )
		//{

		//	int width;

		//	fin.get( temp );
		//	while( temp != ',' )
		//	{
		//		fin.get( temp );
		//	}
		//	fin >> width;

		//	Kiwi::Font::FontCharacter newChar;

		//	float left = (float)(i%numCellsX) * (float)cellWidth; //x position in texture
		//	left *= texU; //convert to U,V coordinates
		//	newChar.left = left;

		//	float right = (float)(i%numCellsX) * (float)cellWidth + (float)width;
		//	right *= texU;
		//	newChar.right = right;

		//	float top = (int)(i / numCellsY) * (float)cellHeight; //y position in texture
		//	top *= texV; //convert to U,V coordinates
		//	newChar.top = top;

		//	float bottom = top;
		//	bottom += (float)fontHeight * texV;
		//	newChar.bottom = bottom;

		//	newChar.charWidth = width;
		//	newChar.charHeight = fontHeight;

		//	characterSet.push_back( newChar );

		//}

		//fin.close();

		//Kiwi::Font* newFont = new Kiwi::Font();

		//m_byteMutex.lock();
		//	m_loadedSizeInBytes += sizeof( Kiwi::Font );
		//m_byteMutex.unlock();

		//return newFont;

	}

	Kiwi::IAsset* SceneLoader::_LoadShader( std::wstring name, std::wstring vsFile, std::wstring psFile )
	{
		return 0;
		/*try
		{
			D3DAssetLoader loader;

			m_assetID.guard.lock();
				unsigned int id = m_assetID.id++;
			m_assetID.guard.unlock();

			Kiwi::Shader* shader = new Kiwi::Shader( name, id, vsFile, psFile, loader.GetDevice() );

			m_byteMutex.lock();
				m_loadedSizeInBytes += sizeof( Kiwi::Shader* );
			m_byteMutex.unlock();

			return shader;

		} catch( ... )
		{
			throw;
		}*/

	}

	bool SceneLoader::Finished()
	{

		m_exceptionMutex.lock();
			if( m_caughtExceptions.size() != 0 )
			{
				Kiwi::Exception e = m_caughtExceptions[0];
				m_caughtExceptions.erase( m_caughtExceptions.begin() );
				throw e;
			}
		m_exceptionMutex.unlock();

		if( m_isLoading )
		{
			return false;
		} else
		{
			if( m_loadQueueThread.joinable() )
			{
				m_loadQueueThread.join();
			}
			return true;
		}

	}

	bool SceneLoader::_CheckType( std::wstring type )
	{

		if( type.compare(L"Texture") == 0 || type.compare(L"StaticMesh") == 0 || type.compare(L"AnimatedMesh") == 0 ||
			type.compare(L"Font") == 0 || type.compare(L"Shader") == 0 )
		{
			return true;

		} else
		{
			//custom type
			return false;
		}

	}

}