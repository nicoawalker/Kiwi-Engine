#ifndef _KIWI_IUIWIDGET_H_
#define _KIWI_IUIWIDGET_H_

#include "../../Core/Vector2d.h"
#include "..\..\Core\Entity.h"

#include "../../Core/IAssetConsumer.h"

namespace Kiwi
{

	class Scene;
	class UICanvas;

	class IUIWidget : 
		public Kiwi::Entity,
		public Kiwi::IAssetConsumer
	{
		friend class UICanvas;
	protected:

		Kiwi::UICanvas* m_widgetCanvas;

		std::wstring m_widgetType;

		Kiwi::Vector2d m_widgetDimensions;
		Kiwi::Vector3d m_widgetPosition;

		Kiwi::Vector2d m_canvasWidgetScale;

	protected:

		virtual void _OnWidgetResized( const Kiwi::Vector2d& resizeAmount ) {} //called whenever the widget's scale changes
		virtual void _OnWidgetMoved( const Kiwi::Vector3d& translation ) {} //called whenever the widget is moved

		Kiwi::EventResponse _OnAssetFetched( Kiwi::IAsset* asset ) { return Kiwi::EventResponse::NONE; }

	public:

		IUIWidget( std::wstring name, UICanvas& canvas, std::wstring widgetType );
		virtual ~IUIWidget() = 0;

		/*sets the size of the widget, in pixels*/
		virtual void SetWidgetDimensions( const Kiwi::Vector2d& dimensions );

		/*sets the widget's x and y dimensions, leaving the depth unchanged*/
		virtual void SetWidgetPosition( const Kiwi::Vector2d& position );

		/*sets the widget's x and y dimensions as well as the widget depth*/
		virtual void SetWidgetPosition( const Kiwi::Vector3d& position );

		/*sets the widget's depth*/
		virtual void SetWidgetDepth( double depth );

		std::wstring GetWidgetType()const { return m_widgetType; }

		/*returns the size of the widget, in pixels*/
		Kiwi::Vector2d& GetWidgetDimensions() { return m_widgetDimensions; }
		Kiwi::Vector3d& GetWidgetPosition() { return m_widgetPosition; }

		Kiwi::UICanvas* GetWidgetCanvas()const { return m_widgetCanvas; }

	};
}

#endif