#ifndef _KIWI_SCENEEVENT_H_
#define _KIWI_SCENEEVENT_H_

#include "Event.h"

#include "..\Types.h"

namespace Kiwi
{

	class Scene;

	class SceneEvent :
		public Kiwi::Event<Kiwi::Scene>
	{
	public:

		enum SCENEEVENT_TYPE { ENTITY_CREATED, ENTITY_DESTROYED, ASSET_ADDED, ASSET_CREATED, SCENE_STARTED, SCENE_SHUTDOWN };

	protected:

		SCENEEVENT_TYPE m_sceneEventType;

		Kiwi::Scene* m_sourceScene;

		Kiwi::StringPair m_parameters;

	public:

		SceneEvent( Kiwi::Scene& sourceScene, SCENEEVENT_TYPE eventType ) :
			Event( &sourceScene ), m_sceneEventType(eventType), m_sourceScene( &sourceScene ) {}

		SceneEvent( Kiwi::Scene& sourceScene, SCENEEVENT_TYPE eventType, const Kiwi::StringPair& parameters ) :
			Event( &sourceScene ), m_sceneEventType( eventType ), m_sourceScene( &sourceScene ), m_parameters( parameters )
		{
		}

		virtual ~SceneEvent() {}

		SCENEEVENT_TYPE GetEventType()const { return m_sceneEventType; }
		Kiwi::Scene* GetSourceScene()const { return m_sourceScene; }
		const Kiwi::StringPair& GetParameters()const { return m_parameters; }

	};
};

#endif