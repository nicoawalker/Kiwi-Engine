#include "StaticModel.h"
#include "Renderer.h"
#include "GPUBuffer.h"

#include "..\Core\EngineRoot.h"
#include "../Core/Entity.h"
#include "..\Core\Scene.h"

namespace Kiwi
{

	StaticModel::StaticModel( const std::wstring& modelName ) :
		Kiwi::IModel( modelName, L"StaticModel" )
	{
		m_vertexGPUBuffers.assign( 1, nullptr );
		m_indexGPUBuffers.assign( 1, nullptr );

		m_recheckTransparency = true;
		m_updateGPUBuffers = false;
	}
	
	StaticModel::StaticModel( const std::wstring& modelName, const std::vector<Kiwi::StaticModel::Subset>& subsets,
				 const std::vector<Kiwi::Vertex>& vertices, const std::vector<unsigned long>& indices ) :
		Kiwi::IModel( modelName, L"StaticModel" )
	{
		m_uncommitedChanges.vertices = vertices;
		m_uncommitedChanges.indices = indices;
		m_uncommitedChanges.subsets = subsets;
		m_uncommitedChanges.subsetsChanged = true;
		m_uncommitedChanges.indicesChanged = true;
		m_uncommitedChanges.verticesChanged = true;
		m_updateGPUBuffers = true;

		m_vertexGPUBuffers.assign( 1, nullptr );
		m_indexGPUBuffers.assign( 1, nullptr );

		m_recheckTransparency = true;

		this->CommitChanges();
	}

	StaticModel::StaticModel( const std::wstring& modelName, MODEL_PRIMITIVE_TYPE primitiveType ):
		Kiwi::IModel( modelName, L"StaticModel" )
	{
		m_vertexGPUBuffers.assign( 1, nullptr );
		m_indexGPUBuffers.assign( 1, nullptr );

		m_updateGPUBuffers = true;
		m_recheckTransparency = true;

		switch( primitiveType )
		{
			case MODEL_PRIMITIVE_TYPE::QUAD:
				{
					double xPos = 0.5;
					double yPos = 0.5;

					std::vector<unsigned long> indices;

					std::vector<Kiwi::Vertex> vertices = {
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, yPos, 0.0 ),
									  Kiwi::Vector2d( 0.0, 0.0 ),
									  Kiwi::Vector3d( 0.0, 0.0, -1.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, yPos, 0.0 ),
									  Kiwi::Vector2d( 1.0, 0.0 ),
									  Kiwi::Vector3d( 0.0, 0.0, -1.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, -yPos, 0.0 ),
									  Kiwi::Vector2d( 0.0, 1.0 ),
									  Kiwi::Vector3d( 0.0, 0.0, -1.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, -yPos, 0.0 ),
									  Kiwi::Vector2d( 0.0, 1.0 ),
									  Kiwi::Vector3d( 0.0, 0.0, -1.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, yPos, 0.0 ),
									  Kiwi::Vector2d( 1.0, 0.0 ),
									  Kiwi::Vector3d( 0.0, 0.0, -1.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, -yPos, 0.0 ),
									  Kiwi::Vector2d( 1.0, 1.0 ),
									  Kiwi::Vector3d( 0.0, 0.0, -1.0 ) )
					};

					for( unsigned int i = 0; i < 6; i++ )
					{
						indices.push_back( i );
					}

					m_uncommitedChanges.vertices = vertices;
					m_uncommitedChanges.indices = indices;
					m_uncommitedChanges.subsets.push_back( Kiwi::IModel::Subset( L"", 0, (unsigned int)vertices.size() - 1, 0, (unsigned int)indices.size() - 1, 0, 0 ) );
					m_uncommitedChanges.indicesChanged = true;
					m_uncommitedChanges.verticesChanged = true;
					m_uncommitedChanges.subsetsChanged = true;

					this->CommitChanges();
				}

