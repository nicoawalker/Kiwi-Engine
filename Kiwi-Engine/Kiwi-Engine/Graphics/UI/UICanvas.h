#ifndef _KIWI_UICANVAS_H_
#define _KIWI_UICANVAS_H_

#include "..\Color.h"

#include "../../Core/Entity.h"

#include "..\..\Events\EventHandling.h"

#include <string>
#include <vector>

namespace Kiwi
{

	class Scene;
	class RenderTarget;
	class Camera;
	class IUIWidget;
	class UISprite;
	class UIButton;
	class UITextbox;

	class UICanvas : 
		public Kiwi::Entity,
		public Kiwi::ISceneEventReceiver,
		public Kiwi::ITransformEventReceiver
	{
		friend class IUIWidget;
		friend class UIButton;
		friend class UISprite;
		friend class UITextbox;
		friend class UIScrollBar;
	protected:

		Kiwi::RenderTarget* m_canvasRenderTarget;

		Kiwi::Camera* m_camera;

		/*dimensions of the canvas in world space*/
		Kiwi::Vector2d m_canvasDimensions;

		/*how much to scale each widget by to account for the difference between the canvas dimensions and the back buffer dimensions
		equal to backbuffer dimensions / canvasDimensions*/
		Kiwi::Vector2d m_canvasWidgetScale;

		/*clear color used to clear the render target*/
		Kiwi::Color m_canvasBgColor;

		std::wstring m_widgetShader; //if set, this will be the shader used by every UIWidget attached to this canvas

		std::vector<Kiwi::IUIWidget*> m_widgets;

		bool m_redrawWidgets;

	protected:

		/*creates and attaches a mesh to the canvas*/
		bool _CreateMesh();

		/*notifies the canvas that a widget has been resized, and that it must now re-size it and then re-draw all of the widgets to the render target*/
		void _NotifyWidgetResized( Kiwi::IUIWidget& widget );

		/*notifies the canvas that a widget has been moved, and that it must now move it and then re-draw all of the widgets to the render target*/
		void _NotifyWidgetMoved( Kiwi::IUIWidget& widget );

		/*notifies the canvas that the widget has changed in some other way, and that it should now re-draw it*/
		void _NotifyWidgetChanged( Kiwi::IUIWidget& widget ) { m_redrawWidgets = true; }

		/*renders all of the attached widgets to the render target if m_redrawWidgets is true*/
		Kiwi::EventResponse _OnPreRender( std::shared_ptr<Kiwi::SceneEvent> evt );

		void _OnStart();
		void _OnShutdown();

		Kiwi::EventResponse _OnTranslate( std::shared_ptr<Kiwi::TransformEvent> evt );

		void _OnUpdate();

	public:

		UICanvas( Kiwi::Scene& scene, std::wstring canvasName, const Kiwi::Vector2d& dimensions, Kiwi::Camera* camera = 0 );
		virtual ~UICanvas();

		Kiwi::UISprite* CreateSprite( const std::wstring& spriteName, const std::wstring& spriteTexture );
		Kiwi::UIButton* CreateButton( const std::wstring& buttonName, const std::wstring& buttonTexture );
		Kiwi::UITextbox* CreateTextbox( const std::wstring& textboxName, const std::wstring& font, const std::wstring& textboxTexture = L"", const std::wstring& initialText = L"" );
		Kiwi::UIScrollBar* CreateScrollBar( const std::wstring& scrollbarName, const std::wstring& buttonTexture, const std::wstring& sliderTexture, const std::wstring& backgroundTexture );

		void SetCamera( Kiwi::Camera* camera );

		/*sets the background color of the canvas*/
		void SetBackgroundColor( const Kiwi::Color& bgColor );

		void SetWidgetShader( const std::wstring& shaderName ) { m_widgetShader = shaderName; }

		void UpdateWidgets() { m_redrawWidgets = true; }

		Kiwi::Scene* GetScene()const { return m_scene; }
		Kiwi::Camera* GetCamera()const { return m_camera; }
		const Kiwi::Vector2d& GetWidgetScale()const { return m_canvasWidgetScale; }
		Kiwi::Vector2d GetWidgetScreenPosition( Kiwi::IUIWidget& widget );
		std::wstring GetWidgetShader()const { return m_widgetShader; }
		Kiwi::RenderTarget* GetRenderTarget()const { return m_canvasRenderTarget; }

	};

}

#endif