#include "Mesh.h"
#include "IndexBuffer.h"
#include "Renderer.h"

#include "../Core/Utilities.h"

namespace Kiwi
{

	Mesh::Mesh( std::wstring name, std::wstring file, Kiwi::Renderer* renderer ):
		IAsset(name, L"StaticMesh")
	{

		m_name = name;
		m_renderer = renderer;
		m_vertexBuffer = 0;
		m_indexBuffer = 0;
		m_hasTransparency = false;
		m_hasTexture = false;
		m_assetFiles.push_back( file );

	}

	Mesh::Mesh( std::wstring name, std::wstring file, Kiwi::Renderer* renderer, std::vector<Kiwi::Mesh::Vertex> vertices ):
		IAsset(name, L"StaticMesh")
	{

		try
		{
			m_name = name;
			m_renderer = renderer;
			m_vertexBuffer = 0;
			m_indexBuffer = 0;
			m_hasTransparency = false;
			m_hasTexture = false;
			m_assetFiles.push_back( file );

			Kiwi::Mesh::Subset subset;
			subset.vertices = vertices;

			m_subsets.push_back(subset);

			long vertexCount = 0;
			for( const Kiwi::Mesh::Subset subset : m_subsets )
			{
				vertexCount += (long)subset.vertices.size();
			}

			//create the dynamic vertex and index buffers
			m_vertexBuffer = new Kiwi::VertexBuffer<Kiwi::Mesh::ShaderVertex>( m_name + L"_VB", renderer, vertexCount );
			m_indexBuffer = new Kiwi::IndexBuffer( m_name + L"_IB", renderer, vertexCount );

			//convert the mesh subset data into an array of ShaderVertex structs and also
			//generate the indices for the index buffer
			unsigned long index = 0;
			std::vector<Kiwi::Mesh::ShaderVertex> shaderVerts;
			std::vector<unsigned long> indices;
			for( Kiwi::Mesh::Subset& subset : m_subsets )
			{

				//check for transparency
				if( subset.material.GetTransparency() != 0.0f ) m_hasTransparency = true;
				//check for a texture
				if( subset.material.IsTextured() ) m_hasTexture = true;

				subset.startIndex = index;

				//generate ShaderVertex structs from the vertices in the subset
				for( const Kiwi::Mesh::Vertex& vertex : subset.vertices )
				{
					Kiwi::Mesh::ShaderVertex vert;
					vert.position = DirectX::XMFLOAT3( vertex.position.x, vertex.position.y, vertex.position.z );
					vert.tex = DirectX::XMFLOAT2( vertex.textureUV.x, vertex.textureUV.y );
					vert.normal = DirectX::XMFLOAT3( vertex.normals.x, vertex.normals.y, vertex.normals.z );
					shaderVerts.push_back( vert );
					indices.push_back( index );
					index++;
				}

				subset.endIndex = index;
			}

			//copy the data into the buffers
			m_vertexBuffer->SetData( shaderVerts );
			m_indexBuffer->SetData( indices );

		} catch( ... )
		{
			throw;
		}

	}

