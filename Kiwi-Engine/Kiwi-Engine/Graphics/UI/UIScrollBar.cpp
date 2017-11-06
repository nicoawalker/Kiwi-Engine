#include "UIScrollBar.h"
#include "UIButton.h"
#include "UISprite.h"
#include "UICanvas.h"

#include "../Texture.h"

#include "..\..\Core\Scene.h"
#include "../../Core/EngineRoot.h"
#include "..\..\Core\InputManager.h"

namespace Kiwi
{

	UIScrollBar::UIScrollBar( const std::wstring& name, Kiwi::UICanvas& canvas, const std::wstring& buttonTexture, const std::wstring& sliderTexture, const std::wstring& backgroundTexture ) :
		Kiwi::IUIWidget( name, canvas, L"UIScrollBar" )
	{
		m_button1 = m_button2 = m_slider = nullptr;
		m_backgroundSprite = nullptr;

		m_buttonTexture = buttonTexture;
		m_sliderTexture = sliderTexture;
		m_backgroundTexture = backgroundTexture;

		m_minSliderHeight = 10;
		m_currentScrollStep = 1;
		m_totalScrollSteps = 1;
		m_scrollFrequency = 500.0;
		m_buttonHoldDelay = 1.0;
		m_scrollTimer = 0.0;
		m_sliderHeight = 10;
		m_totalScrollDistance = 0; //total distance the slider can travel
		m_scrollPercentage = 100.0; //percentage of the way to the bottom the slider currently is
		m_scrollStepY = 0.0; //change in position of scroll bar for each line
		m_sliderYMax = 0.0; //stores the position of the slider if it is at the top of the scroll bar
		m_sliderYMin = 0.0;
		m_sliderYPos = 0.0; //stores the current slider y position
		m_currentScrollDirection = 0;
		m_sliderPressed = false;
	}

	UIScrollBar::~UIScrollBar()
	{
	}

	void UIScrollBar::_AdjustSliderSettings()
	{
		//total scroll distance is the distance between the edges of the two buttons
		m_totalScrollDistance = (int)(m_widgetDimensions.y - (m_button1Dimensions.y + m_button2Dimensions.y));

		//slider height is the larger between the min size and the scroll distance / number of steps
		m_sliderHeight = (int)max( m_minSliderHeight, m_totalScrollDistance / (double)m_totalScrollSteps );

		//y min and max are the positions of the inner edges of the two buttons
		m_sliderYMin = m_button2Position.y + (m_button2Dimensions.y / 2.0);
		m_sliderYMax = m_button1Position.y - (m_button1Dimensions.y / 2.0);

		//amount to move the slider each time a button is pressed
		m_scrollStepY = m_totalScrollDistance / (double)m_totalScrollSteps;

		m_sliderYPos = m_sliderYMax - ((double)m_currentScrollStep * m_scrollStepY);
		
		/*update the slider y position and height based on the above settings*/
		if( m_slider != nullptr )
		{
			m_slider->SetWidgetPosition( Kiwi::Vector2d( m_slider->GetWidgetPosition().x, m_sliderYPos ) );
			m_slider->SetWidgetDimensions( Kiwi::Vector2d( m_slider->GetWidgetDimensions().x, (double)m_sliderHeight ) );
		}
	}

	Kiwi::EventResponse UIScrollBar::_OnSliderPress( std::shared_ptr<Kiwi::UIButtonEvent> evt )
	{
		m_sliderPressed = true;

		return Kiwi::EventResponse::NONE;
	}

	Kiwi::EventResponse UIScrollBar::_OnSliderRelease( std::shared_ptr<Kiwi::UIButtonEvent> evt )
	{
		m_sliderPressed = false;

		return Kiwi::EventResponse::NONE;
	}

