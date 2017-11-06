#include "Material.h"
#include "Mesh.h"
#include "Texture.h"
#include "IShader.h"

#include "..\Types.h"

#include "../Core/EngineRoot.h"
#include "..\Core\Utilities.h"

namespace Kiwi
{

	Material::Material() :
		Kiwi::IAsset( L"Mat", L"Material" )
	{
		m_textures.assign( 4, nullptr );
		m_textureNames.resize( 4 );

		m_reflectivity = 0.0f;
		m_opticalDensity = 0.0f;
		m_illum = 0;

		m_colors.resize( 5 );
		m_colors[0] = Kiwi::Color( 1.0f, 0.0f, 1.0f, 1.0f );

		m_shaderEffect = 0;
		m_shader = nullptr;
		m_shaderID = 0;
		m_uid = Kiwi::NewUID();

		m_translucencyType = 0;
	}

	Material::Material( const std::vector<std::wstring>& textures ) :
		Kiwi::IAsset( L"Mat", L"Material" )
	{
		m_textureNames = textures;
		m_textureNames.resize( 4 );

		m_textures.assign( 4, nullptr );

		m_reflectivity = 0;
		m_opticalDensity = 0;
		m_illum = 0;

		m_colors.resize( 5 );
		m_colors[0] = Kiwi::Color( 1.0f, 0.0f, 1.0f, 1.0f );

		m_shaderEffect = 0;
		m_shader = nullptr;
		m_shaderID = 0;
		m_uid = Kiwi::NewUID();

		m_translucencyType = 0;

		/*retrieve the textures, if they are loaded*/
		for( uint i = 0; i < 4; i++ )
		{
			if( m_textureNames[i].length() > 0 && m_textures[i] == nullptr )
			{
				this->FetchAsset( L"Texture", m_textureNames[i] );
			}
		}
	}

	Material::Material( const Material& other ):
		Kiwi::IAsset(other.GetAssetName(), other.GetAssetCategory())
	{
		m_uid = Kiwi::NewUID();
		m_shaderEffect = other.m_shaderEffect;
		m_shaderName = other.m_shaderName;
		m_renderGroup = other.m_renderGroup;
		m_textures = other.m_textures;
		m_textureNames = other.m_textureNames;
		m_colors = other.m_colors;
		m_reflectivity = other.m_reflectivity;
		m_opticalDensity = other.m_opticalDensity;
		m_illum = other.m_illum;
		m_shader = other.m_shader;
		m_shaderID = other.m_shaderID;
		m_translucencyType = other.m_translucencyType;

		if( m_textures.size() < 4 ) m_textures.assign( 4, nullptr );
		if( m_textureNames.size() < 4 ) m_textureNames.resize( 4 );
		if( m_colors.size() < 5 ) m_colors.resize( 5 );

		if( m_shaderName.size() > 0 && m_shader == nullptr )
		{
			this->FetchAsset( L"Shader", m_shaderName );
		}
	}

	Material::Material( const std::wstring& materialName ):
		Kiwi::IAsset( materialName, L"Material" )
	{
		m_textures.assign( 4, nullptr );
		m_textureNames.resize( 4 );

		m_reflectivity = 0.0f;
		m_opticalDensity = 0.0f;
		m_illum = 0;

		m_colors.resize( 5 );
		m_colors[0] = Kiwi::Color( 1.0f, 0.0f, 1.0f, 1.0f );

		m_shaderEffect = 0;
		m_shader = nullptr;
		m_shaderID = 0;
		m_uid = Kiwi::NewUID();
		m_translucencyType = 0;
	}

	Material::Material( const std::wstring& materialName, const Kiwi::Color& diffuseColor ):
		Kiwi::IAsset( materialName, L"Material" )
	{
		m_colors.resize( 5 );
		m_colors[0] = diffuseColor;
		
		m_textures.assign( 4, nullptr );
		m_textureNames.resize( 4 );

		m_reflectivity = 0.0f;
		m_opticalDensity = 0.0f;
		m_illum = 0;

		m_shaderEffect = 0;
		m_shader = nullptr;
		m_shaderID = 0;
		m_uid = Kiwi::NewUID();
		m_translucencyType = 0;
	}

	Material::Material( const std::wstring& materialName, Kiwi::Texture* diffuseMap, Kiwi::Texture* bumpMap, Kiwi::Texture* ambientMap, Kiwi::Texture* specularMap ):
		Kiwi::IAsset( materialName, L"Material" )
	{

		m_textures.assign( 4, nullptr );
		m_textures[0] = diffuseMap;
		m_textures[1] = bumpMap;
		m_textures[2] = ambientMap;
		m_textures[3] = specularMap;

		m_textureNames.resize( 4 );

		if( diffuseMap )
		{
			m_textureNames[0] = diffuseMap->GetAssetName();
			diffuseMap->IncrementReferenceCount();
		}

		if( bumpMap )
		{
			m_textureNames[1] = bumpMap->GetAssetName();
			bumpMap->IncrementReferenceCount();
		}

		if( ambientMap )
		{
			m_textureNames[2] = ambientMap->GetAssetName();
			ambientMap->IncrementReferenceCount();
		}

		if( specularMap )
		{
			m_textureNames[3] = specularMap->GetAssetName();
			specularMap->IncrementReferenceCount();
		}

		m_reflectivity = 0.0f;
		m_opticalDensity = 0.0f;
		m_illum = 0;

		m_colors.resize( 5 );
		m_colors[0] = Kiwi::Color( 1.0f, 0.0f, 1.0f, 1.0f );

		m_shaderEffect = 0;
		m_shader = nullptr;
		m_shaderID = 0;
		m_uid = Kiwi::NewUID();
		m_translucencyType = 0;
	}