	Mesh::Mesh(std::wstring name, std::wstring file, Kiwi::Renderer* renderer, std::vector<Kiwi::Mesh::Subset> meshData):
		IAsset(name, L"StaticMesh")
	{

		try
		{
			m_name = name;
			m_renderer = renderer;
			m_vertexBuffer = 0;
			m_indexBuffer = 0;
			m_hasTransparency = false;
			m_hasTexture = false;
			m_assetFiles.push_back( file );

			m_subsets = meshData;

			long vertexCount = 0;
			for( const Kiwi::Mesh::Subset subset : m_subsets )
			{
				vertexCount += (long)subset.vertices.size();
			}

			//create the dynamic vertex and index buffers
			m_vertexBuffer = new Kiwi::VertexBuffer<Kiwi::Mesh::ShaderVertex>( m_name + L"_VB", renderer, vertexCount );
			m_indexBuffer = new Kiwi::IndexBuffer( m_name + L"_IB", renderer, vertexCount );

			//convert the mesh subset data into an array of ShaderVertex structs and also
			//generate the indices for the index buffer
			unsigned long index = 0;
			std::vector<Kiwi::Mesh::ShaderVertex> shaderVerts;
			std::vector<unsigned long> indices;
			for ( Kiwi::Mesh::Subset& subset : m_subsets )
			{

				//check for transparency
				if(subset.material.GetTransparency() != 0.0f) m_hasTransparency = true;
				//check for a texture
				if( subset.material.IsTextured() ) m_hasTexture = true;

				subset.startIndex = index;

				//generate ShaderVertex structs from the vertices in the subset
				for ( const Kiwi::Mesh::Vertex& vertex : subset.vertices )
				{
					Kiwi::Mesh::ShaderVertex vert;
					vert.position = DirectX::XMFLOAT3(vertex.position.x, vertex.position.y, vertex.position.z );
					vert.tex = DirectX::XMFLOAT2(vertex.textureUV.x, vertex.textureUV.y);
					vert.normal = DirectX::XMFLOAT3(vertex.normals.x, vertex.normals.y, vertex.normals.z);
					shaderVerts.push_back(vert);
					indices.push_back(index);
					index++;
				}

				subset.endIndex = index;
			}

			//copy the data into the buffers
			m_vertexBuffer->SetData(shaderVerts);
			m_indexBuffer->SetData(indices);

		}catch (...)
		{
			throw;
		}

	}

	Mesh::~Mesh()
	{

		Kiwi::FreeMemory( m_assetFiles );
		Kiwi::FreeMemory(m_subsets);

		SAFE_DELETE(m_vertexBuffer);
		SAFE_DELETE(m_indexBuffer);

	}

	void Mesh::Bind( Kiwi::Renderer* renderer )
	{

		if( renderer == 0 || m_vertexBuffer == 0 || m_indexBuffer == 0 ) return;

		unsigned int stride = sizeof( ShaderVertex );
		unsigned int offset = 0;

		ID3D11DeviceContext* deviceCon = renderer->GetDeviceContext();

		// set the vertex buffer to active so it can be rendered
		ID3D11Buffer* vBuffer = m_vertexBuffer->GetD3DBuffer();
		deviceCon->IASetVertexBuffers( 0, 1, &vBuffer, &stride, &offset );
		// same for index buffer
		ID3D11Buffer* iBuffer = m_indexBuffer->GetD3DBuffer();
		deviceCon->IASetIndexBuffer( iBuffer, DXGI_FORMAT_R32_UINT, 0 );

	}

	void Mesh::SetData(std::vector<Kiwi::Mesh::Subset> meshData)
	{

		//convert the mesh subset data into an array of ShaderVertex structs and also
		//generate the indices for the index buffer
		unsigned long index = 0;
		std::vector<Kiwi::Mesh::ShaderVertex> shaderVerts;
		std::vector<unsigned long> indices;
		for ( Kiwi::Mesh::Subset& subset : meshData)
		{

			//check for transparency
			if (subset.material.GetTransparency() != 0.0f) m_hasTransparency = true;

			subset.startIndex = index;

			//generate ShaderVertex structs from the vertices in the subset
			for (const Kiwi::Mesh::Vertex& vertex : subset.vertices)
			{
				Kiwi::Mesh::ShaderVertex vert;
				vert.position = DirectX::XMFLOAT3(vertex.position.x, vertex.position.y, vertex.position.z );
				vert.tex = DirectX::XMFLOAT2(vertex.textureUV.x, vertex.textureUV.y);
				vert.normal = DirectX::XMFLOAT3(vertex.normals.x, vertex.normals.y, vertex.normals.z);
				shaderVerts.push_back(vert);
				indices.push_back(index);
				index++;
			}

			subset.endIndex = index;
		}

		try
		{
			if( !m_vertexBuffer ) m_vertexBuffer = new Kiwi::VertexBuffer<Kiwi::Mesh::ShaderVertex>( m_name + L"_VB", m_renderer, index );
			if( !m_indexBuffer ) m_indexBuffer = new Kiwi::IndexBuffer( m_name + L"_IB", m_renderer, index );

			//copy the data into the buffers
			m_vertexBuffer->SetData( shaderVerts );
			m_indexBuffer->SetData( indices );

			//copied successfully, get rid of the old mesh data
			Kiwi::FreeMemory(m_subsets);

			m_subsets = meshData;

		} catch (...)
		{
			throw;
		}

	}

