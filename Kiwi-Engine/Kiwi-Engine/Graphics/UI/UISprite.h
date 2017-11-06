#ifndef _KIWI_UISPRITE_H_
#define _KIWI_UISPRITE_H_

#include "IUIWidget.h"

#include "..\Color.h"

#include <string>

namespace Kiwi
{

	class Texture;
	class UICanvas;

	class UISprite :
		public Kiwi::IUIWidget
	{
	protected:

		std::wstring m_textureName;

	protected:

		void _OnStart();
		void _OnShutdown();

	public:

		UISprite( const std::wstring& name, Kiwi::UICanvas& canvas, const std::wstring& textureName = L"" );
		~UISprite();

		void SetTexture( const std::wstring& textureName );

		/*takes a vector containing 6 coordinates, and applies these to each vertex in the sprite's quad*/
		void SetTextureUVs( const std::vector<Kiwi::Vector2d>& uvs );

		void SetColor( const Kiwi::Color& color );

		std::wstring GetTextureName()const { return m_textureName; }

	};
}

#endif