#ifndef _KIWI_BLENDSTATEMANAGER_H_
#define _KIWI_BLENDSTATEMANAGER_H_

#include "BlendState.h"
#include "DirectX.h"

#include "../Core/ComponentContainer.h"

#include <string>

namespace Kiwi
{

	class Renderer;

	struct D3DBlendDescProperties
	{
		bool alphaToCoverageEnable;
		bool independentBlendEnable;

		D3DBlendDescProperties()
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
	private:

		struct ObjectMultiKey
		{
			std::wstring name; //unique name
			long uid; //unique id

			ObjectMultiKey( std::wstring n, long i ) :
				name( n ), uid( i )
			{
			}

			bool operator== ( const ObjectMultiKey& right )const
			{
				return name.compare( right.name ) == 0 || uid == right.uid;
			}

		};

		struct ObjectMultiKeyHash
		{
			std::size_t operator() ( const ObjectMultiKey& key )const
			{
				return std::hash<std::wstring>()(key.name) ^ ((std::hash<long>()(key.uid) << 1) >> 1);
			}
		};
	protected:

		Kiwi::Renderer* m_renderer;

		std::unordered_map<ObjectMultiKey, std::unique_ptr<Kiwi::BlendState>, ObjectMultiKeyHash> m_blendStates;

	protected:

		Kiwi::BlendState* _FindBlendState( const ObjectMultiKey& key );

		void _DestroyBlendState( const ObjectMultiKey& key );

	public:

		BlendStateManager( Kiwi::Renderer& renderer );
		~BlendStateManager() {}

		Kiwi::BlendState* CreateBlendState( std::wstring name, const D3DBlendDescProperties& blendDesc, const D3DRenderTargetBlendDesc& rtBlendDesc, const Kiwi::Vector4& blendFactor, unsigned int sampleMask );

		void AddBlendState( std::unique_ptr<Kiwi::BlendState> blendState );

		Kiwi::BlendState* FindBlendState( std::wstring name );

		Kiwi::BlendState* FindBlendState( long uid );

		void DestroyAllBlendStates() { m_blendStates.clear(); }

		void DestroyBlendState( std::wstring name );

		void DestroyBlendState( long uid );

	};
}

#endif