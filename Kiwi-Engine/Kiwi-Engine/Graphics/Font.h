#ifndef _KIWIFONT_H_
#define _KIWIFONT_H_

#include "Mesh.h"

#include "..\Core\Vector3.h"
#include "..\Core\IAsset.h"

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

		enum ALIGNMENT { ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTRE };

		struct Character
		{
			//UV coordinates
			float left, right, top, bottom;
			//size of the character in pixels
			int charWidth, charHeight;
		};

	protected:

		std::vector<Font::Character> m_fontCharacters;

		Kiwi::Texture* m_texture;

		//amount of space placed between each character
		float m_characterSpacing;

		std::wstring m_fontName;

	public:

		Font( std::wstring fontName, Kiwi::Texture* texture, std::vector<Font::Character> characters );
		~Font();

		/*returns the on-screen width of the string*/
		float CalculateWidth( std::wstring text );

		void SetCharacterSpacing( float spacing ) { m_characterSpacing = spacing; }

		/*returns the height of all of the characters (height is constant)*/
		float GetCharacterHeight()const { return (float)m_fontCharacters[0].charHeight; }

		Kiwi::Font::Character GetCharacter( char character ) { return m_fontCharacters[character]; }
		Kiwi::Texture* GetTexture()const { return m_texture; }
		float GetCharacterSpacing()const { return m_characterSpacing; }
		std::wstring GetName()const { return m_fontName; }

	};
};

#endif