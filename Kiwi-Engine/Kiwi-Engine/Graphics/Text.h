#ifndef _KIWI_TEXT_H_
#define _KIWI_TEXT_H_

#include "Font.h"
#include "Color.h"

#include "../Types.h"

#include "..\Core\Entity.h"
#include "../Core/Component.h"
#include "..\Core\IAssetConsumer.h"

namespace Kiwi
{

	class Scene;

	class Text :
		public Kiwi::Component,
		public Kiwi::IAssetConsumer
	{
	protected:

		//the font used for rendering text
		Kiwi::Font* m_font;
		Kiwi::Mesh* m_textMesh;
		Kiwi::Texture* m_fontTexture;

		Kiwi::Vector2d m_dimensions;
		Kiwi::Vector2d m_maxDimensions;

		TEXT_ALIGNMENT m_textAlignment;

		Kiwi::Color m_textColor;

		//maximum number of characters
		int m_maxCharacters;

		std::wstring m_text;
		std::wstring m_fontName;
		std::wstring m_shaderName;

	protected:

		void _OnStart();
		void _OnShutdown();

		bool _InitializeText();

		void _GenerateTextMesh( std::wstring text );

		Kiwi::EventResponse _OnAssetFetched( Kiwi::IAsset* asset );

	public:

		Text( std::wstring name, Kiwi::Scene& scene, std::wstring fontName, const Kiwi::Vector2d& maxDimensions );
		virtual ~Text();

		virtual void SetText( std::wstring text );

		void SetAlignment( TEXT_ALIGNMENT alignment );

		virtual void SetMaxCharacters( unsigned int maxChars ) { m_maxCharacters = maxChars; }

		virtual void SetColor( const Kiwi::Color& color );

		void SetShader( std::wstring shaderName );

		virtual Kiwi::Vector2d GetDimensions()const { return m_dimensions; }

		virtual std::wstring GetText()const { return m_text; }

		virtual unsigned int GetMaxCharacters()const { return m_maxCharacters; }

		virtual Kiwi::Color GetColor()const { return m_textColor; }

		double GetCharacterHeight()const;
	};
}

#endif