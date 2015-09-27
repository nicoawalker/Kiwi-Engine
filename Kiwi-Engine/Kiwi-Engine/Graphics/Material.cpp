#include "Material.h"
#include "Mesh.h"

namespace Kiwi
{

	Material::Material()
	{

		m_mesh = 0;
		m_materialName = L"uninitialized";
		
		m_diffuseMap = 0;
		m_bumpMap = 0;
		m_ambientMap = 0;
		m_specularMap = 0;

		m_reflectivity = 0.0f;
		m_opticalDensity = 0.0f;
		m_illum = 0;

		m_diffuseColor = Kiwi::Color( 1.0f, 1.0f, 1.0f, 1.0f );

	}

	Material::Material(std::wstring name, const Kiwi::Color& diffuseColor)
	{

		m_mesh = 0;
		m_materialName = name;
		m_diffuseColor = diffuseColor;

		m_diffuseMap = 0;
		m_bumpMap = 0;
		m_ambientMap = 0;
		m_specularMap = 0;

		m_reflectivity = 0.0f;
		m_opticalDensity = 0.0f;
		m_illum = 0;

	}

	Material::Material(std::wstring name, Kiwi::Texture* diffuseMap, Kiwi::Texture* bumpMap, Kiwi::Texture* ambientMap, Kiwi::Texture* specularMap)
	{

		m_mesh = 0;
		m_materialName = name;

		m_diffuseMap = diffuseMap;
		m_bumpMap = bumpMap;
		m_ambientMap = ambientMap;
		m_specularMap = specularMap;

		m_reflectivity = 0.0f;
		m_opticalDensity = 0.0f;
		m_illum = 0;

		m_diffuseColor = Kiwi::Color( 1.0f, 1.0f, 1.0f, 1.0f );

	}

	Material::~Material()
	{

		m_diffuseMap = 0;
		m_bumpMap = 0;
		m_ambientMap = 0;
		m_specularMap = 0;

		m_reflectivity = 0.0f;
		m_opticalDensity = 0.0f;
		m_illum = 0;

	}

	void Material::SetTexture(std::wstring textureType, Kiwi::Texture* texture)
	{

		if (textureType.compare(L"Diffuse") == 0)
		{
			m_diffuseMap = texture;

		} else if (textureType.compare(L"Ambient") == 0)
		{
			m_ambientMap = texture;

		} else if (textureType.compare(L"Bump") == 0)
		{
			m_bumpMap = texture;

		} else if (textureType.compare(L"Specular") == 0)
		{
			m_specularMap = texture;
		}

	}

	void Material::SetColor(std::wstring colorType, const Kiwi::Color& color)
	{

		if (colorType.compare(L"Diffuse") == 0)
		{
			m_diffuseColor = color;
			if( m_mesh )
			{
				if( m_diffuseColor.alpha != 1.0f )
				{
					m_mesh->m_hasTransparency = true;
				} else
				{
					m_mesh->m_hasTransparency = false;
				}
			}

		} else if (colorType.compare(L"Ambient") == 0)
		{
			m_ambientColor = color;

		} else if (colorType.compare(L"Specular") == 0)
		{
			m_specularColor = color;

		} else if (colorType.compare(L"Emissive") == 0)
		{
			m_emissiveColor = color;

		} else if (colorType.compare(L"Transmission") == 0)
		{
			m_transmissionFilter = color;
		}

	}

	Kiwi::Texture* Material::GetTexture(std::wstring textureType)const
	{

		if (textureType.compare(L"Diffuse") == 0)
		{
			return m_diffuseMap;

		} else if (textureType.compare(L"Ambient") == 0)
		{
			return m_ambientMap;

		} else if (textureType.compare(L"Bump") == 0)
		{
			return m_bumpMap;

		} else if (textureType.compare(L"Specular") == 0)
		{
			return m_specularMap;
		}

		return 0;

	}

	const Kiwi::Color& Material::GetColor(std::wstring colorType)const
	{

		if (colorType.compare(L"Diffuse") == 0)
		{
			return m_diffuseColor;

		} else if (colorType.compare(L"Ambient") == 0)
		{
			return m_ambientColor;

		} else if (colorType.compare(L"Specular") == 0)
		{
			return m_specularColor;

		} else if (colorType.compare(L"Emissive") == 0)
		{
			return m_emissiveColor;

		} else if (colorType.compare(L"TransmissionFilter") == 0)
		{
			return m_transmissionFilter;
		}

		return m_diffuseColor;

	}
	

};