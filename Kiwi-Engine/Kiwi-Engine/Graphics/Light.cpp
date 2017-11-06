#include "Light.h"

namespace Kiwi
{

	Light::Light( Kiwi::Scene& scene, std::wstring name, LightType lightType, const std::wstring& renderTarget, const Kiwi::Color& color ):
		Kiwi::Entity( name, scene )
	{
		m_entityType = ENTITY_LIGHT;

		m_lightType = lightType;
		m_renderTarget = renderTarget;
		m_lightColor = color;

		m_range = 0.0;
		m_cone = 0.0;
	}

	Light::Light( Kiwi::Scene& scene, std::wstring name, LightType lightType, const std::wstring& renderTarget, const Kiwi::Vector3d& direction, const Kiwi::Color& color ) :
		Kiwi::Entity( name, scene )
	{
		m_entityType = ENTITY_LIGHT;

		m_lightType = lightType;
		m_renderTarget = renderTarget;
		m_lightColor = color;
		m_direction = direction;

		m_range = 0.0;
		m_cone = 0.0;
	}

	Light::Light( Kiwi::Scene& scene, std::wstring name, LightType lightType, const std::wstring& renderTarget, const Kiwi::Vector3d& attenuation, 
				  double range, const Kiwi::Color& color ) :
		Kiwi::Entity( name, scene )
	{
		m_entityType = ENTITY_LIGHT;

		m_lightType = lightType;
		m_renderTarget = renderTarget;
		m_lightColor = color;
		m_attenuation = attenuation;
		m_range = range;

		m_cone = 0.0;
	}

	Light::Light( Kiwi::Scene& scene, std::wstring name, LightType lightType, const std::wstring& renderTarget, const Kiwi::Vector3d& attenuation, 
				  const Kiwi::Vector3d& direction, double range, double cone, const Kiwi::Color& color ) :
		Kiwi::Entity( name, scene )
	{
		m_entityType = ENTITY_LIGHT;

		m_lightType = lightType;
		m_renderTarget = renderTarget;
		m_lightColor = color;
		m_direction = direction;
		m_attenuation = attenuation;
		m_range = range;
		m_cone = cone;
	}

}