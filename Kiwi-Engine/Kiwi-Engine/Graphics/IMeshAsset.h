#ifndef _KIWI_IMESHASSET_H_
#define _KIWI_IMESHASSET_H_

#include "..\Core\IAsset.h"

namespace Kiwi
{

	class IMeshAsset:
		public Kiwi::IAsset
	{
	protected:

		/*stores whether or not this mesh will be used as a shared mesh, in which case instancing will be used*/
		bool m_isShared;

	public:

		IMeshAsset( const std::wstring& assetName, const std::wstring& assetCategory, const std::wstring& assetSubcategory );

		//copies the contents of an existing mesh into this one
		IMeshAsset( const Kiwi::IMeshAsset& source );
		~IMeshAsset() = 0;

		virtual std::unique_ptr<Kiwi::IAsset> Clone() { return nullptr; }

		void SetShared( bool isShared ) { m_isShared = isShared; }

		bool IsShared()const { return m_isShared; }

	};

}

#endif