#ifndef _KIWI_UISCROLLBAR_H_
#define _KIWI_UISCROLLBAR_H_

#include "IUIWidget.h"

#include "../../Events/EventHandling.h"
//#include "..\..\Events\UIScrollBarEventBroadcaster.h"

#include "..\Color.h"

#include "../../Core/Vector2d.h"

#include <string>

namespace Kiwi
{

	class UIButton;
	class UISprite;
	class UserInterface;
	class UICanvas;

	class UIScrollBar:
		public Kiwi::IUIWidget,
		public Kiwi::IUIButtonEventReceiver
	{
	protected:

		Kiwi::UIButton* m_button1;
		Kiwi::UIButton* m_button2;
		Kiwi::UIButton* m_slider;
		Kiwi::UISprite* m_backgroundSprite;

		std::wstring m_buttonTexture;
		std::wstring m_sliderTexture;
		std::wstring m_backgroundTexture;

		Kiwi::Vector2d m_button1Dimensions;
		Kiwi::Vector2d m_button2Dimensions;
		Kiwi::Vector2d m_sliderDimensions;
		Kiwi::Vector2d m_button1Position;
		Kiwi::Vector2d m_button2Position;

		int m_sliderHeight;
		int m_minSliderHeight; //slider will not shrink any smaller than this
		int m_totalScrollDistance; //total distance the slider can travel
		int m_currentScrollDirection; //-1 = down, 0 = no scroll, 1 = up

		uint m_totalScrollSteps; //number of button presses/steps to get the slider from top to bottom
		uint m_currentScrollStep;

		double m_scrollFrequency; //amount of time between each scroll event if a button is held, in milliseconds
		double m_scrollTimer;
		double m_scrollPercentage; //percentage of the way to the bottom the slider currently is
		double m_scrollStepY; //change in position of scroll bar for each line
		double m_sliderYMax; //stores the position of the slider if it is at the top of the scroll bar
		double m_sliderYMin;
		double m_sliderYPos; //stores the current slider y position
		double m_buttonHoldDelay; //how long either button must be held before the slider will continuously scroll
		double m_buttonHoldTimer;

		bool m_sliderPressed; //true if the slider is currently pressed

	protected:

		/*updates the slider properies, dimensions, and position based on the current settings*/
		void _AdjustSliderSettings();

		Kiwi::EventResponse _OnSliderPress( std::shared_ptr<Kiwi::UIButtonEvent> evt );
		Kiwi::EventResponse _OnSliderRelease( std::shared_ptr<Kiwi::UIButtonEvent> evt );

		void _OnStart();
		void _OnUpdate();

		void _OnWidgetResized( const Kiwi::Vector2d& resizeAmount );

		void _Scroll();

		Kiwi::EventResponse _ScrollStart( std::shared_ptr<Kiwi::UIButtonEvent> evt );
		Kiwi::EventResponse _ScrollStop( std::shared_ptr<Kiwi::UIButtonEvent> evt );

		void _UpdateSlider();
		void _UpdateSliderPosition();

	public:

		UIScrollBar( const std::wstring& name, Kiwi::UICanvas& canvas, const std::wstring& buttonTexture = L"", const std::wstring& sliderTexture = L"", const std::wstring& backgroundTexture = L"" );
		~UIScrollBar();

		double GetMinimumSliderHeight()const { return m_minSliderHeight; }

		double GetScrollFrequency()const { return m_scrollFrequency; }

		/*sets how many steps the slider will go through from top to bottom*/
		void SetScrollStepCount( int totalSteps );

		/*sets the minimum height past which the slider will not shrink*/
		void SetMinimumSliderHeight( int minHeight );

		/*sets the current scroll step (updates the position of the slider)*/
		void SetScrollStep( uint currentStep );

		/*sets how many times per second the slider will move when either button is held*/
		void SetScrollFrequency( double scrollsPerSecond );

		void SetSliderWidth( double width );
		void SetButton1Dimensions( const Kiwi::Vector2d& dimensions );
		void SetButton2Dimensions( const Kiwi::Vector2d& dimensions );

		//unsigned int GetLineCount()const { return m_lineCount; }
		//unsigned int GetCurrentLine()const { return m_currentLine; }
		//unsigned int GetScrollSpeed()const { return m_linesPerScroll; }
	};
}

#endif