#ifndef _KIWI_IENGINEAPP_H_
#define _KIWI_IENGINEAPP_H_

#include "EngineRoot.h"

#include "..\Events\EventHandling.h"
#include "../Events/WindowEvent.h"

#include <memory>

namespace Kiwi
{

	class EngineRoot;

	class IEngineApp:
		public Kiwi::IFrameEventReceiver
	{
	private:

		std::unique_ptr<Kiwi::EngineRoot> m_engine;

	private:

		Kiwi::EventResponse _OnFrameEvent( std::shared_ptr<Kiwi::FrameEvent> evt );

	protected:

		virtual void _OnUpdate() {}
		virtual void _OnFixedUpdate() {}
		virtual void _OnShutdown() {}
		//virtual void _OnRenderStart() {}
		//virtual void _OnRenderEnd() {}

	public:

		IEngineApp();
		virtual ~IEngineApp();

		void Update() { this->_OnUpdate(); }
		void FixedUpdate() { this->_OnFixedUpdate(); }

		void Shutdown();

		virtual void Launch() = 0;

	};
};

#endif