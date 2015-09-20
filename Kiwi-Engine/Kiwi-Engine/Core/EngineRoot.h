#ifndef _KIWI_ENGINEROOT_H_
#define _KIWI_ENGINEROOT_H_

#include "FrameEventBroadcaster.h"
#include "SceneManager.h"

#include "../Graphics/GraphicsManager.h"

namespace Kiwi
{

	class EngineRoot:
		public Kiwi::FrameEventBroadcaster
	{
	protected:

		//class that contains and controls all created scenes
		Kiwi::SceneManager m_sceneManager;

		//class used to create and manage graphical components (e.g. renderer, renderWindow...)
		Kiwi::GraphicsManager m_graphicsManager;

		bool m_engineRunning;

	protected:

		virtual void _MainLoop();
		virtual void _PumpMessages();

	public:

		EngineRoot();
		~EngineRoot();

		void Start();
		void Pause() {}
		void Stop();

		Kiwi::SceneManager* GetSceneManager() { return &m_sceneManager; }
		Kiwi::GraphicsManager* GetGraphicsManager() { return &m_graphicsManager; }

	};

}

#endif