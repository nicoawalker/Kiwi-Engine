#ifndef _KIWI_MATERIALIMPORTER_H_
#define _KIWI_MATERIALIMPORTER_H_

#include "Material.h"

#include <string>
#include <memory>
#include <vector>

namespace Kiwi
{

	struct MaterialData
	{
		std::wstring materialName;
		std::wstring shader;

		//path to the material file (excluding the filename)
		std::wstring matPath;

		/*stores the names of the material's textures
		>[0]: diffuse map
		>[1]: bump map
		>[2]: ambient map
		>[3]: specular map*/
		std::vector<std::wstring> textures;

		/*stores the file paths of the material's textures
		>[0]: diffuse map
		>[1]: bump map
		>[2]: ambient map
		>[3]: specular map*/
		std::vector<std::wstring> textureFiles;

		/*stores the material's colors
		>[0]: diffuse
		>[1]: emissive
		>[2]: ambient
		>[3]: specular
		>[4]: transmission filter*/
		std::vector<Kiwi::Color> colors;

		double reflectivity;
		double opticalDensity;
		int illum;

		MaterialData( const std::wstring& name, const std::wstring& path )
		{
			matPath = path;
			materialName = name;

			reflectivity = 0.0;
			opticalDensity = 0.0;
			illum = 0;

			colors.resize( 5 );
			textures.resize( 4 );
		}
	};

	class MaterialImporter
	{
	protected:

		bool _CheckStream( std::wifstream& stream, std::wstring& error );

	public:

		MaterialImporter() {}

		std::wstring Import( std::vector<MaterialData>& _matData, const std::wstring& matFile );

	};

}

#endif