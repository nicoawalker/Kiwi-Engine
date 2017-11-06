#ifndef _KIWI_LIGHT_H_
#define _KIWI_LIGHT_H_

#include "Color.h"

#include "../Core/Entity.h"

namespace Kiwi
{

	enum LightType { DIFFUSE, POINT, SPOTLIGHT, AMBIENT };

	class Light :
		public Kiwi::Entity
	{
	protected:

		LightType m_lightType;

		/*direction of the light. not used in point lights*/
		Kiwi::Vector3d m_direction;

		/*defines how the intensity of light fades when moving away from the light
		Attenuation = att0 + (att1 * d) + (att2 * d²)
		|-d: distance from the light
		|-att0 (constant modifier): amount of light that does not fade (minimum light level at any distance)
		|-att1 (linear modifier): gradually decreases light level in a linear fashion as distance increases
		|-att2 (exponential modifier): rapidly decreases light level in an exponential fashion as distance increases*/
		Kiwi::Vector3d m_attenuation;

		Kiwi::Color m_lightColor;

		std::wstring m_renderTarget;

		/*used in spotlights to define the size of the cone of light, the higher the value the smaller the cone*/
		double m_cone;

		/*maximum range of the light*/
		double m_range;

	public:

		/*constructor for an ambient light*/
		Light( Kiwi::Scene& scene, std::wstring name, LightType lightType, const std::wstring& renderTarget, const Kiwi::Color& color = Kiwi::Color( 1.0, 1.0, 1.0, 1.0 ) );

		/*constructor for a diffuse light*/
		Light( Kiwi::Scene& scene, std::wstring name, LightType lightType, const std::wstring& renderTarget, const Kiwi::Vector3d& direction, const Kiwi::Color& color = Kiwi::Color( 1.0, 1.0, 1.0, 1.0 ) );

		/*constructor for a point light*/
		Light( Kiwi::Scene& scene, std::wstring name, LightType lightType, const std::wstring& renderTarget, const Kiwi::Vector3d& attenuation, double range, const Kiwi::Color& color = Kiwi::Color( 1.0, 1.0, 1.0, 1.0 ) );

		/*constructor for a spotlight*/
		Light( Kiwi::Scene& scene, std::wstring name, LightType lightType, const std::wstring& renderTarget, const Kiwi::Vector3d& attenuation, const Kiwi::Vector3d& direction, double range, double cone, const Kiwi::Color& color = Kiwi::Color( 1.0, 1.0, 1.0, 1.0 ) );

		const Kiwi::Vector3d& GetAttenuation()const { return m_attenuation; }

		const Kiwi::Color& GetColor()const { return m_lightColor; }

		double GetCone()const { return m_cone; }

		const Kiwi::Vector3d& GetDirection()const { return m_direction; }

		double GetRange()const { return m_range; }

		const std::wstring& GetRenderTarget()const { return m_renderTarget; }

		LightType GetLightType()const { return m_lightType; }

		void SetAttenuation( const Kiwi::Vector3d& attenuation ) { m_attenuation = attenuation; }

		void SetColor( const Kiwi::Color& color ) { m_lightColor = color; }

		void SetDirection( const Kiwi::Vector3d& direction ) { m_direction = direction; }

		void SetRange( double range ) { m_range = range; }

		void SetRenderTarget( const std::wstring& rt ) { m_renderTarget = rt; }

	};
}

#endif