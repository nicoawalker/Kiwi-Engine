#include "FontFactory.h"
#include "TextureFactory.h"
#include "Scene.h"
#include "EngineRoot.h"

#include "..\Graphics\DirectX.h"
#include "..\DirectXTK\DDSTextureLoader.h"

#include <fstream>

namespace Kiwi
{

	std::wstring FontFactory::ObjectType = L"Font";

	void FontFactory::_CreateAsset( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const Kiwi::StringPair& parameters )
	{

		std::wifstream fin;

		std::wstring path;
		auto pItr = parameters.find( L"Path" );
		if( pItr != parameters.end() )
		{
			path = pItr->second;
		}

		if( path.length() == 0 ) throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Failed to load font '" + assetName + L"': no file path specified", KIWI_EXCEPTION::INVALIDPARAMETER );

		std::vector<Kiwi::Font::Character> characterSet;
		std::wstring fontFile = path;
		std::wstring temp;
		std::wstring fontFace;
		std::wstring line;
		std::wstring textureName;
		std::wstring textureFilename;
		std::wstring directory;
		int size = 0;
		bool bold = false;
		bool italic = false;
		bool unicode = false;
		bool smoothing = false;
		int outlineThickness = 0;
		int base = 0;
		int numPages = 0;
		int charCount = 0;
		int imageWidth = 0, imageHeight = 0;
		int cellWidth = 0, cellHeight = 0;
		int fontHeight = 0;

		directory = fontFile.substr( 0, fontFile.find_last_of( L"/" ) + 1 ); //get path up to last forward slash
		if( directory.length() == 0 )
		{
			directory = fontFile.substr( 0, fontFile.find_last_of( L"\\" ) + 1 ); //get path up to last backslash
			if( directory.length() == 0 )
			{
				throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Invalid/unspecified directory in font file '" + fontFile + L"'" );
			}
		}

		fin.open( fontFile );
		if( fin.fail() )
		{
			throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Failed to open font file '" + fontFile + L"'" );
		}

		//process line 1
		std::getline( fin, line );

		temp = line.substr( line.find( L"face=\"" ) + 6, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L"\"" ) );
		if( temp.length() == 0 )
		{
			throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Typeface not found in font file '" + fontFile + L"'" );
		}
		fontFace = temp;

		temp = line.substr( line.find( L"size=" ) + 5, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.length() == 0 )
		{
			throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Font size not found in font file '" + fontFile + L"'" );
		}
		size = stoi( temp );

		temp = line.substr( line.find( L"bold=" ) + 5, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.length() == 0 )
		{
			throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Bold value not found in font file '" + fontFile + L"'" );
		}
		bold = ( stoi( temp ) == 0 ) ? false : true;

		temp = line.substr( line.find( L"italic=" ) + 7, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.length() == 0 )
		{
			throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Italic value not found in font file '" + fontFile + L"'" );
		}
		italic = ( stoi( temp ) == 0 ) ? false : true;

		temp = line.substr( line.find( L"unicode=" ) + 8, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.length() == 0 )
		{
			throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Unicode value not found in font file '" + fontFile + L"'" );
		}
		unicode = (stoi( temp ) == 0) ? false : true;

		temp = line.substr( line.find( L"smooth=" ) + 7, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.length() == 0 )
		{
			throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Smoothing value not found in font file '" + fontFile + L"'" );
		}
		smoothing = (stoi( temp ) == 0) ? false : true;

		temp = line.substr( line.find( L"outline=" ) + 8, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.length() == 0 )
		{
			throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Outline thickness not found in font file '" + fontFile + L"'" );
		}
		outlineThickness = stoi( temp );


		//process line 2
		std::getline( fin, line );

		temp = line.substr( line.find( L"lineHeight=" ) + 11, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.length() == 0 )
		{
			throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Line Height not found in font file '" + fontFile + L"'" );
		}
		cellHeight = stoi( temp );

		temp = line.substr( line.find( L"base=" ) + 5, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.length() == 0 )
		{
			throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Base value not found in font file '" + fontFile + L"'" );
		}
		base = stoi( temp );

		temp = line.substr( line.find( L"scaleW=" ) + 7, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.length() == 0 )
		{
			throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Image width not found in font file '" + fontFile + L"'" );
		}
		imageWidth = stoi( temp );

		temp = line.substr( line.find( L"scaleH=" ) + 7, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.length() == 0 )
		{
			throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Image height not found in font file '" + fontFile + L"'" );
		}
		imageHeight = stoi( temp );

