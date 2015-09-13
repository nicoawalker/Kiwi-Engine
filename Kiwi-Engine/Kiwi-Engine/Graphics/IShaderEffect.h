#ifndef _KIWI_ISHADEREFFECT_H_
#define _KIWI_ISHADEREFFECT_H_

#include <string>
#include <vector>
#include <unordered_map>

namespace Kiwi
{

	class Any;

	class IShaderEffect
	{
	protected:

		std::wstring m_effectShader;

		std::vector<Kiwi::Any*> m_effectBuffers;

		std::unordered_map<std::wstring, Kiwi::Any*> m_bufferParams;

	public:
		
		IShaderEffect() {}
		virtual ~IShaderEffect() {}

		template<typename ValueType>
		void SetValue( std::wstring valueName, const ValueType& value ) = 0;

		template<typename ValueType>
		ValueType* GetValue( std::wstring valueName ) = 0;

		virtual Kiwi::Any* GetBuffer( unsigned int index ) { return (index < m_effectBuffers.size()) ? m_effectBuffers[index] : 0; }

		virtual std::wstring GetShaderName()const { return m_effectShader; }

	};
}

#endif