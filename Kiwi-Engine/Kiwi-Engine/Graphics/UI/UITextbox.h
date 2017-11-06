#ifndef _KIWI_UITEXTBOX_H_
#define _KIWI_UITEXTBOX_H_

#include "IUIWidget.h"

#include "../Color.h"

#include "..\..\Types.h"

#include "../../Events/EventHandling.h"

#include "..\..\Core\Vector2d.h"
#include "../../Core/Vector4d.h"

#include <string>

namespace Kiwi
{

	class UICanvas;
	class RenderTarget;
	class Font;
	class Texture;

	class UITextbox:
		public Kiwi::IUIWidget
	{
	protected:

		Kiwi::Font* m_font;

		/*current dimensions of the text mesh*/
		Kiwi::Vector2d m_textDimensions;

		/*size of the bounding text box*/
		Kiwi::Vector2d m_textboxDimensions;
		Kiwi::Vector4d m_textboxMargins;

		TEXT_ALIGNMENT m_textAlignment;

		Kiwi::Color m_textColor;

		//maximum number of characters
		int m_maxCharacters;

		std::wstring m_text;
		std::wstring m_fontName;
		std::wstring m_textboxTextureName;

		double m_textSharpening;
		double m_lineSpacing;

		bool m_rebuildTextMesh;
		bool m_wordWrapEnabled;

	protected:

		Kiwi::Mesh* _CreateMesh();

		void _OnShutdown();
		void _OnStart();
		void _OnUpdate();

		void _OnWidgetResized( const Kiwi::Vector2d& resizeAmount );
		void _OnWidgetMoved( const Kiwi::Vector3d& translation );

		Kiwi::EventResponse _OnAssetFetched( Kiwi::IAsset* asset );

		void _GenerateTextMesh();

	public:

		UITextbox( const std::wstring& name, Kiwi::UICanvas& canvas, const std::wstring& fontName, const std::wstring& textBoxTexture, const std::wstring& text );
		~UITextbox(){}

		std::wstring GetText()const { return m_text; }

		double GetTextSharpening()const { return m_textSharpening; }

		void SetLineSpacing( double lineSpacing );

		void SetText( const std::wstring& newText );

		void AppendText( const std::wstring& newText );

		void SetTextboxMargins( const Kiwi::Vector4d& margins );

		void SetTextColor( const Kiwi::Color& color );

		/*any pixel with an alpha value below this value will be clipped when rendering*/
		void SetTextSharpening( double sharpening ) { m_textSharpening = sharpening; }

	};
}

#endif