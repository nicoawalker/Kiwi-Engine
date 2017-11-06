#ifndef _KIWI_ISCENEEVENTLISTENER_H_
#define _KIWI_ISCENEEVENTLISTENER_H_

#include "SceneEvent.h"

namespace Kiwi
{

	class ISceneEventBroadcaster;

	class ISceneEventListener
	{
		friend class ISceneEventBroadcaster;
	private:

		void _OnSceneEvent( const Kiwi::SceneEvent& evt );

	protected:

		virtual void _OnSceneStarted( const Kiwi::SceneEvent& evt ) {}
		virtual void _OnAssetAdded( const Kiwi::SceneEvent& evt ) {}

	public:

		ISceneEventListener() {}
		virtual ~ISceneEventListener() {}

	};
};

#endif