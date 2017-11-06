#include "StaticMeshAsset.h"

namespace Kiwi
{

	StaticMeshAsset::StaticMeshAsset( const Kiwi::StaticMeshAsset& other ) :
		Kiwi::IMeshAsset( other.m_assetName, L"Mesh", L"StaticMesh" )
	{
		m_submeshes = other.m_submeshes;
		m_vertices = other.m_vertices;
		m_uvs = other.m_uvs;
		m_normals = other.m_normals;
		m_indices = other.m_indices;
		m_colors = other.m_colors;

		m_assetLoadState = other.m_assetLoadState;
		m_assetCategory = other.m_assetCategory;
		m_assetSubcategory = other.m_assetSubcategory;
		m_assetProperties = other.m_assetProperties;
	}

	StaticMeshAsset::StaticMeshAsset( std::wstring name, std::vector<Kiwi::Submesh> submeshes, std::vector<Kiwi::Vector3d>& vertices, std::vector<Kiwi::Vector2d>& uvs, std::vector<Kiwi::Vector3d>& normals ):
		Kiwi::IMeshAsset( name, L"Mesh", L"StaticMesh" )
	{
		m_submeshes = submeshes;
		m_vertices = vertices;
		m_uvs = uvs;
		m_normals = normals;
	}

	StaticMeshAsset::StaticMeshAsset( std::wstring name, std::vector<Kiwi::Submesh> submeshes, std::vector<Kiwi::Vector3d>& vertices, std::vector<Kiwi::Vector2d>& uvs, std::vector<Kiwi::Vector3d>& normals, const std::vector<unsigned long>& indices ) :
		Kiwi::IMeshAsset( name, L"Mesh", L"StaticMesh" )
	{
		m_submeshes = submeshes;
		m_vertices = vertices;
		m_uvs = uvs;
		m_normals = normals;
		m_indices = indices;
	}

	StaticMeshAsset::StaticMeshAsset( std::wstring name, std::vector<Kiwi::Submesh> submeshes, std::vector<Kiwi::Vector3d>& vertices, std::vector<Kiwi::Vector2d>& uvs, std::vector<Kiwi::Vector3d>& normals, const std::vector<Kiwi::Color>& vertexColors ) :
		Kiwi::IMeshAsset( name, L"Mesh", L"StaticMesh" )
	{
		m_submeshes = submeshes;
		m_vertices = vertices;
		m_uvs = uvs;
		m_normals = normals;
		m_colors = vertexColors;
	}

	StaticMeshAsset::StaticMeshAsset( std::wstring name, std::vector<Kiwi::Submesh> submeshes, std::vector<Kiwi::Vector3d>& vertices, std::vector<Kiwi::Vector2d>& uvs, std::vector<Kiwi::Vector3d>& normals, const std::vector<unsigned long>& indices, const std::vector<Kiwi::Color>& vertexColors ) :
		Kiwi::IMeshAsset( name, L"Mesh", L"StaticMesh" )
	{
		m_submeshes = submeshes;
		m_vertices = vertices;
		m_uvs = uvs;
		m_normals = normals;
		m_indices = indices;
		m_colors = vertexColors;
	}

	StaticMeshAsset::~StaticMeshAsset()
	{
	}

}