#ifndef _KIWI_UIBUTTONEVENT_H_
#define _KIWI_UIBUTTONEVENT_H_

#include "IEvent.h"

#include "../Types.h"

namespace Kiwi
{

	class UIButton;

	class UIButtonEvent :
		public Kiwi::IEvent<Kiwi::UIButton>
	{
	public:

		UIButtonEvent( Kiwi::UIButton& source, const std::wstring& assetEventCategory ) :
			Kiwi::IEvent<UIButton>( &source, assetEventCategory ) {}

	};

}

#endif