			case MODEL_PRIMITIVE_TYPE::CUBE:
				{
					double xPos = 0.5;
					double yPos = 0.5;
					double zPos = 0.5;
					std::vector<unsigned long> indices;

					std::vector<Kiwi::Vertex> vertices = {
						//back face
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, yPos, -zPos ),
										Kiwi::Vector2d( 0.0, 0.0 ),
										Kiwi::Vector3d( 0.0, 0.0, -1.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, yPos, -zPos ),
										Kiwi::Vector2d( 1.0, 0.0 ),
										Kiwi::Vector3d( 0.0, 0.0, -1.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, -yPos, -zPos ),
										Kiwi::Vector2d( 0.0, 1.0 ),
										Kiwi::Vector3d( 0.0, 0.0, -1.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, -yPos, -zPos ),
										Kiwi::Vector2d( 0.0, 1.0 ),
										Kiwi::Vector3d( 0.0, 0.0, -1.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, yPos, -zPos ),
										Kiwi::Vector2d( 1.0, 0.0 ),
										Kiwi::Vector3d( 0.0, 0.0, -1.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, -yPos, -zPos ),
										Kiwi::Vector2d( 1.0, 1.0 ),
										Kiwi::Vector3d( 0.0, 0.0, -1.0 ) ),
						//right face
						Kiwi::Vertex( Kiwi::Vector3d( xPos, yPos, -zPos ),
										Kiwi::Vector2d( 0.0, 0.0 ),
										Kiwi::Vector3d( 1.0, 0.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, yPos, zPos ),
										Kiwi::Vector2d( 1.0, 0.0 ),
										Kiwi::Vector3d( 1.0, 0.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, -yPos, -zPos ),
										Kiwi::Vector2d( 0.0, 1.0 ),
										Kiwi::Vector3d( 1.0, 0.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, -yPos, -zPos ),
										Kiwi::Vector2d( 0.0, 1.0 ),
										Kiwi::Vector3d( 1.0, 0.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, yPos, zPos ),
										Kiwi::Vector2d( 1.0, 0.0 ),
										Kiwi::Vector3d( 1.0, 0.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, -yPos, zPos ),
										Kiwi::Vector2d( 1.0, 1.0 ),
										Kiwi::Vector3d( 1.0, 0.0, 0.0 ) ),
						//front face
						Kiwi::Vertex( Kiwi::Vector3d( xPos, yPos, zPos ),
										Kiwi::Vector2d( 0.0, 0.0 ),
										Kiwi::Vector3d( 0.0, 0.0, 1.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, yPos, zPos ),
										Kiwi::Vector2d( 1.0, 0.0 ),
										Kiwi::Vector3d( 0.0, 0.0, 1.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, -yPos, zPos ),
										Kiwi::Vector2d( 0.0, 1.0 ),
										Kiwi::Vector3d( 0.0, 0.0, 1.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, -yPos, zPos ),
										Kiwi::Vector2d( 0.0, 1.0 ),
										Kiwi::Vector3d( 0.0, 0.0, 1.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, yPos, zPos ),
										Kiwi::Vector2d( 1.0, 0.0 ),
										Kiwi::Vector3d( 0.0, 0.0, 1.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, -yPos, zPos ),
										Kiwi::Vector2d( 1.0, 1.0 ),
										Kiwi::Vector3d( 0.0, 0.0, 1.0 ) ),
						//left face
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, yPos, zPos ),
										Kiwi::Vector2d( 0.0, 0.0 ),
										Kiwi::Vector3d( -1.0, 0.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, yPos, -zPos ),
										Kiwi::Vector2d( 1.0, 0.0 ),
										Kiwi::Vector3d( -1.0, 0.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, -yPos, zPos ),
										Kiwi::Vector2d( 0.0, 1.0 ),
										Kiwi::Vector3d( -1.0, 0.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, -yPos, zPos ),
										Kiwi::Vector2d( 0.0, 1.0 ),
										Kiwi::Vector3d( -1.0, 0.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, yPos, -zPos ),
										Kiwi::Vector2d( 1.0, 0.0 ),
										Kiwi::Vector3d( -1.0, 0.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, -yPos, -zPos ),
										Kiwi::Vector2d( 1.0, 1.0 ),
										Kiwi::Vector3d( -1.0, 0.0, 0.0 ) ),
						//top face
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, yPos, zPos ),
										Kiwi::Vector2d( 0.0, 0.0 ),
										Kiwi::Vector3d( 0.0, 1.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, yPos, zPos ),
										Kiwi::Vector2d( 1.0, 0.0 ),
										Kiwi::Vector3d( 0.0, 1.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, yPos, -zPos ),
										Kiwi::Vector2d( 0.0, 1.0 ),
										Kiwi::Vector3d( 0.0, 1.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, yPos, -zPos ),
										Kiwi::Vector2d( 0.0, 1.0 ),
										Kiwi::Vector3d( 0.0, 1.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, yPos, zPos ),
										Kiwi::Vector2d( 1.0, 0.0 ),
										Kiwi::Vector3d( 0.0, 1.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, yPos, -zPos ),
										Kiwi::Vector2d( 1.0, 1.0 ),
										Kiwi::Vector3d( 0.0, 1.0, 0.0 ) ),
						//bottom face
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, -yPos, -zPos ),
										Kiwi::Vector2d( 0.0, 0.0 ),
										Kiwi::Vector3d( 0.0, -1.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, -yPos, -zPos ),
										Kiwi::Vector2d( 1.0, 0.0 ),
										Kiwi::Vector3d( 0.0, -1.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, -yPos, zPos ),
										Kiwi::Vector2d( 0.0, 1.0 ),
										Kiwi::Vector3d( 0.0, -1.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( -xPos, -yPos, zPos ),
										Kiwi::Vector2d( 0.0, 1.0 ),
										Kiwi::Vector3d( 0.0, -1.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, -yPos, -zPos ),
										Kiwi::Vector2d( 1.0, 0.0 ),
										Kiwi::Vector3d( 0.0, -1.0, 0.0 ) ),
						Kiwi::Vertex( Kiwi::Vector3d( xPos, -yPos, zPos ),
										Kiwi::Vector2d( 1.0, 1.0 ),
										Kiwi::Vector3d( 0.0, -1.0, 0.0 ) )
					};

					for( unsigned int i = 0; i < 36; i++ )
					{
						indices.push_back( i );
					}

					m_uncommitedChanges.vertices = vertices;
					m_uncommitedChanges.indices = indices;
					m_uncommitedChanges.subsets.push_back( Kiwi::IModel::Subset( L"", 0, (unsigned int)vertices.size() - 1, 0, (unsigned int)indices.size() - 1, 0, 0 ) );
					m_uncommitedChanges.indicesChanged = true;
					m_uncommitedChanges.verticesChanged = true;
					m_uncommitedChanges.subsetsChanged = true;

					this->CommitChanges();
				}
			default: break;
		}
	}

	StaticModel::StaticModel( const StaticModel& source ):
		Kiwi::IModel( source )
	{
		m_vertexGPUBuffers.assign( 1, nullptr );
		m_indexGPUBuffers.assign( 1, nullptr );

		m_subsets = source.m_subsets;
		m_updateGPUBuffers = true;
	}

	void StaticModel::_UpdateGPUBuffers()
	{
		if( m_updateGPUBuffers )
		{
			m_updateGPUBuffers = false;

			/*if there are no vertex buffers of the correct size we need to create one to put the new vertices in*/
			if( m_vertexGPUBuffers[0] == nullptr || m_vertexGPUBuffers[0]->GetCapacity() != m_vertices.size() )
			{
				/*free existing buffer before creating the new one*/
				this->_FreeVertexBuffer( 0 );

				/*create a new vertex buffer. this will throw an exception if creation fails*/
				m_vertexGPUBuffers[0] = this->_CreateVertexGPUBuffer( 0, (unsigned int)m_vertices.size() - 1 );

			} else
			{
				/*convert the model's vertices into d3d vertices suitable to send to a shader*/
				std::vector<Kiwi::D3DVertex> d3dVertices( m_vertices.size() );
				for( unsigned int i = 0; i < (unsigned int)d3dVertices.size(); i++ )
				{
					Kiwi::Vector3dToXMFLOAT3( m_vertices[i].position, d3dVertices[i].position );
					Kiwi::Vector3dToXMFLOAT3( m_vertices[i].normal, d3dVertices[i].normal );
					Kiwi::Vector2dToXMFLOAT2( m_vertices[i].tex, d3dVertices[i].tex );
					Kiwi::ColorToXMFLOAT4( m_vertices[i].color, d3dVertices[i].color );
					Kiwi::Vector3dToXMFLOAT3( m_vertices[i].tangent, d3dVertices[i].tangent );
					Kiwi::Vector3dToXMFLOAT3( m_vertices[i].bitangent, d3dVertices[i].bitangent );
				}

				/*update the vertex buffer with the new vertices*/
				m_vertexGPUBuffers[0]->Update( d3dVertices );
			}

			/*if there are no index buffers we need to create one to put the new indices in*/
			if( m_indexGPUBuffers[0] == nullptr || m_indexGPUBuffers[0]->GetCapacity() != m_indices.size() )
			{
				this->_FreeIndexBuffer( 0 );

				/*create a new index buffer*/
				m_indexGPUBuffers[0] = this->_CreateIndexGPUBuffer( 0, (unsigned int)m_indices.size() - 1 );

			} else
			{
				/*update the index buffer with the new indices.
				this will throw an exception if the update fails*/
				m_indexGPUBuffers[0]->Update( m_indices );
			}
		}

		/*must be shared and have linked meshes to require an instance buffer update*/
		if( m_isShared == false || m_linkedMeshes.size() == 0 || m_instanceData.size() == 0 ) return;

		if( m_instanceGPUBuffer == nullptr || m_instanceData.size() > m_instanceGPUBuffer->GetCapacity() )
		{
			this->_InitializeInstanceBuffer();

		} else
		{
			/*update the data inside the existing instance buffer*/
			m_instanceGPUBuffer->Update( m_instanceData, Kiwi::GPUBufferMapOp::DISCARD );
		}
	}

	void StaticModel::AddSubset()
	{
		m_uncommitedChanges.subsets.push_back( Kiwi::IModel::Subset() );
		m_uncommitedChanges.subsetsChanged = true;
	}

	void StaticModel::AddSubset( const Kiwi::IModel::Subset& subset )
	{
		m_uncommitedChanges.subsets.push_back( subset );
		m_uncommitedChanges.subsetsChanged = true;
	}

	bool StaticModel::Bind( Kiwi::Renderer& renderer, unsigned int subsetIndex )
	{
		if( subsetIndex >= m_subsets.size() ) return false;

		/*if the model is shared we need to set both the vertex buffer and the index buffer*/
		if( m_isShared )
		{
			if( m_instanceGPUBuffer != nullptr )
			{
				Kiwi::GPUBuffer<Kiwi::D3DVertex>* vertexBuffer = m_vertexGPUBuffers[0];
				Kiwi::GPUBuffer<unsigned long>* indexBuffer = m_indexGPUBuffers[0];

				if( indexBuffer != nullptr && vertexBuffer != nullptr )
				{
					/*bind the vertex buffer and the instance buffer*/
					std::vector<unsigned int> strides = { sizeof( Vertex ), sizeof( ModelInstanceData ) };
					std::vector<unsigned int> offsets = { 0, 0 };

					std::vector<Kiwi::IGPUBuffer*> vertexBuffers = { vertexBuffer, m_instanceGPUBuffer };

					// set the vertex and instance buffers to active
					renderer.SetVertexBuffers( 0, vertexBuffers, offsets );

					/*bind the index buffer*/
					renderer.SetIndexBuffer( indexBuffer, DXGI_FORMAT_R32_UINT, 0 );

					return true;
				}
			}

		} else
		{
			Kiwi::GPUBuffer<Kiwi::D3DVertex>* vertexBuffer = m_vertexGPUBuffers[0];
			Kiwi::GPUBuffer<unsigned long>* indexBuffer = m_indexGPUBuffers[0];

			if( indexBuffer != nullptr && vertexBuffer != nullptr )
			{
				/*bind the vertex buffer*/
				renderer.SetVertexBuffer( 0, vertexBuffer, 0 );

				/*bind the index buffer*/
				renderer.SetIndexBuffer( indexBuffer, DXGI_FORMAT_R32_UINT, 0 );

				return true;
			}
		}

		return false;
	}

	void StaticModel::CommitChanges()
	{
		if( m_uncommitedChanges.clearModel )
		{
			/*free all the gpu buffers so they can be garbage collected*/
			this->_FreeVertexBuffer( 0 );
			this->_FreeIndexBuffer( 0 );

			if( m_instanceGPUBuffer != nullptr ) m_instanceGPUBuffer->Free();
			m_instanceGPUBuffer = nullptr;

			Kiwi::FreeMemory( m_vertices );
			Kiwi::FreeMemory( m_indices );
			Kiwi::FreeMemory( m_subsets );
		}

		if( m_uncommitedChanges.verticesChanged )
		{
			m_vertices = m_uncommitedChanges.vertices;
			m_updateGPUBuffers = true;
		}

		if( m_uncommitedChanges.indicesChanged )
		{
			m_indices = m_uncommitedChanges.indices;
			m_updateGPUBuffers = true;
		}

		if( m_uncommitedChanges.subsetsChanged )
		{
			m_subsets = m_uncommitedChanges.subsets;
		}

		/*clamp the data in all the subsets to be within range*/
		for( Kiwi::IModel::Subset& subset : m_subsets )
		{
			Kiwi::clamp( subset.vertexOffset, (unsigned int)0, (unsigned int)max( 0, (int)m_vertices.size() - 1 ) );
			Kiwi::clamp( subset.numVertices, (unsigned int)0, (unsigned int)max( 0, (int)(m_vertices.size() - 1) - subset.vertexOffset ) );

			Kiwi::clamp( subset.indexOffset, (unsigned int)0, (unsigned int)max( 0, (int)m_indices.size() - 1 ) );
			Kiwi::clamp( subset.numIndices, (unsigned int)0, (unsigned int)max( 0, (int)(m_indices.size() - 1) - subset.indexOffset ) );

			//static models only use a single vertex and index buffer
			subset.vBufIndex = 0;
			subset.iBufIndex = 0;

			subset.parent = this;
		}	

		m_uncommitedChanges.Reset();

		/*calculate vertex tangents and bitangents*/
		this->_CalculateVectors();
	}

	void StaticModel::Clear ()
	{
		m_uncommitedChanges.clearModel = true;
	}

	std::unique_ptr<Kiwi::IAsset> StaticModel::Clone()
	{
		return std::make_unique<Kiwi::StaticModel>( *this );
	}

	Kiwi::IModel::Subset* StaticModel::GetSubset( unsigned int subsetIndex )
	{
		return (subsetIndex >= m_subsets.size()) ? nullptr : &m_subsets[subsetIndex];
	}

	unsigned int StaticModel::GetSubsetCount()const
	{
		return (unsigned int)m_subsets.size();
	}

	void StaticModel::SetVertices( const std::vector<Kiwi::Vertex>& vertices )
	{
		m_uncommitedChanges.vertices = vertices;
		m_uncommitedChanges.verticesChanged = true;
	}

	void StaticModel::SetIndices( const std::vector<unsigned long>& indices )
	{
		m_uncommitedChanges.indices = indices;
		m_uncommitedChanges.indicesChanged = true;
	}

	void StaticModel::SetSubsets( const std::vector<Kiwi::StaticModel::Subset>& subsets )
	{
		m_uncommitedChanges.subsets = subsets;
		m_uncommitedChanges.subsetsChanged = true;
	}

	bool StaticModel::UsingTransparency()
	{
		if( m_recheckTransparency )
		{
			m_recheckTransparency = false;

			for( const IModel::Subset& subset : m_subsets )
			{
				if( subset.GetMaterial() == nullptr )
				{
					if( subset.GetMaterialName().size() > 0 )
					{
						/*if a subset has a material set, but doesn't yet have a pointer
						 to it, the subset must be checked again later for transparency*/
						m_recheckTransparency = true;
					}
					continue;
				}

				if( subset.GetMaterial()->HasTransparency() )
				{
					/*if any material has transparency, the check is finished*/
					m_hasTransparency = true;
					m_recheckTransparency = false;
					break;
				}
			}
		}

		return m_hasTransparency;
	}

}