	void UIScrollBar::_OnStart()
	{
		//m_button1Position = Kiwi::Vector2d( 0.0, (m_widgetDimensions.y / 2.0) - (m_button1Dimensions.y / 2.0) );
		//m_button2Position = Kiwi::Vector2d( 0.0, (m_widgetDimensions.y / -2.0) + (m_button2Dimensions.y / 2.0) );

		m_button1Dimensions.Set( Kiwi::Vector2d( 50.0, 50.0 ) );

		m_button1 = m_widgetCanvas->CreateButton( m_objectName + L"/Button1", m_buttonTexture );
		if( m_button1 != nullptr )          
		{
			KIWIDEBUGSTRING( L"Created button1\n" );
			//this->AttachChild( m_button1 );
			m_button1->SetWidgetDimensions( m_button1Dimensions );
			KIWIDEBUGSTRING( Kiwi::ToWString( L"Dimensions: " + m_button1Dimensions.ToString() ).c_str() );
			m_button1->ConnectUIButtonEventReceiver( *this, L"ButtonUp", &UIScrollBar::_ScrollStop );
			m_button1->ConnectUIButtonEventReceiver( *this, L"ButtonDown", &UIScrollBar::_ScrollStart );
			m_button1->ConnectUIButtonEventReceiver( *this, L"ButtonHeld", &UIScrollBar::_ScrollStart );

			//m_button1->SetWidgetPosition( m_button1Position );
			KIWIDEBUGSTRING( Kiwi::ToWString( L" Position: " + m_button1Position.ToString() ).c_str() );
		}

		/*m_button2 = m_widgetCanvas->CreateButton( m_objectName + L"/Button2", m_buttonTexture );
		if( m_button2 != nullptr )
		{
			KIWIDEBUGSTRING( L"Created button2. " );
			this->AttachChild( m_button2 );
			m_button2->SetWidgetDimensions( m_button2Dimensions );
			KIWIDEBUGSTRING( Kiwi::ToWString( L"Dimensions: " + m_button2Dimensions.ToString() ).c_str() );
			m_button2->ConnectUIButtonEventReceiver( *this, L"ButtonUp", &UIScrollBar::_ScrollStop );
			m_button2->ConnectUIButtonEventReceiver( *this, L"ButtonDown", &UIScrollBar::_ScrollStart );
			m_button2->ConnectUIButtonEventReceiver( *this, L"ButtonHeld", &UIScrollBar::_ScrollStart );

			m_button2->SetWidgetPosition( m_button2Position );
			KIWIDEBUGSTRING( Kiwi::ToWString( L" Position: " + m_button2Position.ToString() ).c_str() );
		}*/

		//m_backgroundSprite = m_widgetCanvas->CreateSprite( m_objectName + L"/Background", m_backgroundTexture );
		//if( m_backgroundSprite != nullptr )
		//{
		//	this->AttachChild( m_backgroundSprite );

		//	//height of the background sprite should be the distance between the inner edges of the buttons
		//	m_backgroundSprite->SetWidgetDimensions( m_widgetDimensions - Kiwi::Vector2d( 0.0, (m_button1Dimensions.y / 2.0) + (m_button2Dimensions.y / 2.0) ) );

		//	//background goes under everything else
		//	m_backgroundSprite->SetWidgetDepth( -0.01 );
		//}

		/*m_slider = m_widgetCanvas->CreateButton( m_objectName + L"/Slider", m_sliderTexture );
		if( m_slider != nullptr )
		{
			m_slider->SetWidgetDimensions( Kiwi::Vector2d( m_widgetDimensions.x, m_minSliderHeight ) );
		}
		this->_AdjustSliderSettings();*/

		m_widgetCanvas->_NotifyWidgetResized( *this );
		m_widgetCanvas->_NotifyWidgetMoved( *this );

		KIWIDEBUGSTRING(m_button1->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform )->GetScale().ToString().c_str());
	}

	void UIScrollBar::_OnUpdate()
	{
		//if( m_currentScrollDirection != 0 )
		//{
		//	m_buttonHoldTimer += _kGameTimer.GetDeltaTime();
		//	if( m_buttonHoldTimer > m_buttonHoldDelay )
		//	{
		//		m_scrollTimer += _kGameTimer.GetDeltaTime();
		//		if( m_scrollTimer > m_scrollFrequency )
		//		{
		//			m_scrollTimer -= m_scrollFrequency;
		//			this->_Scroll();
		//		}
		//	}
		//}

		//if( m_sliderPressed == true )
		//{
		//	Kiwi::UIButton* slider = this->FindComponent<Kiwi::UIButton>( m_objectName + L"/Slider" );
		//	if( slider != nullptr )
		//	{
		//		double mouseYPos = m_scene->GetRenderWindow()->GetInputManager().GetMousePosition().y;
		//		if( mouseYPos > m_transform->GetGlobalPosition().y + m_sliderYMax )
		//		{
		//			mouseYPos = m_sliderYMax;

		//		} else if( mouseYPos < m_transform->GetGlobalPosition().y - m_sliderYMin )
		//		{
		//			mouseYPos = m_sliderYMin;
		//		}
		//		//m_currentScrollStep = m_totalScrollDistance / mouseYPos;

		//		m_currentScrollStep = uint( (m_sliderYMax - mouseYPos) / m_scrollStepY );

		//		slider->SetWidgetPosition( Kiwi::Vector2d( slider->GetWidgetPosition().x, mouseYPos ) );
		//	}
		//}
	}

	void UIScrollBar::_OnWidgetResized( const Kiwi::Vector2d& resizeAmount )
	{
		//m_button1Position = Kiwi::Vector2d( 0.0, (m_widgetDimensions.y / 2.0) - (m_button1Dimensions.y / 2.0) );
		//m_button2Position = Kiwi::Vector2d( 0.0, (m_widgetDimensions.y / -2.0) + (m_button2Dimensions.y / 2.0) );

		///*update the button y positions*/
		//if( m_button1 != nullptr )
		//{
		//	m_button1Position.x = m_button1->GetWidgetPosition().x;
		//	m_button1->SetWidgetPosition( m_button1Position );
		//}

		//if( m_button2 != nullptr )
		//{
		//	m_button2Position.x = m_button2->GetWidgetPosition().x;
		//	m_button2->SetWidgetPosition( m_button2Position );
		//}

		//if( m_backgroundSprite != nullptr )
		//{
		//	//height of the background sprite should be the distance between the inner edges of the buttons
		//	m_backgroundSprite->SetWidgetDimensions( m_widgetDimensions - Kiwi::Vector2d( 0.0, (m_button1Dimensions.y / 2.0) + (m_button2Dimensions.y / 2.0) ) );
		//}

		///*update the slider*/
		//this->_AdjustSliderSettings();
	}

	void UIScrollBar::_Scroll()
	{
		if( m_currentScrollDirection == -1 && m_currentScrollStep < m_totalScrollSteps )
		{/*scroll down*/
			m_currentScrollStep++;

			this->_AdjustSliderSettings();

		} else if( m_currentScrollDirection == 1 && m_currentScrollStep > 1 )
		{/*scroll up*/
			m_currentScrollStep--;

			this->_AdjustSliderSettings();
		}
	}

	Kiwi::EventResponse UIScrollBar::_ScrollStart( std::shared_ptr<Kiwi::UIButtonEvent> evt )
	{
		if( evt->source->GetName().compare( m_objectName + L"/Button1" ) == 0 )
		{
			if( m_currentScrollDirection != 1 )
			{/*if not already scrolling up, start scrolling up now*/
				m_currentScrollDirection = 1;
				m_scrollTimer = 0.0;
				m_buttonHoldTimer = 0.0;
				this->_Scroll();
			}

		} else if( evt->source->GetName().compare( m_objectName + L"/Button2" ) == 0 )
		{
			if( m_currentScrollDirection != -1 )
			{/*if not already scrolling down, start scrolling down now*/
				m_currentScrollDirection = -1;
				m_scrollTimer = 0.0;
				m_buttonHoldTimer = 0.0;
				this->_Scroll();
			}
		}

		return Kiwi::EventResponse::NONE;
	}

	Kiwi::EventResponse UIScrollBar::_ScrollStop( std::shared_ptr<Kiwi::UIButtonEvent> evt )
	{
		if( evt->source->GetName().compare( m_objectName + L"/Button1" ) == 0 || evt->source->GetName().compare( m_objectName + L"/Button2" ) == 0 )
		{
			m_currentScrollDirection = 0;
			m_scrollTimer = 0.0;
			m_buttonHoldTimer = 0.0;
		}

		return Kiwi::EventResponse::NONE;
	}

	void UIScrollBar::SetScrollStepCount( int totalSteps )
	{
		if( totalSteps >  0 && totalSteps != m_totalScrollSteps )
		{
			m_totalScrollSteps = totalSteps;
			this->_AdjustSliderSettings();
		}
	}

	void UIScrollBar::SetSliderWidth( double width )
	{
		/*Kiwi::UIButton* slider = this->FindComponent<Kiwi::UIButton>( m_objectName + L"/Slider" );
		if( slider != nullptr )
		{
			slider->SetWidgetDimensions( Kiwi::Vector2d( width, slider->GetWidgetDimensions().y ) );
		}*/
	}

	void UIScrollBar::SetButton1Dimensions( const Kiwi::Vector2d& dimensions )
	{
		m_button1Dimensions = dimensions;

		m_button1Position = Kiwi::Vector2d( 0.0, (m_widgetDimensions.y / 2.0) - (m_button1Dimensions.y / 2.0) );

		/*update the button y positions*/
		if( m_button1 != nullptr )
		{
			m_button1->SetWidgetDimensions( dimensions );
			m_button1Position.x = m_button1->GetWidgetPosition().x;
			m_button1->SetWidgetPosition( m_button1Position );

			this->_AdjustSliderSettings();
		}
	}

	void UIScrollBar::SetButton2Dimensions( const Kiwi::Vector2d& dimensions )
	{
		m_button2Dimensions = dimensions;

		m_button2Position = Kiwi::Vector2d( 0.0, (m_widgetDimensions.y / -2.0) + (m_button2Dimensions.y / 2.0) );

		/*update the button y positions*/
		if( m_button2 != nullptr )
		{
			m_button2->SetWidgetDimensions( dimensions );
			m_button2Position.x = m_button2->GetWidgetPosition().x;
			m_button2->SetWidgetPosition( m_button2Position );

			this->_AdjustSliderSettings();
		}
	}

	//void UIScrollBar::SetLineCount( int lineCount )
	//{
	//	
	//	if( lineCount < 1 ) lineCount = 1;

	//	m_lineCount = lineCount;

	//	this->_UpdateSlider();
	//	this->_UpdateSliderPosition();

	//}

	//void UIScrollBar::SetMinimumSliderHeight( int height )
	//{

	//	m_minSliderHeight = height;

	//}

	//void UIScrollBar::ScrollToLine( unsigned int position )
	//{

	//	m_currentLine = position;
	//	Kiwi::clamp( m_currentLine, (unsigned int)1, m_lineCount );

	//	this->_UpdateSliderPosition();

	//}

	//void UIScrollBar::_OnButtonPress( const Kiwi::UIButtonEvent& evt )
	//{

	//	Kiwi::UIButton* button = evt.GetSource();
	//	if( button == m_button1 ) //up / left button
	//	{
	//		if( m_currentLine > 1 )
	//		{
	//			m_currentLine -= m_linesPerScroll;
	//			Kiwi::clamp( m_currentLine, (unsigned int)1, m_lineCount );

	//			this->_UpdateSliderPosition();

	//			this->BroadcastScrollBarEvent( Kiwi::UIScrollBarEvent( this, -1.0, m_currentLine, 1.0, m_lineCount ) );
	//		}

	//	}else if( button == m_button2 ) //down / right button
	//	{
	//		if( m_currentLine < m_lineCount )
	//		{
	//			m_currentLine += m_linesPerScroll;
	//			Kiwi::clamp( m_currentLine, (unsigned int)1, m_lineCount );

	//			this->_UpdateSliderPosition();

	//			this->BroadcastScrollBarEvent( Kiwi::UIScrollBarEvent( this, 1.0, m_currentLine, 1.0, m_lineCount ) );
	//		}

	//	}

	//}

	//void UIScrollBar::_OnButtonHeld( const Kiwi::UIButtonEvent& evt )
	//{

	//	if( m_entity && m_entity->GetScene() )
	//	{
	//		Kiwi::Transform* transform = m_entity->FindComponent<Kiwi::Transform>();
	//		assert( transform != 0 );

	//		Kiwi::UIButton* button = evt.GetSource();

	//		double deltaTime = m_entity->GetScene()->GetEngine()->GetGameTimer()->GetDeltaTime();
	//		m_scrollTimer += deltaTime;

	//		if( m_scrollTimer > 1.0 / (double)m_scrollFrequency )
	//		{
	//			m_scrollTimer -= 1.0 / (double)m_scrollFrequency;

	//			if( button == m_button1 )
	//			{
	//				if( m_currentLine > 1 )
	//				{
	//					m_currentLine -= m_linesPerScroll;
	//					Kiwi::clamp( m_currentLine, (unsigned int)1, m_lineCount );

	//					this->_UpdateSliderPosition();

	//					this->BroadcastScrollBarEvent( Kiwi::UIScrollBarEvent( this, -1.0, m_currentLine, 1.0, m_lineCount ) );
	//				}

	//			} else if( button == m_button2 )
	//			{
	//				if( m_currentLine < m_lineCount )
	//				{
	//					m_currentLine += m_linesPerScroll;
	//					Kiwi::clamp( m_currentLine, (unsigned int)1, m_lineCount );

	//					this->_UpdateSliderPosition();

	//					this->BroadcastScrollBarEvent( Kiwi::UIScrollBarEvent( this, 1.0, m_currentLine, 1.0, m_lineCount ) );
	//				}
	//			}

	//		} else if( button == m_slider )
	//		{
	//			Kiwi::Vector2d mDelta = m_entity->GetScene()->GetRenderWindow()->GetInputManager().GetMouseDelta();
	//			mDelta.y /= m_entity->GetScene()->GetRenderWindow()->GetInputManager().GetMouseSpeed().y;

	//			m_sliderY -= mDelta.y;
	//			Kiwi::clamp( m_sliderY, m_sliderYMin, m_sliderYMax );
	//			m_currentLine = (unsigned int)ceil( (m_sliderYMax - round( m_sliderY )) / m_scrollStepY );
	//			Kiwi::clamp( m_currentLine, (unsigned int)1, m_lineCount );

	//			if( m_slider != 0 )
	//			{
	//				Kiwi::Entity* slider = m_slider->GetEntity();
	//				if( slider != 0 )
	//				{
	//					Kiwi::Transform* sliderTransform = slider->FindComponent<Kiwi::Transform>();
	//					if( sliderTransform != 0 )
	//					{
	//						sliderTransform->SetLocalPosition( Kiwi::Vector3d( sliderTransform->GetLocalPosition().x, m_sliderY, sliderTransform->GetLocalPosition().z ) );
	//					}
	//				}
	//			}

	//			this->BroadcastScrollBarEvent( Kiwi::UIScrollBarEvent( this, mDelta.y, m_currentLine, 1.0, m_lineCount ) );
	//		}
	//	}

	//}

	//void UIScrollBar::_OnButtonRelease( const Kiwi::UIButtonEvent& evt )
	//{

	//	m_scrollTimer = 0.0;

	//}
}