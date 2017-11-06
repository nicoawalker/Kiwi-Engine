#ifndef _KIWIFONT_H_
#define _KIWIFONT_H_

#include "..\Core\Vector3.h"
#include "../Core/IAsset.h"
#include "..\Core\Vector2d.h"
#include "../Core/Vector4d.h"

#include <string>
#include <vector>

namespace Kiwi
{

	class Material;
	class Renderer;
	class Mesh;
	class Transform;
	class Texture;

	class Font:
		public Kiwi::IAsset
	{
	public:

		struct Character
		{
			//UV coordinates
			Kiwi::Vector2d position; //position of the top left corner of the character's bounding box within the font image
			Kiwi::Vector2d dimensions; //size of the character's quad in pixels
			Kiwi::Vector4d uvs; //x, y = top left corner, z, w = bottom right corner
			Kiwi::Vector2d offset; //y = distance from the top of the line to the top of the character, x = amount the character should be shifted to the right
			double xAdvance; //amount to move the cursor by to get to the starting position of the next character

			Character()
			{
				xAdvance = 0.0;
			}
		};

	protected:

		std::vector<Font::Character> m_fontCharacters;

		//amount of space placed between each character
		double m_characterSpacing;
		uint m_lineHeight; //height of each line
		double m_charBase; //how far from the top of the line the bottom of each character should be placed

		std::wstring m_fontName;
		std::wstring m_fontFace;
		std::wstring m_fontTexture;

	public:

		Font( const Kiwi::Font& );
		Font( const std::wstring& fontName, const std::wstring& fontFace, const std::wstring& fontTexture, std::vector<Font::Character> characters, uint lineHeight );
		~Font();

		/*returns the on-screen width of the string*/
		double CalculateWidth( std::wstring text );

		/*creates a copy of the font and returns control to the caller*/
		virtual std::unique_ptr<Kiwi::IAsset> Clone();

		void SetCharacterSpacing( double spacing ) { m_characterSpacing = spacing; }

		//float GetCharacterWidth( wchar_t character );

		/*returns the height of all of the characters (height is constant)*/
		//float GetCharacterHeight()const { return (float)m_fontCharacters[0].charHeight; }

		uint GetLineHeight()const { return m_lineHeight; }

		Kiwi::Font::Character GetCharacter( char character ) { return m_fontCharacters[character]; }
		std::wstring GetTexture()const { return m_fontTexture; }
		double GetCharacterSpacing()const { return m_characterSpacing; }
		std::wstring GetName()const { return m_fontName; }

	};
};

#endif