	Material::Material( const std::wstring& materialName, const std::vector<std::wstring>& textures, const std::vector<Kiwi::Color>& colors, const std::wstring& shader,
						double reflectivity, double opticalDensity, int illumination ) :
		Kiwi::IAsset( materialName, L"Material" )
	{
		m_textureNames = textures;
		m_textureNames.resize( 4 );

		m_textures.assign( 4, nullptr );

		m_reflectivity = reflectivity;
		m_opticalDensity = opticalDensity;
		m_illum = illumination;

		m_colors = colors;
		m_colors.resize( 5 );

		m_shaderEffect = 0;
		m_shaderName = shader;
		m_shader = nullptr;
		m_shaderID = 0;
		m_uid = Kiwi::NewUID();
		m_translucencyType = 0;

		/*retrieve the textures, if they are loaded*/
		for( uint i = 0; i < 4; i++ )
		{
			if( m_textureNames[i].length() > 0 && m_textures[i] == nullptr )
			{
				this->FetchAsset( L"Texture", m_textureNames[i] );
			}
		}

		if( m_shaderName.size() > 0 )
		{
			this->FetchAsset( L"Shader", m_shaderName );
		}
	}

	Material::~Material()
	{
		for( uint i = 0; i < 4; i++ )
		{
			if( m_textures[i] != nullptr ) m_textures[i]->DecrementReferenceCount();
			m_textures[i] = 0;
		}

		if( m_shader != nullptr ) m_shader->Free();
	}

	Kiwi::EventResponse Material::_OnAssetFetched( Kiwi::IAsset* asset )
	{
		if( asset->GetAssetCategory().compare( L"Texture" ) == 0 )
		{
			for( unsigned int i = 0; i < m_textureNames.size(), i < m_textures.size(); i++ )
			{
				if( asset->GetAssetName().compare( m_textureNames[i] ) == 0 )
				{
					if( m_textures[i] != nullptr ) m_textures[i]->DecrementReferenceCount();

					m_textures[i] = dynamic_cast<Kiwi::Texture*>(asset);
					if( m_textures[i] != nullptr )
					{
						m_textures[i]->IncrementReferenceCount();
						return Kiwi::EventResponse::DISCONNECT_THIS;
					}
				}
			}

		} else if( asset->GetAssetCategory().compare( L"Shader" ) == 0 && asset->GetAssetName().compare( m_shaderName ) == 0 )
		{
			Kiwi::IShader* shader = dynamic_cast<Kiwi::IShader*>(asset);
			if( shader == nullptr )return Kiwi::EventResponse::NONE;

			m_shader = shader;
			m_shaderID = shader->GetUID();
			m_shader->Reserve();
		}

		return Kiwi::EventResponse::NONE;
	}

	void Material::SetTexture( Material::TEXTURE_TYPE textureType, Kiwi::Texture* texture)
	{
		uint typeIndex = (uint)textureType;
		if( typeIndex < 4 )
		{
			if( m_textures[typeIndex] ) m_textures[typeIndex]->DecrementReferenceCount();

			m_textures[typeIndex] = texture;

			if( m_textures[typeIndex] )
			{
				m_textures[typeIndex]->IncrementReferenceCount();
			}
		}
	}

	void Material::SetTexture( Material::TEXTURE_TYPE textureType, const std::wstring& textureName )
	{
		uint typeIndex = (uint)textureType;
		if( typeIndex < 4 )
		{
			m_textureNames[typeIndex] = textureName;

			this->FetchAsset( L"Texture", textureName );
		}
	}

	void Material::SetColor( Material::COLOR_TYPE colorType, const Kiwi::Color& color)
	{
		uint typeIndex = (uint)colorType;
		if( typeIndex < 5 )
		{
			m_colors[typeIndex] = color;
		}
	}

	void Material::SetShader( const std::wstring& shader )
	{
		if( m_shader != nullptr ) m_shader->Free();
		m_shader = nullptr;
		m_shaderID = 0;
		m_shaderName = shader;
		this->FetchAsset( L"Shader", shader );
	}

	bool Material::HasTransparency()const
	{
		if( m_textures[0] != nullptr )
		{
			return m_textures[0]->HasTransparency();

		} else
		{
			return m_colors[0].alpha != 1.0;
		}
	}

	Kiwi::Texture* Material::GetTexture( Material::TEXTURE_TYPE textureType )const
	{
		uint typeIndex = (uint)textureType;
		if( typeIndex < 4 )
		{
			return m_textures[typeIndex];
		}

		return 0;
	}

	const Kiwi::Color& Material::GetColor( Material::COLOR_TYPE colorType )const
	{
		uint typeIndex = (uint)colorType;
		if( typeIndex < 5 )
		{
			return m_colors[typeIndex];
		}

		return m_colors[0];
	}

	Material& Material::operator= ( const Material& other )
	{
		m_uid = Kiwi::NewUID();
		m_shaderEffect = other.m_shaderEffect;
		m_shaderName = other.m_shaderName;
		m_renderGroup = other.m_renderGroup;
		m_textures = other.m_textures;
		m_textureNames = other.m_textureNames;
		m_colors = other.m_colors;
		m_reflectivity = other.m_reflectivity;
		m_opticalDensity = other.m_opticalDensity;
		m_illum = other.m_illum;
		m_translucencyType = other.m_translucencyType;
		m_shader = other.m_shader;
		m_shaderID = other.m_shaderID;

		return *this;
	}

};