#ifndef _KIWI_ENGINEROOT_H_
#define _KIWI_ENGINEROOT_H_

#include "FrameEventBroadcaster.h"
#include "SceneManager.h"
#include "GameTimer.h"

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

		//stores a pointer to the main game window
		Kiwi::RenderWindow* m_gameWindow;

		Kiwi::GameTimer m_gameTimer;

		bool m_engineRunning;

	protected:

		virtual void _MainLoop();
		virtual void _PumpMessages();

	public:

		EngineRoot();
		~EngineRoot();

		void Start( std::wstring logFile, Kiwi::RenderWindow* gameWindow );
		void Pause() {}
		void Stop();

		void SetGameWindow( Kiwi::RenderWindow* window ) { if( window ) m_gameWindow = window; }

		Kiwi::GameTimer* GetGameTimer() { return &m_gameTimer; }

		Kiwi::RenderWindow* GetGameWindow()const { return m_gameWindow; }
		Kiwi::SceneManager* GetSceneManager() { return &m_sceneManager; }
		Kiwi::GraphicsManager* GetGraphicsManager() { return &m_graphicsManager; }

	};

}

#endif