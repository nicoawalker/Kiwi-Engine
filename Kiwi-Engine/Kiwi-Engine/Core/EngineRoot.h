#ifndef _KIWI_ENGINEROOT_H_
#define _KIWI_ENGINEROOT_H_

#include "Singleton.h"
#include "SceneManager.h"
#include "GameTimer.h"
#include "Console.h"
#include "IGameObjectFactory.h"
#include "IAssetFactory.h"
#include "AssetManager.h"
#include "InputTranslator.h"

#include "Threading\ThreadPool.h"

#include "../Graphics/GraphicsManager.h"

#include "..\Physics\PhysicsSystem.h"

#include "../Events/IFrameEventEmitter.h"

#include <unordered_set>
#include <mutex>
#include <deque>
#include <string>
#include <memory>

namespace Kiwi
{

	class IEngineApp;
	class LoggingWorker;

	class EngineRoot:
		public Kiwi::Singleton<Kiwi::EngineRoot>,
		public Kiwi::IFrameEventEmitter
	{
	protected:

		//class that contains and controls all created scenes
		Kiwi::SceneManager m_sceneManager;

		Kiwi::AssetManager m_assetManager;

		//class used to create and manage graphical components (e.g. renderer, renderWindow...)
		Kiwi::GraphicsManager m_graphicsManager;

		Kiwi::PhysicsSystem m_physicsSystem;

		Kiwi::GameTimer m_gameTimer;

		Kiwi::Console m_console;

		Kiwi::InputTranslator m_inputTranslator;

		Kiwi::ThreadPool m_threadPool;

		std::unordered_set<unsigned int> m_activeThreadIDs;

		/*map with ownership over all of the game object factory objects used by the engine, where the key is the ObjectType of the factory*/
		//std::unordered_map<std::wstring, std::unique_ptr<Kiwi::IGOFactory>> m_objectFactories;

		//std::unordered_map<std::wstring, std::unique_ptr<Kiwi::IAssetFactory>> m_assetFactories;

		std::unordered_map<std::wstring, std::unique_ptr<Kiwi::IFactory>> m_factories;

		std::shared_ptr<LoggingWorker> m_logWorker;

		std::recursive_mutex m_engineMutex;

		std::wstring m_logFile;

		/*amount to scale all 2D objects by*/
		double m_globalUIScale;

		bool m_engineRunning;
		bool m_initialized;
		bool m_shutdown;

	protected:

		void _MainLoop();

		/*dispatched all current messages sent to the window
		returns false if WM_QUIT is received*/
		bool _PumpMessages();

	public:

		EngineRoot();
		~EngineRoot();

		void Initialize( std::wstring logFile );

		void Shutdown();

		void Start();
		void Pause() {}
		void Stop();

		/*adds a new message to the output queue to be written to the current log file*/
		void Log( const std::wstring log );

		/*stores the factory so that instances of it maybe be created via the CreateFactory method*/
		void RegisterNewFactory( std::unique_ptr<Kiwi::IFactory> factory );

		void SetGlobalUIScale( double globalScale ) { m_globalUIScale = globalScale; }

		double GetGlobalUIScale()const { return m_globalUIScale; }

		Kiwi::GameTimer& GetGameTimer() { return m_gameTimer; }
		Kiwi::Console& GetConsole() { return m_console; }
		Kiwi::SceneManager& GetSceneManager() { return m_sceneManager; }
		Kiwi::GraphicsManager& GetGraphicsManager() { return m_graphicsManager; }
		Kiwi::PhysicsSystem& GetPhysicsSystem() { return m_physicsSystem; }
		Kiwi::AssetManager& GetAssetManager() { return m_assetManager; }
		Kiwi::ThreadPool& GetThreadPool() { return m_threadPool; }
		Kiwi::InputTranslator& GetInputTranslator() { return m_inputTranslator; }

		/*returns ownership of a new copy of the asset factory used to create objects of the given type, or a null pointer if there is no such factory*/
		template<typename FactoryType>
		std::unique_ptr<FactoryType> CreateFactory( std::wstring objectType )
		{
			std::lock_guard<std::recursive_mutex> lockGuard( m_engineMutex );

			auto factoryItr = m_factories.find( objectType );
			if( factoryItr != m_factories.end() )
			{
				/*create a new clone of the correct derived type for the factory, then store the casted result into a new unique pointer and return it*/
				FactoryType* factory = dynamic_cast<FactoryType*>(factoryItr->second->Clone());
				return std::unique_ptr<FactoryType>( factory );
			}

			/*no matching factory found, so return a null pointer*/
			return nullptr;
		}

	};

}

#define _kEngine Kiwi::EngineRoot::GetSingleton()
#define _kThreadPool _kEngine.GetThreadPool()
#define _kGraphics _kEngine.GetGraphicsManager()
#define _kPhysics _kEngine.GetPhysicsSystem()
#define _kAssetManager _kEngine.GetAssetManager()
#define _kSceneManager _kEngine.GetSceneManager()
#define _kGameTimer _kEngine.GetGameTimer()
#define _kConsole _kEngine.GetConsole()
#define _kInput _kEngine.GetInputTranslator()

#endif