#ifndef _KIWIOBJIMPORTER_H_
#define _KIWIOBJIMPORTER_H_

#include "Color.h"

#include "../Core/Vector2.h"
#include "../Core/Vector3.h"
#include "../Core/Vector4.h"

#include <string>
#include <vector>

namespace Kiwi
{

	struct OBJMaterialData
	{
		std::wstring name;
		std::wstring bumpMap;
		std::wstring diffuseMap;
		std::wstring ambientMap;
		std::wstring specularMap;
		Kiwi::Color diffuseColor;
		Kiwi::Color ambientColor;
		Kiwi::Color specularColor;
		Kiwi::Color emissiveColor;
		Kiwi::Color transmissionFilter;
		bool hasTexture;
		float reflectivity;
		float opticalDensity;
		int illum;
	};

	struct OBJFace
	{
		float v1, t1, n1;
		float v2, t2, n2;
		float v3, t3, n3;
	};

	struct OBJRawData
	{
		OBJMaterialData mData;
		std::vector<OBJFace> faces;
		std::vector<Kiwi::Vector3> vertices;
		std::vector<Kiwi::Vector3> normals;
		std::vector<Kiwi::Vector3> texCoords;
	};

	struct OBJVertex
	{
		Kiwi::Vector3 position;
		Kiwi::Vector3 normals;
		Kiwi::Vector2 textureUV;
	};

	struct OBJVertexGroup
	{
		OBJMaterialData mData;
		std::vector<OBJVertex> vertices;
	};

	struct OBJMeshData
	{
		std::vector<OBJVertexGroup> vGroups;
	};

	class OBJImporter
	{
	private:

		std::wstring m_errorString;


	private:

		void _ConvertFaces(std::vector<OBJRawData>& faceData, std::vector<OBJVertexGroup>& vertexGroups);

	public:

		OBJImporter(){}
		~OBJImporter(){}

		OBJMeshData* Import(std::wstring objFile);

	};

};

#endif