#ifndef _KIWI_MATERIAL_H_
#define _KIWI_MATERIAL_H_

#include "Color.h"

#include "../Core/Component.h"
#include "..\Core\IAssetConsumer.h"

#include "../Events/EventHandling.h"

#include <vector>

namespace Kiwi
{

	class Texture;
	class Mesh;
	class IShaderEffect;
	class IShader;

	class Material:
		public Kiwi::IAsset,
		public Kiwi::IAssetConsumer
	{
	protected:

		long m_uid; //unique id to this material

		Kiwi::IShader* m_shader;

		Kiwi::IShaderEffect* m_shaderEffect;

		std::wstring m_shaderName;
		long m_shaderID; //numerical shader id. this is set automatically by the render queue
		std::wstring m_renderGroup;

		/*stores the material's textures
		>[0]: diffuse map
		>[1]: bump map
		>[2]: ambient map
		>[3]: specular map*/
		std::vector<Kiwi::Texture*> m_textures;

		std::vector<std::wstring> m_textureNames;

		/*stores the material's colors
		>[0]: diffuse
		>[1]: emissive
		>[2]: ambient
		>[3]: specular
		>[4]: transmission filter*/
		std::vector<Kiwi::Color> m_colors;

		double m_reflectivity;
		double m_opticalDensity;
		int m_illum;

		/*type of translucency blending to use when rendering the material
		|-0: opaque
		|-1: normal blending
		|-2: subtractive blending
		|-3: additive blending*/
		char m_translucencyType;

	public:

		enum class TEXTURE_TYPE { DIFFUSE = 0, BUMP = 1, AMBIENT = 2, SPECULAR = 3 };
		enum class COLOR_TYPE { DIFFUSE = 0, EMISSIVE = 1, AMBIENT = 2, SPECULAR = 3, TRANSMISSION_FILTER = 4 };

	protected:

		Kiwi::EventResponse _OnAssetFetched( Kiwi::IAsset* asset );

	public:

		Material();

		Material( const std::vector<std::wstring>& textures );

		Material( const Material& other );

		//creates an empty colored material, initialized to the color white
		Material( const std::wstring& materialName );

		//creates a colored material 
		Material( const std::wstring& materialName, const Kiwi::Color& diffuseColor );

		Material( const std::wstring& materialName, Kiwi::Texture* diffuseMap, Kiwi::Texture* bumpMap = nullptr, Kiwi::Texture* ambientMap = nullptr, Kiwi::Texture* specularMap = nullptr );

		//creates a textured material
		Material( const std::wstring& materialName, const std::vector<std::wstring>& textures, const std::vector<Kiwi::Color>& colors, const std::wstring& shader,
				  double reflectivity, double opticalDensity, int illumination );

		virtual ~Material();

		std::unique_ptr<Kiwi::IAsset> Clone() { return std::make_unique<Kiwi::Material>( *this ); }

		/*sets one of the material's textures*/
		void SetTexture( Material::TEXTURE_TYPE textureType, Kiwi::Texture* texture );

		/*sets the name of one of the material's textures. the material will then retrieve the texture once it becomes available*/
		void SetTexture( Material::TEXTURE_TYPE textureType, const std::wstring& textureName );

		/*sets one of the material's color values, depending on the value of 'colorType'
		possible values are: Diffuse, Ambient, Specular, Emissive, or Transmission*/
		void SetColor( Material::COLOR_TYPE colorType, const Kiwi::Color& color );

		void SetReflectivity( float reflectivity ) { m_reflectivity = reflectivity; }

		void SetOpticalDensity( float opticalDensity ) { m_opticalDensity = opticalDensity; }

		void SetIllumination( int illumination ) { m_illum = illumination; }

		/*sets the alpha value of this material's diffuse color, which stores
		the transparency value for the material*/
		void SetTransparency( float transparency ) { m_colors[0].alpha = transparency; }

		void SetShader( const std::wstring& shader );

		void SetRenderGroup( std::wstring renderGroup ) { m_renderGroup = renderGroup; }

		bool IsTextured()const { return m_textures[0] != 0; }

		bool HasTransparency()const;

		double GetReflectivity()const { return m_reflectivity; }

		double GetOpticalDensity()const { return m_opticalDensity; }

		int GetIllumination()const { return m_illum; }

		Kiwi::IShader* GetShader()const { return m_shader; }

		std::wstring GetShaderName()const { return m_shaderName; }

		long GetShaderID()const { return m_shaderID; }

		std::wstring GetRenderGroup()const { return m_renderGroup; }

		char GetTranslucencyType()const { return m_translucencyType; }

		long GetUID()const { return m_uid; }

		/*returns one of the material's textures, depending on the value of 'textureType'
		possible values are: Diffuse, Ambient, Bump, or Specular*/
		Kiwi::Texture* GetTexture( Material::TEXTURE_TYPE textureType )const;

		/*returns one of the material's color values, depending on the value of 'colorType'
		possible values are: Diffuse, Ambient, Specular, Emissive, or Transmission*/
		const Kiwi::Color& GetColor( Material::COLOR_TYPE colorType )const;

		Material& operator= ( const Material& other );

	};

	typedef std::vector<Kiwi::Material*> MaterialList;

};

#endif