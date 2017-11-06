#ifndef _KIWI_RENDERSTATEOBJECTMANAGER_H_
#define _KIWI_RENDERSTATEOBJECTMANAGER_H_

#include "DirectX.h"
#include "RasterState.h"
#include "DepthStencil.h"
#include "BlendState.h"

#include "..\Core\Vector4.h"

#include <unordered_map>
#include <string>
#include <memory>

namespace Kiwi
{

	class Renderer;

	class RenderStateObjectManager
	{
	private:

		struct ObjectMultiKey
		{
			std::wstring name; //unique name
			long uid; //unique id

			ObjectMultiKey( std::wstring n, long i ) :
				name( n ), uid( i ) {}

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

		std::unordered_map<ObjectMultiKey, std::unique_ptr<Kiwi::RasterState>, ObjectMultiKeyHash> m_rasterStates;
		std::unordered_map<ObjectMultiKey, std::unique_ptr<Kiwi::DepthStencil>, ObjectMultiKeyHash> m_depthStencils;
		std::unordered_map<ObjectMultiKey, std::unique_ptr<Kiwi::BlendState>, ObjectMultiKeyHash> m_blendStates;

	protected:

		Kiwi::RasterState* _FindRasterState( const ObjectMultiKey& key );
		
		Kiwi::DepthStencil* _FindDepthStencil( const ObjectMultiKey& key );

		Kiwi::BlendState* _FindBlendState( const ObjectMultiKey& key );

		void _DestroyRasterState( const ObjectMultiKey& key );
		void _DestroyDepthStencil( const ObjectMultiKey& key );
		void _DestroyBlendState( const ObjectMultiKey& key );

	public:

		RenderStateObjectManager( Kiwi::Renderer& renderer );
		~RenderStateObjectManager();

		Kiwi::RasterState* CreateRasterState( std::wstring name, const Kiwi::D3DRasterStateDesc& desc );

		Kiwi::DepthStencil* CreateDepthStencil( std::wstring name, const DepthBufferProperties& bufferProperties, const DepthStencilProperties& stencilProperties );

		Kiwi::DepthStencil* CreateDepthStencil( std::wstring name, ID3D11Texture2D* depthBuffer, ID3D11DepthStencilView* depthView, ID3D11DepthStencilState* depthState, ID3D11DepthStencilState* depthDisabledState );

		Kiwi::BlendState* CreateBlendState( std::wstring name, const KiwiBlendDesc& rtBlendDesc, unsigned int sampleMask );

		Kiwi::BlendState* FindBlendState( const std::wstring& name );

		Kiwi::RasterState* FindRasterState( std::wstring name );

		Kiwi::RasterState* FindRasterState( long uid );

		Kiwi::DepthStencil* FindDepthStencil( std::wstring name );

		Kiwi::DepthStencil* FindDepthStencil( long uid );

		void DestroyAllRasterStates() { m_rasterStates.clear(); }

		void DestroyAllDepthStencils() { m_depthStencils.clear(); }

		void DestroyBlendState( const std::wstring& name );

		void DestroyRasterState( std::wstring name );

		void DestroyRasterState( long uid );

		void DestroyDepthStencil( std::wstring name );

		void DestroyDepthStencil( long uid );

		void Shutdown();

	};

}

#endif