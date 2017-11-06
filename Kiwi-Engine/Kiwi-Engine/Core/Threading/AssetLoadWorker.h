#ifndef _KIWI_ASSETLOADWORKER_H_
#define _KIWI_ASSETLOADWORKER_H_

#include "IAsyncWorker.h"

#include "..\..\Types.h"

#include <string>
#include <memory>

namespace Kiwi
{

	class AssetManager;
	class IAsset;
	class Exception;
	class AssetLoadQueue;

	struct QueuedAssetData;

	class AssetLoadWorker :
		public Kiwi::IAsyncWorker
	{
	protected:

		std::unique_ptr<QueuedAssetData> m_assetData;

		Kiwi::AssetManager* m_assetManager;

		std::unique_ptr<Kiwi::IAsset> m_loadedAsset;

		std::wstring m_errorMsg;

		long m_assetID;

	public:

		AssetLoadWorker( Kiwi::AssetManager& assetManager, QueuedAssetData& assetData );

		void Run();

		void Finish();

	};
}

#endif