	void Mesh::SetSubsetMaterial(unsigned int index, const Kiwi::Material& newMaterial)
	{

		if (index >= m_subsets.size()) return;

		//set the new material
		m_subsets[index].material = newMaterial;

	}

	Kiwi::Material* Mesh::GetSubsetMaterial(unsigned int subset)
	{

		if (subset >= m_subsets.size()) return 0;

		return &m_subsets[subset].material;

	}

	Kiwi::Mesh::Subset* Mesh::GetSubset(unsigned int subset)
	{

		if (subset >= m_subsets.size()) return 0;

		return &m_subsets[subset];

	}

	int Mesh::GetIndexCount()const
	{

		if (m_indexBuffer == 0) return 0;

		return m_indexBuffer->GetElementCount();

	}

	int Mesh::GetVertexCount()const
	{

		if (m_vertexBuffer == 0) return 0;

		return m_vertexBuffer->GetElementCount();

	}

	Kiwi::IBuffer* Mesh::GetVertexBuffer()
	{

		return m_vertexBuffer;

	}

	Kiwi::IBuffer* Mesh::GetIndexBuffer()
	{

		return m_indexBuffer;

	}


	//----Static member functions

	Kiwi::Mesh* Mesh::Cube( std::wstring name, Kiwi::Renderer* renderer )
	{

		std::vector<Kiwi::Mesh::Vertex> meshVertices = {
			Kiwi::Mesh::Vertex( -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f ),
			Kiwi::Mesh::Vertex( 0.5f, 0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  0.0f, -0.5f ),
			Kiwi::Mesh::Vertex( -0.5f, -0.5f, -0.5f, 0.0f, 0.5f,  0.0f,  0.0f, -0.5f ),
			Kiwi::Mesh::Vertex( -0.5f, -0.5f, -0.5f, 0.0f, 0.5f,  0.0f,  0.0f, -0.5f ),
			Kiwi::Mesh::Vertex( 0.5f,  0.5f, -0.5f, 0.5f, 0.0f,  0.0f,  0.0f, -0.5f ),
			Kiwi::Mesh::Vertex( 0.5f, -0.5f, -0.5f, 0.5f, 0.5f,  0.0f,  0.0f, -0.5f ),
			Kiwi::Mesh::Vertex( 0.5f,  0.5f, -0.5f, 0.0f, 0.0f,  0.5f,  0.0f,  0.0f ),
			Kiwi::Mesh::Vertex( 0.5f,  0.5f,  0.5f, 0.5f, 0.0f,  0.5f,  0.0f,  0.0f ),
			Kiwi::Mesh::Vertex( 0.5f, -0.5f, -0.5f, 0.0f, 0.5f,  0.5f,  0.0f,  0.0f ),
			Kiwi::Mesh::Vertex( 0.5f, -0.5f, -0.5f, 0.0f, 0.5f,  0.5f,  0.0f,  0.0f ),
			Kiwi::Mesh::Vertex( 0.5f,  0.5f,  0.5f, 0.5f, 0.0f,  0.5f,  0.0f,  0.0f ),
			Kiwi::Mesh::Vertex( 0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  0.5f,  0.0f,  0.0f ),
			Kiwi::Mesh::Vertex( 0.5f,  0.5f,  0.5f, 0.0f, 0.0f , 0.0f,  0.0f , 0.5f ),
			Kiwi::Mesh::Vertex( -0.5f,  0.5f  ,0.5f ,0.5f ,0.0f  ,0.0f , 0.0f,  0.5f ),
			Kiwi::Mesh::Vertex( 0.5f, -0.5f,  0.5f ,0.0f, 0.5f,  0.0f,  0.0f, 0.5f ),
			Kiwi::Mesh::Vertex( 0.5f, -0.5f,  0.5f, 0.0f ,0.5f, 0.0f , 0.0f, 0.5f ),
			Kiwi::Mesh::Vertex( -0.5f,  0.5f , 0.5f ,0.5f ,0.0f , 0.0f , 0.0f, 0.5f ),
			Kiwi::Mesh::Vertex( -0.5f, -0.5f , 0.5f, 0.5f ,0.5f , 0.0f,  0.0f, 0.5f ),
			Kiwi::Mesh::Vertex( -0.5f , 0.5f  ,0.5f, 0.0f ,0.0f ,-0.5f , 0.0f, 0.0f ),
			Kiwi::Mesh::Vertex( -0.5f , 0.5f, -0.5f, 0.5f, 0.0f ,-0.5f , 0.0f,  0.0f ),
			Kiwi::Mesh::Vertex( -0.5f, -0.5f , 0.5f, 0.0f ,0.5f, -0.5f , 0.0f, 0.0f ),
			Kiwi::Mesh::Vertex( -0.5f, -0.5f,  0.5f ,0.0f ,0.5f, -0.5f , 0.0f, 0.0f ),
			Kiwi::Mesh::Vertex( -0.5f,  0.5f, -0.5f, 0.5f ,0.0f, -0.5f , 0.0f, 0.0f ),
			Kiwi::Mesh::Vertex( -0.5f ,-0.5f, -0.5f ,0.5f ,0.5f ,-0.5f, 0.0f, 0.0f ),
			Kiwi::Mesh::Vertex( -0.5f,  0.5f,  0.5f ,0.0f, 0.0f , 0.0f,  0.5f, 0.0f ),
			Kiwi::Mesh::Vertex( 0.5f,  0.5f,  0.5f, 0.5f, 0.0f , 0.0f,  0.5f, 0.0f ),
			Kiwi::Mesh::Vertex( -0.5f,  0.5f, -0.5f, 0.0f, 0.5f,  0.0f , 0.5f, 0.0f ),
			Kiwi::Mesh::Vertex( -0.5f , 0.5f ,-0.5f, 0.0f ,0.5f,  0.0f , 0.5f, 0.0f ),
			Kiwi::Mesh::Vertex( 0.5f, 0.5f , 0.5f, 0.5f, 0.0f , 0.0f , 0.5f, 0.0f ),
			Kiwi::Mesh::Vertex( 0.5f  ,0.5f, -0.5f, 0.5f, 0.5f , 0.0f , 0.5f, 0.0f ),
			Kiwi::Mesh::Vertex( -0.5f ,-0.5f ,-0.5f, 0.0f, 0.0f,  0.0f ,-0.5f, 0.0f ),
			Kiwi::Mesh::Vertex( 0.5f ,-0.5f, -0.5f, 0.5f, 0.0f,  0.0f ,-0.5f, 0.0f ),
			Kiwi::Mesh::Vertex( -0.5f, -0.5f,  0.5f, 0.0f ,0.5f,  0.0f ,-0.5f, 0.0f ),
			Kiwi::Mesh::Vertex( -0.5f ,-0.5f,  0.5f, 0.0f, 0.5f,  0.0f ,-0.5f, 0.0f ),
			Kiwi::Mesh::Vertex( 0.5f, -0.5f, -0.5f, 0.5f, 0.0f,  0.0f ,-0.5f, 0.0f ),
			Kiwi::Mesh::Vertex( 0.5f ,-0.5f , 0.5f, 0.5f, 0.5f,  0.0f, -0.5f, 0.0f )
		};

		Kiwi::Mesh* cubeMesh = new Kiwi::Mesh( name, L"Cube_Primitive", renderer, meshVertices );

		return cubeMesh;

	}

};