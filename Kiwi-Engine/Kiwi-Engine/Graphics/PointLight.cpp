#include "PointLight.h"

namespace Kiwi
{

	PointLight::PointLight( std::wstring name, Kiwi::Scene* scene, float range, const Kiwi::Vector3& attenuation ):
		Entity(name, scene)
	{

		m_diffuseColor = Kiwi::Color( 1.0f, 1.0f, 1.0f, 1.0f );
		m_range = range;
		m_attenuationFactor = attenuation;

	}

	PointLight::PointLight( std::wstring name, Kiwi::Scene* scene, float range, const Kiwi::Vector3& attenuation, const Kiwi::Color& diffuseColor ):
		Entity(name, scene)
	{

		m_diffuseColor = diffuseColor;
		m_range = range;
		m_attenuationFactor = attenuation;

	}

}