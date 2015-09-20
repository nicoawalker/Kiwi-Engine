#include "IEngineApp.h"
#include "Utilities.h"

namespace Kiwi
{

	IEngineApp::IEngineApp()
	{

		m_engine = new Kiwi::EngineRoot();

		m_graphicsManager = m_engine->GetGraphicsManager();
		m_sceneManager = m_engine->GetSceneManager();

		m_engine->AddListener( this );

	}

	IEngineApp::~IEngineApp()
	{

		SAFE_DELETE( m_engine );

	}

}