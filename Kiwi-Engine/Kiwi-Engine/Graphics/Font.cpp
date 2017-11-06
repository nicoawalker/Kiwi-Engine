#include "Font.h"
#include "Texture.h"
#include "Mesh.h"
#include "Material.h"

#include "..\Core\Utilities.h"
#include "../Core/Transform.h"
#include "..\Core\Exception.h"

namespace Kiwi
{

	Font::Font( const Kiwi::Font& other ) :
		Kiwi::IAsset( other.GetName(), L"Font" )
	{
		m_fontName = other.m_fontName;
		m_fontTexture = other.m_fontTexture;
		m_fontCharacters = other.m_fontCharacters;
		m_characterSpacing = other.m_characterSpacing;

		m_assetLoadState = other.m_assetLoadState;
		m_assetCategory = other.m_assetCategory;
		m_assetSubcategory = other.m_assetSubcategory;
		m_assetProperties = other.m_assetProperties;
	}

	Font::Font( const std::wstring& fontName, const std::wstring& fontFace, const std::wstring& fontTexture, std::vector<Font::Character> characters, uint lineHeight ) :
		Kiwi::IAsset( fontName, L"Font" )
	{
		if( characters.size() == 0 )
		{
			throw Kiwi::Exception( L"Font", L"[" + m_fontName + L"] Font was provided with empty character set" );
		}

		m_fontName = fontName;
		m_fontFace = fontFace;
		m_fontTexture = fontTexture;
		m_fontCharacters = characters;
		m_characterSpacing = 0.0;
		m_lineHeight = lineHeight;
	}

	Font::~Font()
	{
		Kiwi::FreeMemory( m_fontCharacters );
	}

	double Font::CalculateWidth( std::wstring text )
	{
		double width = 0.0;
		for( unsigned int i = 0; i < text.size(); i++ )
		{
			uint charIndex = (uint)text[i];
			if( charIndex < m_fontCharacters.size() )
			{
				if( i == text.size() - 1 )
				{/*last character may stick out a bit further so take that into account here*/
					width += m_fontCharacters[charIndex].offset.x + m_fontCharacters[charIndex].dimensions.x;
					width += m_characterSpacing;

				} else
				{
					width += m_fontCharacters[charIndex].offset.x + m_fontCharacters[charIndex].xAdvance;
					width += m_characterSpacing;
				}
			}
		}

		return width;
	}

	std::unique_ptr<Kiwi::IAsset> Font::Clone()
	{
		return std::make_unique<Kiwi::Font>( *this );
	}

	/*float Font::GetCharacterWidth( wchar_t character )
	{

		if( character < m_fontCharacters.size() )
		{
			return (float)m_fontCharacters[character].charWidth;
		}

		return 0.0f;

	}*/

};