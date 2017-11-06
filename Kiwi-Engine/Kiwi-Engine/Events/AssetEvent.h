#ifndef _KIWI_ASSETEVENT_H_
#define _KIWI_ASSETEVENT_H_

#include "IEvent.h"

#include "../Types.h"

#include "..\Core\IAsset.h"

namespace Kiwi
{

	class IAsset;

	class AssetEvent :
		public Kiwi::IEvent<Kiwi::IAsset>
	{
	public:

		AssetEvent( Kiwi::IAsset* source, const std::wstring& assetEventType, const Kiwi::StringPair& parameters ) :
			Kiwi::IEvent<IAsset>( source, assetEventType, parameters ) {}

	};

}

#endif