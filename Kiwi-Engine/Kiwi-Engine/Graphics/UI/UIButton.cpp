#include "UIButton.h"
#include "UISprite.h"
#include "UICanvas.h"
#include "SpriteAnimator.h"

#include "..\RenderWindow.h"
#include "../Text.h"

#include "..\..\Core\Scene.h"
#include "../../Core/EngineRoot.h"

namespace Kiwi
{

	UIButton::UIButton( std::wstring name, Kiwi::UICanvas& canvas, std::wstring buttonTextureName ) :
		Kiwi::IUIWidget( name, canvas, L"UIButton" )
	{
		m_textureName = buttonTextureName;
		m_buttonSprite = nullptr;
		m_isPressed = false;
		m_mouseOver = false;
	}

	UIButton::UIButton( std::wstring name, Kiwi::UICanvas& canvas, std::wstring label, std::wstring labelFont, std::wstring buttonTextureName ) :
		Kiwi::IUIWidget( name, canvas, L"UIButton" )
	{
		m_textureName = buttonTextureName;
		m_buttonSprite = nullptr;
		m_labelFont = labelFont;
		m_label = label;
		m_isPressed = false;
		m_mouseOver = false;
	}

	UIButton::~UIButton()
	{
	}

	void UIButton::_OnStart()
	{
		_kInput.ConnectInputEventReceiver( *this, L"MouseLeftPressed", &UIButton::_OnMousePress );
		_kInput.ConnectInputEventReceiver( *this, L"MouseLeftReleased", &UIButton::_OnMouseReleased );
		_kInput.ConnectInputEventReceiver( *this, L"MouseMove", &UIButton::_OnMouseMove );

		m_buttonSprite = m_widgetCanvas->CreateSprite( m_objectName + L"/Sprite", m_textureName );
		if( m_buttonSprite != nullptr )
		{
			m_buttonSprite->SetWidgetDimensions( m_widgetDimensions );
			this->AttachChild( m_buttonSprite );

			/*Kiwi::SpriteAnimator* anim = m_buttonSprite->FindComponent<Kiwi::SpriteAnimator>( m_buttonSprite->GetName() + L"/SpriteAnimator" );
			if( anim != nullptr )
			{
				std::vector<Kiwi::Vector2d> upUVs = { Kiwi::Vector2d( 0.0, 0.0 ), Kiwi::Vector2d( 0.4296875, 0.0 ), Kiwi::Vector2d( 0.0, 0.9375 ), Kiwi::Vector2d( 0.0, 0.9375 ), Kiwi::Vector2d( 0.4296875, 0.0 ), Kiwi::Vector2d( 0.4296875, 0.9375 ) };
				anim->AddAnimation( L"ButtonUp", 1, { upUVs } );

				std::vector<Kiwi::Vector2d> downUVs = { Kiwi::Vector2d( 0.4296875, 0.0 ), Kiwi::Vector2d( 0.859375, 0.0 ), Kiwi::Vector2d( 0.4296875, 0.9375 ), Kiwi::Vector2d( 0.4296875, 0.9375 ), Kiwi::Vector2d( 0.859375, 0.0 ), Kiwi::Vector2d( 0.859375, 0.9375 ) };
				anim->AddAnimation( L"ButtonDown", 1, { downUVs } );

				anim->Play( L"ButtonUp" );
			}*/
		}

		if( m_label.length() > 0 )
		{
			/*Kiwi::Entity* labelEntity = scene->CreateEntity( m_objectName + L"/label" );
			if( labelEntity != 0 )
			{
				m_label = new Kiwi::Text( m_objectName + L"/label", m_labelFont, m_dimensions );
				if( m_label != 0 )
				{
					m_label->SetAlignment( Kiwi::Font::ALIGN_CENTRE );
					m_label->SetText( m_labelText );
					labelEntity->FindComponent<Kiwi::Transform>()->SetLocalPosition( Kiwi::Vector3( 0.0, 0.0, m_entity->FindComponent<Kiwi::Transform>()->GetLocalPosition().z + 20.0 ) );
					labelEntity->AttachComponent( m_label );
					m_entity->AttachChild( labelEntity );
				}
			}*/
		}

		m_widgetCanvas->_NotifyWidgetResized( *this );
		m_widgetCanvas->_NotifyWidgetMoved( *this );
	}

	void UIButton::_OnShutdown()
	{
	}

	void UIButton::_OnUpdate()
	{
		if( m_isPressed ) this->_OnMouseHeld();

		this->_EmitUIButtonEventQueue();
	}

