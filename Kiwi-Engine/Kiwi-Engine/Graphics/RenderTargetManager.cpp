#include "RenderTargetManager.h"

#include "../Core/Utilities.h"

#include <algorithm>

namespace Kiwi
{

	void RenderTargetManager::AddRenderTarget( Kiwi::RenderTarget* renderTarget )
	{

		if( renderTarget )
		{
			m_renderTargets.Add( renderTarget->GetName(), renderTarget );
		}

	}

}