#ifndef _KIWI_MESH_H_
#define _KIWI_MESH_H_

#include "DirectX.h"
#include "Material.h"
#include "VertexBuffer.h"

#include "../Core/IAsset.h"
#include "../Core/Vector2.h"
#include "../Core/Vector3.h"

#include <vector>
#include <string>

namespace Kiwi
{

	class IndexBuffer;

	class Mesh :
		public Kiwi::IAsset
	{
	public:

		struct ShaderVertex
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT2 tex;
			DirectX::XMFLOAT3 normal;
		};

		struct Vertex
		{
			Vertex() {}
			Vertex( float x, float y, float z, float u, float v, float nX, float nY, float nZ ): 
				position( x, y, z ), textureUV( u, v ), normals( nX, nY, nZ ) {}
			Vertex( const Kiwi::Vector3& pos, const Kiwi::Vector2& texUV, const Kiwi::Vector3& normal ) :
				position( pos ), textureUV( texUV ), normals( normal ){}
			Kiwi::Vector3 position;
			Kiwi::Vector2 textureUV;
			Kiwi::Vector3 normals;
		};

		struct Subset
		{
			std::vector<Mesh::Vertex> vertices;
			Kiwi::Material material;
			unsigned int startIndex; //stores the position in the vertex buffer of this subset
			unsigned int endIndex;
		};

	protected:

		Kiwi::Renderer* m_renderer;

		std::wstring m_name;

		std::vector<Kiwi::Mesh::Subset> m_subsets;

		Kiwi::VertexBuffer<Kiwi::Mesh::ShaderVertex>* m_vertexBuffer;
		Kiwi::IndexBuffer* m_indexBuffer;

		/*if any subset material uses transparency this will be true*/
		bool m_hasTransparency;
		/*if any subset material has a texture this is true*/
		bool m_hasTexture;

	public:

		/*creates an empty mesh*/
		Mesh( std::wstring name, std::wstring file, Kiwi::Renderer* renderer );

		/*creates a mesh from an array of vertices. the vertices will automatically be placed into a new subset*/
		Mesh( std::wstring name, std::wstring file, Kiwi::Renderer* renderer, std::vector<Kiwi::Mesh::Vertex> vertices );

		//Mesh( std::wstring name, std::wstring file, Kiwi::Renderer* renderer, const Kiwi::MeshSubset& subset );
		Mesh( std::wstring name, std::wstring file, Kiwi::Renderer* renderer, std::vector<Kiwi::Mesh::Subset> subsetList );
		virtual ~Mesh();

		void Bind( Kiwi::Renderer* renderer );

		bool HasTransparency()const { return m_hasTransparency; }
		bool HasTexture()const { return m_hasTexture; }

		//void AddSubset( const Kiwi::MeshSubset& subset );
		//void AddSubsetList( std::vector<Kiwi::MeshSubset> subsetList );
		void SetData(std::vector<Kiwi::Mesh::Subset> meshData);

		void SetSubsetMaterial(unsigned int index, const Kiwi::Material& mat);

		Kiwi::Material* GetSubsetMaterial(unsigned int subset);
		Kiwi::Mesh::Subset* GetSubset(unsigned int subsetIndex);

		std::wstring GetName()const { return m_name; }

		unsigned int GetSubsetCount()const { return (unsigned int)m_subsets.size(); }
		int GetIndexCount()const;
		int GetVertexCount()const;

		Kiwi::IBuffer* GetVertexBuffer();
		Kiwi::IBuffer* GetIndexBuffer();

		/*creates a mesh representing a 1x1x1 cube primitive*/
		static Kiwi::Mesh* Rectangle( std::wstring name, Kiwi::Renderer* renderer, const Kiwi::Vector3& dimensions );

	};
};

#endif