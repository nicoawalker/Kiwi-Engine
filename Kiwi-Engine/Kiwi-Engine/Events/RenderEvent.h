#ifndef _KIWI_RENDEREVENT_H_
#define _KIWI_RENDEREVENT_H_

#include "IEvent.h"

#include "../Types.h"

#include "..\Core\Vector2d.h"

namespace Kiwi
{

	class Renderer;

	class RenderEvent :
		public Kiwi::IEvent<Kiwi::Renderer>
	{
	public:

		RenderEvent( Kiwi::Renderer* source, const std::wstring& event ) :
			Kiwi::IEvent<Renderer>( source, event )
		{
		}

	};

}

#endif