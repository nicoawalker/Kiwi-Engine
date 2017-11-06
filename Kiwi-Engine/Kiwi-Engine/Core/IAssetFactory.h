#ifndef _KIWI_IASSETFACTORY_H_
#define _KIWI_IASSETFACTORY_H_

/*abstract factory class used to create new instances of assets*/

#include "IAsset.h"
#include "IFactory.h"

#include "..\Types.h"

#include <string>
#include <mutex>
#include <memory>

namespace Kiwi
{

	class EngineRoot;

	class IAssetFactory:
		public Kiwi::IFactory
	{
	protected:

		std::recursive_mutex m_factoryMutex;

	protected:

		virtual void _CreateAsset( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const Kiwi::StringPair& parameters ) = 0;

	public:

		IAssetFactory() {}
		virtual ~IAssetFactory() {}

		virtual IFactory* Clone()const = 0;

		/*creates a new instance of an asset*/
		std::unique_ptr<Kiwi::IAsset> CreateAsset( std::wstring assetName );

		/*creates a new instance of an asset*/
		std::unique_ptr<Kiwi::IAsset> CreateAsset( std::wstring assetName, const Kiwi::StringPair& parameters );

		/*returns a string identifying the type of object the factory creates*/
		virtual std::wstring GetObjectType()const = 0;

	};

}

#endif