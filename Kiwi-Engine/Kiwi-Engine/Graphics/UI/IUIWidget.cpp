#include "IUIWidget.h"
#include "UICanvas.h"

#include "..\..\Core\Scene.h"

namespace Kiwi
{

	IUIWidget::IUIWidget( std::wstring name, UICanvas& canvas, std::wstring widgetType ) :
		Kiwi::Entity( name, *canvas.GetScene() )
	{

		m_widgetType = widgetType;
		m_widgetCanvas = &canvas;
		m_canvasWidgetScale.Set( 1.0, 1.0 );
		m_widgetDimensions.Set( 1.0, 1.0 );

		m_entityType = ENTITY_2D;
	}

	IUIWidget::~IUIWidget()
	{

	}

	/*void IUIWidget::_ApplyCanvasScaling()
	{
		Kiwi::Vector2d scale = Kiwi::Vector2d( m_widgetCanvas->GetWidgetScale().x * m_widgetCanvas->GetUserWidgetScale().x, m_widgetCanvas->GetWidgetScale().y * m_widgetCanvas->GetUserWidgetScale().y );
		if( scale != m_canvasWidgetScale )
		{
			if( m_transform != nullptr ) m_transform->SetScale( Kiwi::Vector2d( m_widgetDimensions.x * scale.x, m_widgetDimensions.y * scale.y ) );
			m_canvasWidgetScale = scale;
		}
	}*/

	void IUIWidget::SetWidgetDimensions( const Kiwi::Vector2d& dimensions )
	{
		Kiwi::Vector2d sizeDifference = dimensions - m_widgetDimensions;
		m_widgetDimensions = dimensions;

		this->_OnWidgetResized( sizeDifference );
		m_widgetCanvas->_NotifyWidgetResized( *this );
		//if( m_transform != nullptr ) m_transform->SetScale( Kiwi::Vector2d( dimensions.x * m_canvasWidgetScale.x, dimensions.y * m_canvasWidgetScale.y ) );
	}

	void IUIWidget::SetWidgetPosition( const Kiwi::Vector2d& position )
	{
		Kiwi::Vector3d translation = Kiwi::Vector3d( position.x, position.y, m_widgetPosition.z ) - m_widgetPosition;
		m_widgetPosition.Set( position.x, position.y, m_widgetPosition.z );

		this->_OnWidgetMoved( translation );
		m_widgetCanvas->_NotifyWidgetMoved( *this );
	}

	/*sets the widget's x and y dimensions as well as the widget depth*/
	void IUIWidget::SetWidgetPosition( const Kiwi::Vector3d& position )
	{
		Kiwi::Vector3d translation = position - m_widgetPosition;
		m_widgetPosition = position;

		this->_OnWidgetMoved( translation );
		m_widgetCanvas->_NotifyWidgetMoved( *this );
	}

	void IUIWidget::SetWidgetDepth( double depth )
	{
		Kiwi::Vector3d translation = Kiwi::Vector3d( m_widgetPosition.x, m_widgetPosition.y, depth ) - m_widgetPosition;
		m_widgetPosition.z = depth;

		this->_OnWidgetMoved( translation );
		m_widgetCanvas->_NotifyWidgetMoved( *this );
		//if( m_transform != nullptr ) m_transform->SetGlobalPosition( m_transform->GetGlobalPosition().x, m_transform->GetGlobalPosition().y, depth );
	}

}