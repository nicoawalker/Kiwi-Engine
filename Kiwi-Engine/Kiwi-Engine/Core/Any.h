#ifndef _KIWI_ANY_H_
#define _KIWI_ANY_H_

#include "Utilities.h"
#include "Exception.h"

#include <typeinfo>
#include <string>
#include <unordered_map>

namespace Kiwi
{

	class Any
	{
	protected:

		class AnyType
		{
		public:

			virtual ~AnyType() {}
			virtual const std::type_info& TypeInfo() const = 0;
			virtual AnyType* Clone() const = 0;

		};

		template<typename ValueType> 
		class Value :
			public AnyType
		{
		public:

			const ValueType held;

		public:

			Value( const ValueType& value ) :
				held( value ) {}
			~Value() {}

			virtual const std::type_info& TypeInfo() const { return typeid(ValueType); }
			virtual AnyType* Clone() const { return new Value( held ); }

		};

	protected:

		AnyType* content;

	public:

		Any();
		Any( const Kiwi::Any& other );

		template<typename ValueType>
		Any( const ValueType& value ) :
			content( new Value<ValueType>( value ) ) {}

		~Any();

		const std::type_info& TypeInfo() const;

		Kiwi::Any& Swap( Kiwi::Any& other );

		Kiwi::Any& operator=( const Kiwi::Any& other );

		operator const void*() const;

		template<typename ValueType> 
		Kiwi::Any& operator=( const ValueType& val )
		{ 

			return this->Swap( Kiwi::Any( val ) ); 

		}
		
		template<typename ValueType>
		bool CopyTo( ValueType& val ) const
		{

			const ValueType* copy = this->GetPointer<ValueType>();
			if( copy )
			{
				val = *copy;
				return true;
			} else
			{
				return false;
			}

		}
		
		template<typename ValueType>
		const ValueType* GetPointer() const
		{ 

			return ( this->TypeInfo().hash_code() == typeid(ValueType).hash_code() ) ? &static_cast<Value<ValueType>*>( content )->held  : 0;

		}

	};

	template<typename ValueType>
	ValueType Any_Cast( const Kiwi::Any& castee )
	{

		const ValueType* result = castee.GetPointer<ValueType>();
		if( result != 0 )
		{
			return *result;

		} else
		{
			std::wstring fromType = Kiwi::ToWString( castee.TypeInfo().name());
			std::wstring toType = Kiwi::ToWString(typeid(ValueType).name());
			throw Kiwi::Exception( L"Any::GetPointer", L"Bad cast. Cannot cast from '" + fromType + L"' to '" + toType + L"'", KIWI_EXCEPTION::BADCAST );
		}

	}

	typedef std::unordered_map<std::wstring, Kiwi::Any> AnyMap;

}

#endif