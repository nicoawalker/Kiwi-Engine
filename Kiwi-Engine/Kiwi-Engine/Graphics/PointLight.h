#ifndef _KIWI_POINTLIGHT_H_
#define _KIWI_POINTLIGHT_H_

#include "Color.h"

#include "../Core/Entity.h"
#include "../Core/Vector4.h"

namespace Kiwi
{

	class PointLight:
		public Kiwi::Entity
	{
	protected:

		Kiwi::Color m_diffuseColor;

		Kiwi::Vector3 m_attenuationFactor;

		float m_range;

	public:

		PointLight(std::wstring name, Kiwi::Scene* scene, float range, const Kiwi::Vector3& attenuation );
		PointLight( std::wstring name, Kiwi::Scene* scene, float range, const Kiwi::Vector3& attenuation, const Kiwi::Color& diffuseColor );
		virtual ~PointLight() {}

		void SetDiffuseColor( const Kiwi::Color& color ) { m_diffuseColor = color; }
		void SetAttenuationFactor( const Kiwi::Vector3& attenuationFactor ) { m_attenuationFactor = attenuationFactor; }
		void SetRange( float range ) { m_range = range; }

		const Kiwi::Color& GetDiffuseColor()const { return m_diffuseColor; }
		const Kiwi::Vector3& GetAttenuationFactor()const { return m_attenuationFactor; }
		float GetRange()const { return m_range; }

	};
}

#endif