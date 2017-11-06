#ifndef _KIWI_ITRANSFORMEVENTLISTENER_H_
#define _KIWI_ITRANSFORMEVENTLISTENER_H_

#include "TransformEvent.h"

namespace Kiwi
{

	class ITransformEventBroadcaster;

	class ITransformEventListener
	{
	friend class ITransformEventBroadcaster;
	private:

		void _OnTransformEvent( const Kiwi::TransformEvent& evt );

	protected:

		virtual void _OnTransformRotate( const Kiwi::TransformEvent& evt ) {}
		virtual void _OnTransformTranslate( const Kiwi::TransformEvent& evt ) {}

	public:

		ITransformEventListener(){}
		virtual ~ITransformEventListener(){}

	};
};

#endif