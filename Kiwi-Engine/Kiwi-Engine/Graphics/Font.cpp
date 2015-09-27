#include "Font.h"
#include "Texture.h"
#include "Mesh.h"
#include "Material.h"

#include "..\Core\Utilities.h"
#include "..\Core\Logger.h"
#include "..\Core\Transform.h"
#include "..\Core\Exception.h"

namespace Kiwi
{

	Font::Font( std::wstring fontName, Kiwi::Texture* texture, std::vector<Font::Character> characters ) :
		Kiwi::IAsset( fontName, L"Font" )
	{

		if( texture == 0 )
		{
			throw Kiwi::Exception( L"Font", L"[" + m_fontName + L"] Font was provided with null texture" );
		}
		if( characters.size() == 0 )
		{
			throw Kiwi::Exception( L"Font", L"[" + m_fontName + L"] Font was provided with empty character set" );
		}

		m_fontName = fontName;
		m_texture = texture;
		m_fontCharacters = characters;
		m_characterSpacing = 0.0f;

	}

	Font::~Font()
	{

		Kiwi::FreeMemory( m_fontCharacters );

	}

	float Font::CalculateWidth( std::wstring text )
	{

		float width = 0.0f;
		for( unsigned int i = 0; i < text.size(); i++ )
		{
			width += (float)m_fontCharacters[text[i]].charWidth;
			width += m_characterSpacing;
		}

		return width;

	}

};