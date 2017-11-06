#ifndef _KIWI_RENDERSTATEMANAGER_H_
#define _KIWI_RENDERSTATEMANAGER_H_

#include "RasterState.h"

#include "../Core/ComponentContainer.h"

#include <string>
#include <unordered_map>

namespace Kiwi
{

	class Renderer;

	class RasterStateManager
	{
	protected:

		Kiwi::Renderer* m_renderer;

		Kiwi::ComponentContainer<std::wstring, Kiwi::RasterState> m_rasterStates;

	public:

		RasterStateManager( Kiwi::Renderer& renderer );
		virtual ~RasterStateManager();

		Kiwi::RasterState* CreateRasterState( std::wstring name, const Kiwi::D3DRasterStateDesc& desc );

		void AddRasterState( Kiwi::RasterState* rasterState );

		Kiwi::RasterState* FindRasterStateWithName( std::wstring name );

		void DestroyAllRasterStates();
		void DestroyRasterStateWithName( std::wstring name );

	};
};

#endif