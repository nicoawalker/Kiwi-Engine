#ifndef _KIWI_UISCROLLBAREVENT_H_
#define _KIWI_UISCROLLBAREVENT_H_

#include "Event.h"

namespace Kiwi
{

	class UIScrollBar;

	class UIScrollBarEvent: 
		public Kiwi::Event<Kiwi::UIScrollBar>
	{
	protected:

		double m_sliderDelta;
		double m_sliderValue;
		double m_minSliderValue;
		double m_maxSliderValue;

	public:

		UIScrollBarEvent( Kiwi::UIScrollBar* source, double sliderDelta, double sliderValue, double minSliderValue, double maxSliderValue ) :
			Event(source), 
			m_sliderDelta( sliderDelta ), m_sliderValue(sliderValue), m_minSliderValue(minSliderValue), m_maxSliderValue(maxSliderValue) {}

		virtual ~UIScrollBarEvent() {}

		double GetSliderDelta()const { return m_sliderDelta; }
		double GetSliderValue()const { return m_sliderValue; }
		double GetSliderMin()const { return m_minSliderValue; }
		double GetSliderMax()const { return m_maxSliderValue; }

	};
};

#endif