		temp = line.substr( line.find( L"pages=" ) + 6, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L" " ) );
		if( temp.length() == 0 )
		{
			throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Page count not found in font file '" + fontFile + L"'" );
		}
		numPages = stoi( temp );


		//process line 3
		std::getline( fin, line );

		temp = line.substr( line.find( L"file=\"" ) + 6, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L"\"" ) );
		if( temp.length() == 0 )
		{
			throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Font image not found in font file '" + fontFile + L"'" );
		}
		textureFilename = directory + temp;
		textureName = temp.substr( 0, temp.find_last_of( L"." ) ) + L"/Diffuse";

		_kAssetManager.EnqueueAsset( L"Texture", Kiwi::StringPair { { L"Name", textureName }, { L"Path", textureFilename } } );
		KIWIDEBUGSTRING( Kiwi::ToWString( L">>>TEXTURE NAME: " + textureName + L"\n" ).c_str() );

		//process line 4
		std::getline( fin, line );

		temp = line.substr( line.find( L"count=" ) + 6, std::string::npos );
		temp = temp.substr( 0, temp.find_first_of( L"\n" ) );
		if( temp.length() == 0 )
		{
			throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Character count not found in font file '" + fontFile + L"'" );
		}
		charCount = stoi( temp );

		/*amount to multiply the position in the texture with in order
		to get the texture coordinates between 0 and 1*/
		float texU = 1.0f / (float)imageWidth;
		float texV = 1.0f / (float)imageHeight;

		characterSet.resize( 255 );

		//formatting information: http://www.angelcode.com/products/bmfont/doc/render_text.html
		while( std::getline( fin, line ) )
		{
			//skip kerning lines
			if( line[0] == 'k' )
			{
				break;
			}

			int charId = 0;

			temp = line.substr( line.find( L"id=" ) + 3, std::string::npos );
			temp = temp.substr( 0, temp.find_first_of( L" " ) );
			if( temp.length() == 0 )
			{
				throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Character id not found in font file '" + fontFile + L"'" );
			}
			charId = stoi( temp );

			//generate the character by loading the x, y, xOffset, yOffset, xAdvance, width, and height properties of the character
			Kiwi::Font::Character newChar;

			/*x is the x position of the character within the texture*/
			temp = line.substr( line.find( L"x=" ) + 2, std::string::npos );
			temp = temp.substr( 0, temp.find_first_of( L" " ) );
			if( temp.length() == 0 )
			{
				throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Character x pos not found in font file '" + fontFile + L"'" );
			}
			newChar.position.x = stod( temp );

			/*y is the y position of the character within the texture*/
			temp = line.substr( line.find( L"y=" ) + 2, std::string::npos );
			temp = temp.substr( 0, temp.find_first_of( L" " ) );
			if( temp.length() == 0 )
			{
				throw Kiwi::Exception( L"SceneLoader::_LoadFont", L"Character y pos not found in font file '" + fontFile + L"'" );
			}
			newChar.position.y = stod( temp );

			/*height is the vertical size of the character's bounding box*/
			temp = line.substr( line.find( L"height=" ) + 7, std::string::npos );
			temp = temp.substr( 0, temp.find_first_of( L" " ) );
			if( temp.length() == 0 )
			{
				throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Character height not found in font file '" + fontFile + L"'" );
			}
			newChar.dimensions.y = stod( temp );

			/*width is the horizontal size of the character's bounding box*/
			temp = line.substr( line.find( L"width=" ) + 6, std::string::npos );
			temp = temp.substr( 0, temp.find_first_of( L" " ) );
			if( temp.length() == 0 )
			{
				throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Character width not found in font file '" + fontFile + L"'" );
			}
			newChar.dimensions.x = stod( temp );

			/*xAdvance is the amount to move the cursor by to get to the start of the new character once the character has been rendered*/
			temp = line.substr( line.find( L"xadvance=" ) + 9, std::string::npos );
			temp = temp.substr( 0, temp.find_first_of( L" " ) );
			if( temp.length() == 0 )
			{
				throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Character x advance not found in font file '" + fontFile + L"'" );
			}
			newChar.xAdvance = stod( temp );

			/*xoffset is the amount to add to the current cursor position to get the x position of the character*/
			temp = line.substr( line.find( L"xoffset=" ) + 8, std::string::npos );
			temp = temp.substr( 0, temp.find_first_of( L" " ) );
			if( temp.length() == 0 )
			{
				throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Character x offset not found in font file '" + fontFile + L"'" );
			}
			newChar.offset.x = stod( temp );

			/*yoffset is difference between the top of the line and the top of the character*/
			temp = line.substr( line.find( L"yoffset=" ) + 8, std::string::npos );
			temp = temp.substr( 0, temp.find_first_of( L" " ) );
			if( temp.length() == 0 )
			{
				throw Kiwi::Exception( L"FontFactory::_CreateObject", L"Character y offset not found in font file '" + fontFile + L"'" );
			}
			newChar.offset.y = stod( temp );

			/*calculate UV coordinates using the position*/
			newChar.uvs.y = newChar.position.y / (double)imageHeight;
			newChar.uvs.x = newChar.position.x / (double)imageWidth;
			newChar.uvs.z = (newChar.position.x + newChar.dimensions.x) / (double)imageWidth;
			newChar.uvs.w = (newChar.position.y + newChar.dimensions.y) / (double)imageHeight;

			/*resize the chacter array if needed to fit the new character*/
			if( charId >= characterSet.size() )
			{
				characterSet.resize( charId + 1 );
			}

			/*insert the new character*/
			characterSet[charId] = newChar;
		}

		fin.close();

		Kiwi::Font* newFont = new Kiwi::Font( assetName, fontFace, textureName, characterSet, cellHeight );
		if( newFont != nullptr )newFont->AddProperty( L"TextureFile", textureFilename );

		_createdAssets.push_back( std::unique_ptr<Kiwi::Font>( newFont ) );

	}

}