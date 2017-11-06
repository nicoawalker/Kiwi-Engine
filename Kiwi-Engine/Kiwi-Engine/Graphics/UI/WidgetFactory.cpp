#include "WidgetFactory.h"

#include "UICanvas.h"
#include "UISprite.h"
#include "UITextbox.h"
#include "UIButton.h"
#include "UIScrollBar.h"

#include "../../Core/Scene.h"

#include <memory>

namespace Kiwi
{

	std::wstring WidgetFactory::ObjectType = L"UIWidget";

	std::unique_ptr<Kiwi::GameObject> WidgetFactory::_CreateObject( const std::wstring& name, Kiwi::Scene& scene, const Kiwi::StringPair& parameters )
	{		
		Kiwi::UICanvas* canvas = nullptr;

		auto paramItr = parameters.find( L"UICanvas" );
		if( paramItr != parameters.end() )
		{
			canvas = scene.FindEntity<Kiwi::UICanvas>( paramItr->second );
			if( canvas == nullptr ) return std::unique_ptr<Kiwi::UISprite>( nullptr );
		}

		paramItr = parameters.find( L"WidgetType" );
		if( paramItr != parameters.end() )
		{
			if( paramItr->second.compare( L"UISprite" ) == 0 )
			{
				std::wstring texture;

				auto itr = parameters.find( L"Texture0" );
				if( itr != parameters.end() )
				{
					texture = itr->second;
				}

				return std::make_unique<Kiwi::UISprite>( name, *canvas, texture );

			} else if( paramItr->second.compare( L"UIButton" ) == 0 )
			{
				std::wstring buttonSpriteTexture;

				auto itr = parameters.find( L"Texture0" );
				if( itr != parameters.end() )
				{
					buttonSpriteTexture = itr->second;
				}

				itr = parameters.find( L"Label" );
				if( itr == parameters.end() )
				{/*make button without any label/text*/
					return std::make_unique<Kiwi::UIButton>( name, *canvas, buttonSpriteTexture );

				} else
				{/*make button with label/text*/

				}

			} else if( paramItr->second.compare( L"UITextbox" ) == 0 )
			{
				std::wstring textboxTexture;
				std::wstring font;
				std::wstring initialText;

				auto itr = parameters.find( L"Texture0" );
				if( itr != parameters.end() )
				{
					textboxTexture = itr->second;
				}

				itr = parameters.find( L"Font" );
				if( itr != parameters.end() )
				{
					font = itr->second;
				}

				itr = parameters.find( L"Text" );
				if( itr != parameters.end() )
				{
					initialText = itr->second;
				}

				return std::make_unique<Kiwi::UITextbox>( name, *canvas, font, textboxTexture, initialText );

			} else if( paramItr->second.compare( L"UIScrollBar" ) == 0 )
			{
				std::wstring texture0, texture1, texture2;

				auto itr = parameters.find( L"Texture0" ); //button texture
				if( itr != parameters.end() )
				{
					texture0 = itr->second;
				}

				itr = parameters.find( L"Texture1" ); //slider texture
				if( itr != parameters.end() )
				{
					texture1 = itr->second;
				}

				itr = parameters.find( L"Texture2" ); //background texture
				if( itr != parameters.end() )
				{
					texture2 = itr->second;
				}

				return std::make_unique<Kiwi::UIScrollBar>( name, *canvas, texture0, texture1, texture2 );
			}
		}

		return std::unique_ptr<Kiwi::UISprite>( nullptr );
	}

}