	void UIButton::_OnWidgetResized( const Kiwi::Vector2d& resizeAmount )
	{
		m_holdRegion = m_widgetDimensions + Kiwi::Vector2d( 30.0, 30.0 );

		if( m_buttonSprite != nullptr ) m_buttonSprite->SetWidgetDimensions( m_widgetDimensions );
	}

	Kiwi::EventResponse UIButton::_OnMousePress( std::shared_ptr<Kiwi::InputEvent> evt )
	{
		Kiwi::MouseEvent* mEvent = dynamic_cast<Kiwi::MouseEvent*>(evt.get());
		if( mEvent == nullptr ) Kiwi::EventResponse::NONE;

		m_isPressed = false;

		Kiwi::Vector2d halfDimensions( m_widgetDimensions.x / 2.0, m_widgetDimensions.y / 2.0 );
		Kiwi::Vector3d canvasPosition = m_widgetCanvas->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform )->GetGlobalPosition();

		/*if( mEvent->pos.x > m_widgetPosition.x + canvasPosition.x - halfDimensions.x && mEvent->pos.x < m_widgetPosition.x + canvasPosition.x + halfDimensions.x &&
			mEvent->pos.y > m_widgetPosition.y + canvasPosition.y - halfDimensions.y && mEvent->pos.y < m_widgetPosition.y + canvasPosition.y + halfDimensions.y )
		{
			m_isPressed = true;
			
			Kiwi::SpriteAnimator* anim = m_buttonSprite->FindComponent<Kiwi::SpriteAnimator>( m_objectName + L"/Sprite/SpriteAnimator" );
			if( anim != nullptr )
			{
				anim->Play( L"ButtonDown" );
			}

			this->EnqueueUIButtonEvent( std::make_shared<Kiwi::UIButtonEvent>( *this, L"ButtonPressed" ) );
		}*/

		return Kiwi::EventResponse::NONE;
	}

	void UIButton::_OnMouseHeld()
	{
		//KIWIDEBUGSTRING( L"BUTTON HELD" );
		this->EnqueueUIButtonEvent( std::make_shared<Kiwi::UIButtonEvent>( *this, L"ButtonHeld" ) );
	}

	Kiwi::EventResponse UIButton::_OnMouseReleased( std::shared_ptr<Kiwi::InputEvent> evt )
	{
		if( m_isPressed )
		{
			Kiwi::MouseEvent* mEvent = dynamic_cast<Kiwi::MouseEvent*>(evt.get());
			if( mEvent == nullptr ) Kiwi::EventResponse::NONE;

			m_isPressed = false;
			
			/*Kiwi::SpriteAnimator* anim = m_buttonSprite->FindComponent<Kiwi::SpriteAnimator>( m_buttonSprite->GetName() + L"/SpriteAnimator" );
			if( anim != nullptr )
			{
				anim->Play( L"ButtonUp" );
			}*/
			this->EnqueueUIButtonEvent( std::make_shared<Kiwi::UIButtonEvent>( *this, L"ButtonReleased" ) );
		}

		return Kiwi::EventResponse::NONE;
	}

	Kiwi::EventResponse UIButton::_OnMouseMove( std::shared_ptr<Kiwi::InputEvent> evt )
	{
		Kiwi::MouseEvent* mEvent = dynamic_cast<Kiwi::MouseEvent*>(evt.get());
		if( mEvent == nullptr ) Kiwi::EventResponse::NONE;

		if( m_isPressed )
		{
			Kiwi::Vector2d halfDimensions( m_holdRegion.x / 2.0, m_holdRegion.y / 2.0 );
			Kiwi::Vector3d canvasPosition = m_widgetCanvas->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform )->GetGlobalPosition();

			if( mEvent->pos.x > m_widgetPosition.x + canvasPosition.x - halfDimensions.x && mEvent->pos.x < m_widgetPosition.x + canvasPosition.x + halfDimensions.x &&
				mEvent->pos.y > m_widgetPosition.y + canvasPosition.y - halfDimensions.y && mEvent->pos.y < m_widgetPosition.y + canvasPosition.y + halfDimensions.y )
			{
				return Kiwi::EventResponse::NONE;

			} else
			{
				/*release the button, but don't trigger an event because it wasn't actually released*/
				/*m_isPressed = false;
				Kiwi::SpriteAnimator* anim = m_buttonSprite->FindComponent<Kiwi::SpriteAnimator>( m_buttonSprite->GetName() + L"/SpriteAnimator" );
				if( anim != nullptr )
				{
					anim->Play( L"ButtonUp" );
				}*/
			}
		}

		return Kiwi::EventResponse::NONE;
	}

	void UIButton::SetTexture( std::wstring textureName )
	{
		if( m_buttonSprite )
		{
			m_buttonSprite->SetTexture( textureName );
		}
	}

}