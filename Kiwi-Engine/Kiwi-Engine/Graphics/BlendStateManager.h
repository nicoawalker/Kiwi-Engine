#ifndef _KIWI_BLENDSTATEMANAGER_H_
#define _KIWI_BLENDSTATEMANAGER_H_

#include "BlendState.h"
#include "DirectX.h"

#include "../Core/ComponentContainer.h"

#include <string>

namespace Kiwi
{

	class Renderer;

	struct D3DBlendDesc
	{
		bool alphaToCoverageEnable;
		bool independentBlendEnable;

		D3DBlendDesc()
		{
			alphaToCoverageEnable = false;
			independentBlendEnable = false;
		}
	};

	struct D3DRenderTargetBlendDesc
	{

		D3D11_RENDER_TARGET_BLEND_DESC desc;

		D3DRenderTargetBlendDesc()
		{
			memset( &desc, 0, sizeof( D3D11_RENDER_TARGET_BLEND_DESC ) );
		}
	};

	class BlendStateManager
	{
	protected:

		Kiwi::Renderer* m_renderer;

		Kiwi::ComponentContainer<std::wstring, Kiwi::BlendState> m_blendStates;

	public:

		BlendStateManager( Kiwi::Renderer* renderer );
		~BlendStateManager() {}

		Kiwi::BlendState* CreateBlendState( std::wstring name, const D3DBlendDesc& blendDesc, const D3DRenderTargetBlendDesc& rtBlendDesc, const Kiwi::Vector4& blendFactor, unsigned int sampleMask );

		void AddBlendState( Kiwi::BlendState* rasterState );

		Kiwi::BlendState* FindBlendStateWithName( std::wstring name ) { return m_blendStates.Find( name ); }

		void DestroyAllBlendStates() { m_blendStates.DestroyAll(); }
		void DestroyBlendStateWithName( std::wstring name ) { m_blendStates.Destroy( name ); }

	};
}

#endif