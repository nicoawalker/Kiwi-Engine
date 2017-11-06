#ifndef _KIWI_SCENEEVENT_H_
#define _KIWI_SCENEEVENT_H_

#include "IEvent.h"

namespace Kiwi
{

	class Scene;

	enum SceneEventType { ENTITY_CREATED, ENTITY_DESTROYED, SCENE_STARTED, SCENE_SHUTDOWN, PRERENDER, RENDER_START, RENDER_END, POSTRENDER };

	class SceneEvent :
		public Kiwi::IEvent<Kiwi::Scene>
	{
	public:

		SceneEventType eventType;

	public:

		SceneEvent( Kiwi::Scene& source, const std::wstring& category, SceneEventType evtType ) :
			IEvent<Kiwi::Scene>( &source, category ) {}

		virtual ~SceneEvent() {}

	};

};

#endif