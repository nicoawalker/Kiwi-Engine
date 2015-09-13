#ifndef _KIWI_RENDERTARGETMANAGER_H_
#define _KIWI_RENDERTARGETMANAGER_H_

#include "RenderTarget.h"

#include "../Core/ComponentContainer.h"

#include <unordered_map>
#include <vector>
#include <string>

namespace Kiwi
{

	class RenderTargetManager
	{
	protected:

		Kiwi::ComponentContainer<std::wstring, Kiwi::RenderTarget> m_renderTargets;

	public:

		RenderTargetManager() {}
		~RenderTargetManager() {}

		void AddRenderTarget( Kiwi::RenderTarget* renderTarget );

		Kiwi::RenderTarget* FindRenderTargetWithName( std::wstring name ) { return m_renderTargets.Find( name ); }

		void DestroyRenderTargetWithName( std::wstring name ) { m_renderTargets.Destroy( name ); }
		void DestroyRenderTarget( Kiwi::RenderTarget* renderTarget ) { m_renderTargets.Destroy( renderTarget ); }

	};

}

#endif