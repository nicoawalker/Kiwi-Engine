#include "ISceneEventListener.h"

namespace Kiwi
{

	void ISceneEventListener::_OnSceneEvent( const Kiwi::SceneEvent& evt )
	{

		switch( evt.GetEventType() )
		{
			case SceneEvent::ENTITY_CREATED:
				{

					break;
				}
			case SceneEvent::ENTITY_DESTROYED:
				{

					break;
				}
			case SceneEvent::SCENE_STARTED:
				{
					this->_OnSceneStarted( evt );
					break;
				}
			case SceneEvent::ASSET_ADDED:
				{
					this->_OnAssetAdded( evt );
					break;
				}
			default:break;
		}

	}

}