#ifndef _KIWI_MD5MODELFACTORY_H_
#define _KIWI_MD5MODELFACTORY_H_

#include "IAssetFactory.h"

//#include "..\Graphics\AnimatedMeshAsset.h"

#include <string>

namespace Kiwi
{

	class Scene;

	class MD5ModelFactory :
		public Kiwi::IAssetFactory
	{
	public:

		static std::wstring ObjectType;

	protected:

		void _CreateAsset( std::vector<std::unique_ptr<Kiwi::IAsset>>& _createdAssets, std::wstring assetName, const Kiwi::StringPair& parameters );

	public:

		MD5ModelFactory() {}
		virtual ~MD5ModelFactory() {}

		Kiwi::MD5ModelFactory* Clone()const { return new Kiwi::MD5ModelFactory(); }

		std::wstring GetObjectType()const { return ObjectType; }

	};

}

#endif
