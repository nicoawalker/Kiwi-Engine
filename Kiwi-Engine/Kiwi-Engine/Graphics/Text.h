#ifndef _KIWI_TEXT_H_
#define _KIWI_TEXT_H_

#include "Font.h"
#include "Color.h"

#include "..\Core\Entity.h"

namespace Kiwi
{

	class Text:
		public Kiwi::Entity
	{
	protected:

		//the font used for rendering text
		Kiwi::Font* m_font;

		Kiwi::Color m_textColor;

		//maximum number of characters
		unsigned int m_maxCharacters;

		std::wstring m_text;

		//stores the current height and width of the text
		Kiwi::Vector2 m_dimensions;

	protected:

		void _GenerateTextMesh( std::wstring text, Font::ALIGNMENT textAlignment = Font::ALIGN_LEFT );

	public:

		Text( std::wstring name, Kiwi::Scene* scene, std::wstring font, unsigned int maxCharacters = 32 );
		virtual ~Text();

		virtual void Update() {}

		virtual void FixedUpdate() {}

		virtual void SetText( std::wstring text, Font::ALIGNMENT textAlignment = Font::ALIGN_LEFT );

		virtual void SetMaxCharacters( unsigned int maxChars ) { m_maxCharacters = maxChars; }

		virtual void SetColor( const Kiwi::Color& color );

		virtual Kiwi::Vector2 GetDimensions()const { return m_dimensions; }

		virtual std::wstring GetText()const { return m_text; }

		virtual unsigned int GetMaxCharacters()const { return m_maxCharacters; }

		virtual Kiwi::Color GetColor()const { return m_textColor; }
	};
}

#endif