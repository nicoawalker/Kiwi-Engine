#include "MaterialImporter.h"

#include "../Core/EngineRoot.h"

#include <sstream>
#include <iostream>
#include <fstream>

namespace Kiwi
{

	bool MaterialImporter::_CheckStream( std::wifstream& stream, std::wstring& error )
	{
		if( stream.fail() )
		{
			stream.clear();
			stream >> error;
			return false;
		}

		return true;
	}

	std::wstring MaterialImporter::Import( std::vector<MaterialData>& _matData, const std::wstring& matFile )
	{
		if( matFile.size() == 0 )return L"No material file specified";

		std::wstring errorString;

		std::wifstream inputFile( matFile.c_str() );
		if( inputFile )
		{
			std::wstring curString;

			//store the file path of the obj file
			size_t pos = matFile.find_last_of( L"/" );
			if( pos == std::wstring::npos )
			{
				//no '/' found, check for '\' instead
				pos = matFile.find_last_of( L"\\" );
			}
			std::wstring matPath = matFile.substr( 0, pos + 1 );

			bool diffuseSet = false; //if the diffuse color is not set, the ambient color will be used
			bool transparencySet = false;

			int matIndex = -1; //index of material data struct in the _matData array that is currently being loaded

			//read file
			std::wstring currentLine;
			while( getline( inputFile, currentLine ) )
			{
				if( currentLine.size() <= 1 ) continue; //skip empty lines

				/*trim spaces*/
				while( currentLine[0] == ' ' )
				{
					currentLine = currentLine.substr( 1, std::wstring::npos );
				}

				if( currentLine.size() <= 1 ) continue; //skip empty lines

				//get first word in the line, store in lineType
				std::wstringstream ss( currentLine );
				std::wstring lineType;
				ss >> lineType;

				//check what type of data this line contains, and extract it
				switch( lineType[0] )
				{
					case '#': //comment, skip
						{
							break;
						}
					case 'K':
					case 'k': //Ka, Kd, Ks, Ke
						{
							if( matIndex >= _matData.size() ) break;

							if( lineType.compare( L"Ka" ) == 0 )
							{
								//ambient color
								float color = 0.0f;

								//red
								ss >> color;
								if( !_CheckStream( inputFile, errorString ) ) break;
								_matData[matIndex].colors[2].red = (double)color;

								//green
								ss >> color;
								if( !_CheckStream( inputFile, errorString ) ) break;
								_matData[matIndex].colors[2].green = (double)color;

								//blue
								ss >> color;
								if( !_CheckStream( inputFile, errorString ) ) break;
								_matData[matIndex].colors[2].blue = (double)color;

								if( !diffuseSet )
								{
									//also store it as the diffuse color, if there is not yet a diffuse color set
									_matData[matIndex].colors[0] = _matData[matIndex].colors[2];
								}

							} else if( lineType.compare( L"Kd" ) == 0 )
							{
								//diffuse color
								float color = 0.0f;

								//red
								ss >> color;
								if( !_CheckStream( inputFile, errorString ) ) break;
								_matData[matIndex].colors[0].red = (double)color;

								//green
								ss >> color;
								if( !_CheckStream( inputFile, errorString ) ) break;
								_matData[matIndex].colors[0].green = (double)color;

								//blue
								ss >> color;
								if( !_CheckStream( inputFile, errorString ) ) break;
								_matData[matIndex].colors[0].blue = (double)color;

								/*set transparency only if it hans't been set yet*/
								if( !transparencySet ) _matData[matIndex].colors[0].alpha = 1.0;

								diffuseSet = true;

							} else if( lineType.compare( L"Ks" ) == 0 )
							{
								//specular color
								float color = 0.0f;

								//red
								ss >> color;
								if( !_CheckStream( inputFile, errorString ) ) break;
								_matData[matIndex].colors[3].red = (double)color;

								//green
								ss >> color;
								if( !_CheckStream( inputFile, errorString ) ) break;
								_matData[matIndex].colors[3].green = (double)color;

								//blue
								ss >> color;
								if( !_CheckStream( inputFile, errorString ) ) break;
								_matData[matIndex].colors[3].blue = (double)color;

							} else if( lineType.compare( L"Ke" ) == 0 )
							{
								//emissive color
								float color = 0.0f;

								//red
								ss >> color;
								if( !_CheckStream( inputFile, errorString ) ) break;
								_matData[matIndex].colors[1].red = (double)color;

								//green
								ss >> color;
								if( !_CheckStream( inputFile, errorString ) ) break;
								_matData[matIndex].colors[1].green = (double)color;

								//blue
								ss >> color;
								if( !_CheckStream( inputFile, errorString ) ) break;
								_matData[matIndex].colors[1].blue = (double)color;
							}

							break;
						}
					case 'T':
					case 't': //Tr, Tf
						{
							if( matIndex >= _matData.size() ) break;

							if( lineType.compare( L"Tr" ) == 0 )
							{
								//transparency (1=transparent, 0=opaque)

								float transparency;
								ss >> transparency;
								if( !_CheckStream( inputFile, errorString ) ) break;

								/*store transparency in the diffuse color's alpha channel*/
								if( transparency != 0.0f )
								{
									//flip the transparency so that 1.0f is opaque
									_matData[matIndex].colors[0].alpha = 1.0 - (double)transparency;
								}
								transparencySet = true;

							} else if( lineType.compare( L"Tf" ) == 0 )
							{
								//transmission filter (light passing through is filtered by this amount)
								//Tf r g b
								float color = 0.0f;

								//red
								ss >> color;
								if( !_CheckStream( inputFile, errorString ) ) break;
								_matData[matIndex].colors[4].red = (double)color;

								//green
								ss >> color;
								if( !_CheckStream( inputFile, errorString ) ) break;
								_matData[matIndex].colors[4].green = (double)color;

								//blue
								ss >> color;
								if( !_CheckStream( inputFile, errorString ) ) break;
								_matData[matIndex].colors[4].blue = (double)color;
							}

							break;
						}
					case 'D':
					case 'd': //d
						{
							if( matIndex >= _matData.size() ) break;

							//transparency (0=transparent, 1=opaque)(opposite of Tr)
							if( lineType.size() > 1 ) break;

							float transparency;
							ss >> transparency;
							if( !_CheckStream( inputFile, errorString ) ) break;

							/*store transparency in the diffuse color's alpha channel*/
							if( transparency != 1.0f )
							{
								_matData[matIndex].colors[0].alpha = (double)transparency;
							}

							transparencySet = true;

							break;
						}
					case 'B':
					case 'b':
					case 'M': //texture maps
					case 'm': //map_Ka, map_Kd, map_Ks map_bump, bump
						{
							if( matIndex >= _matData.size() ) break;

							if( lineType.compare( L"map_Ka" ) == 0 ) //ambient map texture
							{
								int pos = (int)currentLine.find_first_of( ' ' ) + 1;
								std::wstring map = currentLine.substr( pos, currentLine.find_first_of( '\n' ) - pos );
								std::wstring textureFile = matPath + map;
								map = map.substr( 0, map.find_last_of( '.' ) ); //remove filetype

								/*queue the texture to be loaded in case it hasn't been yet*/
								_kAssetManager.EnqueueAsset( L"Texture", Kiwi::StringPair { {L"Name", map}, {L"Path", textureFile} } );

								if( map.size() > 0 )
								{
									_matData[matIndex].textures[2] = map;
								}

							} else if( lineType.compare( L"map_Kd" ) == 0 ) //diffuse map texture
							{
								int pos = (int)currentLine.find_first_of( ' ' ) + 1;
								std::wstring map = currentLine.substr( pos, currentLine.find_first_of( '\n' ) - pos );
								std::wstring textureFile = matPath + map;
								map = map.substr( 0, map.find_last_of( '.' ) ); //remove filetype

								/*queue the texture to be loaded in case it hasn't been yet*/
								_kAssetManager.EnqueueAsset( L"Texture", Kiwi::StringPair { { L"Name", map }, { L"Path", textureFile } } );

								if( map.size() > 0 )
								{
									_matData[matIndex].textures[0] = map;
								}

							} else if( lineType.compare( L"map_Ks" ) == 0 ) //specular map texture
							{
								int pos = (int)currentLine.find_first_of( ' ' ) + 1;
								std::wstring map = currentLine.substr( pos, currentLine.find_first_of( '\n' ) - pos );
								std::wstring textureFile = matPath + map;
								map = map.substr( 0, map.find_last_of( '.' ) ); //remove filetype

								/*queue the texture to be loaded in case it hasn't been yet*/
								_kAssetManager.EnqueueAsset( L"Texture", Kiwi::StringPair { { L"Name", map }, { L"Path", textureFile } } );

								if( map.size() > 0 )
								{
									_matData[matIndex].textures[3] = map;
								}

							} else if( lineType.compare( L"map_bump" ) == 0 || lineType.compare( L"bump" ) == 0 ) //bump map texture
							{
								int pos = (int)currentLine.find_first_of( ' ' ) + 1;
								std::wstring map = currentLine.substr( pos, currentLine.find_first_of( '\n' ) - pos );
								std::wstring textureFile = matPath + map;
								map = map.substr( 0, map.find_last_of( '.' ) ); //remove filetype

								/*queue the texture to be loaded in case it hasn't been yet*/
								_kAssetManager.EnqueueAsset( L"Texture", Kiwi::StringPair { { L"Name", map }, { L"Path", textureFile } } );

								if( map.size() > 0 )
								{
									_matData[matIndex].textures[1] = map;
								}
							}

							break;
						}
					case 'N':
					case 'n': //Ns, Ni, newmtl
						{
							if( lineType.compare( L"newmtl" ) == 0 )
							{
								//get the name of the material
								std::wstring mtlName;

								int pos = (int)currentLine.find_first_of( ' ' ) + 1;
								if( pos < currentLine.size() )
								{
									mtlName = currentLine.substr( pos, currentLine.find_first_of( '\n' ) - pos );
								}

								matIndex++;
								_matData.push_back( MaterialData( mtlName, matPath ) );

							} else
							{
								if( matIndex >= _matData.size() ) break;

								if( lineType.compare( L"Ns" ) == 0 && matIndex != -1 )
								{
									//specular power, 0-1000 (reflectivity)
									float reflectivity = 0.0f;
									ss >> reflectivity;

									if( !_CheckStream( inputFile, errorString ) ) break;

									_matData[matIndex].reflectivity = (double)reflectivity;

								} else if( lineType.compare( L"Ni" ) == 0 && matIndex != -1 )
								{
									//optical density, 0-10 (used to bend light)
									float optDensity = 0.0f;
									ss >>optDensity;

									if( !_CheckStream( inputFile, errorString ) ) break;

									_matData[matIndex].opticalDensity = (double)optDensity;
								}
							}

							break;
						}
					case 'I':
					case 'i': //illumination
						{
							if( matIndex >= _matData.size() ) break;

							if( lineType.compare( L"illum" ) == 0 )
							{
								//illumination model, 0-10
								int illum = 0;
								ss >> illum;

								if( !_CheckStream( inputFile, errorString ) ) break;

								_matData[matIndex].illum = illum;
							}

							break;
						}
					case 'S':
					case 's':
						{
							if( matIndex >= _matData.size() ) break;

							if( lineType.compare( L"shader" ) == 0 )
							{
								std::wstring shader;
								int pos = (int)currentLine.find_first_of( ' ' ) + 1;
								if( pos < currentLine.size() )
								{
									shader = currentLine.substr( pos, currentLine.find_first_of( '\n' ) - pos );
								}

								_matData[matIndex].shader = shader;
							}

							break;
						}
					default:break;
				}
			}

			inputFile.close();

		} else
		{
			errorString = L"Failed to open material file \"" + matFile + L"\"";
		}

		return errorString;
	}
}