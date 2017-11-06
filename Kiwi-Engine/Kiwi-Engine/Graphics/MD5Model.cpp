#include "MD5Model.h"
#include "GPUBuffer.h"
#include "Renderer.h"

#include "../Core/EngineRoot.h"

namespace Kiwi
{

	MD5Model::MD5Model ( const std::wstring& name, const std::vector<Kiwi::Vertex>& vertices, const std::vector<unsigned long>& indices, const std::vector<Kiwi::MD5Model::MD5Subset>& md5Subsets,
			   const std::vector<Kiwi::MD5Model::Joint>& joints, const std::vector<Kiwi::MD5Model::JointWeight>& weights ) :
		Kiwi::IModel ( name, L"MD5Model" )
	{
		m_uncommitedChanges = std::make_unique<MD5Model::UncommitedChanges>();
		m_uncommitedChanges->vertices = vertices;
		m_uncommitedChanges->indices = indices;
		m_uncommitedChanges->md5Subsets = md5Subsets;
		m_uncommitedChanges->joints = joints;
		m_uncommitedChanges->jointWeights = weights;

		this->CommitChanges();
	}

	MD5Model::MD5Model ( const MD5Model& source ):
		Kiwi::IModel( source )
	{
		m_vertices = source.m_vertices;
		m_indices = source.m_indices;
		m_md5Subsets = source.m_md5Subsets;
		m_joints = source.m_joints;
		m_jointWeights = source.m_jointWeights;
	}

	bool MD5Model::_LinkMesh( Kiwi::Mesh& mesh )
	{
		/*update the model if this is the first mesh that is being linked*/
		bool update = false;
		if( m_linkedMeshes.size() == 0 ) update = true;

		if( !IModel::_LinkMesh( mesh ) ) return false;

		if( update )this->_UpdateModel();

		return true;
	}

	bool MD5Model::_UpdateModel()
	{
		/*no need to do anything if there is nothing to display, it can wait until there are vertices*/
		if( m_vertices.size() == 0 ) return true;

		/*update the subsets that need it*/
		for( Kiwi::MD5Model::MD5Subset& subset : m_md5Subsets )
		{
			if( subset.updateGPUBuffer == false ) continue;

			/*if there are no vertex buffers of the correct size we need to create one to put the new vertices in*/
			if( m_vertexGPUBuffers[subset.vBufIndex] == nullptr || m_vertexGPUBuffers[subset.vBufIndex]->GetCapacity() != m_vertices.size() )
			{
				/*free existing buffer before creating the new one*/
				this->_FreeVertexBuffer( subset.vBufIndex );

				try
				{
					/*create a new vertex buffer. this will throw an exception if creation fails*/
					m_vertexGPUBuffers[subset.vBufIndex] = this->_CreateVertexGPUBuffer( subset.vertexOffset, subset.vertexOffset + subset.numVertices );

				} catch( Kiwi::Exception& e )
				{
					_kConsole.Print( Kiwi::ConsoleMessage( ConsoleMessageType::Error, e ) );
					return false;
				}

				if( m_vertexGPUBuffers[subset.vBufIndex] == nullptr )
				{
					return false;
				}

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

				/*update the vertex buffer with the new vertices.
				this will throw an exception if the update fails*/
				m_vertexGPUBuffers[subset.vBufIndex]->Update( d3dVertices );
			}

			/*if there are no index buffers we need to create one to put the new indices in*/
			if( m_indexGPUBuffers[subset.iBufIndex] == nullptr || m_indexGPUBuffers[subset.iBufIndex]->GetCapacity() != m_indices.size() )
			{
				this->_FreeIndexBuffer( subset.iBufIndex );

				try
				{
					/*create a new index buffer.
					this will throw an exception if creation fails*/
					m_indexGPUBuffers[subset.iBufIndex] = this->_CreateIndexGPUBuffer( subset.indexOffset, subset.indexOffset + subset.numIndices );

				} catch( Kiwi::Exception& e )
				{
					_kConsole.Print( Kiwi::ConsoleMessage( ConsoleMessageType::Error, e ) );
					return false;
				}

				if( m_indexGPUBuffers[subset.iBufIndex] == nullptr )
				{
					return false;
				}

			} else
			{
				/*update the index buffer with the new indices.
				this will throw an exception if the update fails*/
				m_indexGPUBuffers[subset.iBufIndex]->Update( m_indices );
			}
		}

		return true;
	}

	bool MD5Model::Bind( Kiwi::Renderer& renderer, unsigned int subsetIndex )
	{
		if( subsetIndex >= m_md5Subsets.size() ) return false;

		if( m_isShared && m_instanceGPUBuffer != nullptr )
		{
			Kiwi::GPUBuffer<Kiwi::D3DVertex>* vertexBuffer = m_vertexGPUBuffers[m_md5Subsets[subsetIndex].vBufIndex];
			Kiwi::GPUBuffer<unsigned long>* indexBuffer = m_indexGPUBuffers[m_md5Subsets[subsetIndex].iBufIndex];

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

		} else
		{
			Kiwi::GPUBuffer<Kiwi::D3DVertex>* vertexBuffer = m_vertexGPUBuffers[m_md5Subsets[subsetIndex].vBufIndex];
			Kiwi::GPUBuffer<unsigned long>* indexBuffer = m_indexGPUBuffers[m_md5Subsets[subsetIndex].iBufIndex];

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

	void MD5Model::CommitChanges()
	{
		if( m_uncommitedChanges == nullptr ) return;

		if( m_uncommitedChanges->vertices.size() == 0 )
		{
			for( Kiwi::GPUBuffer<Kiwi::D3DVertex>* buffer : m_vertexGPUBuffers )
			{
				//if( buffer != nullptr ) buffer->Free();
			}
			Kiwi::FreeMemory( m_vertexGPUBuffers );
			Kiwi::FreeMemory( m_vertices );

		} else
		{
			m_vertices = m_uncommitedChanges->vertices;
		}

		if( m_uncommitedChanges->indices.size() == 0 )
		{
			for( Kiwi::GPUBuffer<unsigned long>* buffer : m_indexGPUBuffers )
			{
				//if( buffer != nullptr ) buffer->Free();
			}
			Kiwi::FreeMemory( m_indexGPUBuffers );
			Kiwi::FreeMemory( m_indices );

		} else
		{
			m_indices = m_uncommitedChanges->indices;
		}

		if( m_uncommitedChanges->md5Subsets.size() > 0 )
		{
			/*clamp the data indices in all the subsets to be within range and then add them to the subset list*/
			for( Kiwi::MD5Model::MD5Subset& subset : m_uncommitedChanges->md5Subsets )
			{
				if( m_vertices.size() > 0 )
				{
					Kiwi::clamp( subset.vertexOffset, (unsigned int)0, (unsigned int)m_vertices.size() - 1 );
					Kiwi::clamp( subset.numVertices, (unsigned int)0, (unsigned int)m_vertices.size() - subset.vertexOffset );

				} else
				{
					subset.vertexOffset = 0;
					subset.numVertices = 0;
				}

				if( m_indices.size() > 0 )
				{
					Kiwi::clamp( subset.indexOffset, (unsigned int)0, (unsigned int)m_uncommitedChanges->indices.size() - 1 );
					Kiwi::clamp( subset.numIndices, (unsigned int)0, (unsigned int)m_uncommitedChanges->indices.size() - subset.indexOffset );

				} else
				{
					subset.indexOffset = 0;
					subset.numIndices = 0;
				}

				if( m_jointWeights.size() > 0 )
				{
					Kiwi::clamp( subset.weightOffset, (unsigned int)0, (unsigned int)m_jointWeights.size() - 1 );
					Kiwi::clamp( subset.numWeights, (unsigned int)0, (unsigned int)m_jointWeights.size() - subset.weightOffset );

				} else
				{
					subset.weightOffset = 0;
					subset.numWeights = 0;
				}

				//static models only use a single vertex and index buffer
				subset.vBufIndex = 0;
				subset.iBufIndex = 0;

				m_md5Subsets.push_back( subset );
			}
		}

		m_uncommitedChanges.reset( nullptr );

		/*calculate vertex tangents and bitangents*/
		this->_CalculateVectors();

		/*update the gpu buffers of the model.
		will throw an exception if something fails*/
		this->_UpdateModel();
	}

	void MD5Model::Clear()
	{
		/*free all the gpu buffers so they can be garbage collected*/
		for( Kiwi::GPUBuffer<Kiwi::D3DVertex>* buffer : m_vertexGPUBuffers )
		{
			//if( buffer != nullptr ) buffer->Free();
		}

		for( Kiwi::GPUBuffer<unsigned long>* buffer : m_indexGPUBuffers )
		{
			//if( buffer != nullptr ) buffer->Free();
		}

		if( m_instanceGPUBuffer != nullptr ) m_instanceGPUBuffer->Free();

		Kiwi::FreeMemory( m_vertices );
		Kiwi::FreeMemory( m_indices );
		Kiwi::FreeMemory( m_md5Subsets );
		Kiwi::FreeMemory( m_joints );
		Kiwi::FreeMemory( m_jointWeights );
		Kiwi::FreeMemory( m_vertexGPUBuffers );
		Kiwi::FreeMemory( m_indexGPUBuffers );

		m_uncommitedChanges.reset( nullptr );
	}

	std::unique_ptr<Kiwi::IAsset> MD5Model::Clone()
	{
		return std::make_unique<Kiwi::MD5Model>( *this );
	}

	void MD5Model::SetVertices( const std::vector<Kiwi::Vertex>& vertices )
	{
		if( m_uncommitedChanges == nullptr )
		{
			m_uncommitedChanges = std::make_unique<MD5Model::UncommitedChanges>();
		}

		if( m_uncommitedChanges != nullptr )
		{
			m_uncommitedChanges->vertices = vertices;
		}
	}

	void MD5Model::SetIndices( const std::vector<unsigned long>& indices )
	{
		if( m_uncommitedChanges == nullptr )
		{
			m_uncommitedChanges = std::make_unique<MD5Model::UncommitedChanges>();
		}

		if( m_uncommitedChanges != nullptr )
		{
			m_uncommitedChanges->indices = indices;
		}
	}

	void MD5Model::SetJoints( const std::vector<Kiwi::MD5Model::Joint>& joints )
	{
		if( m_uncommitedChanges == nullptr )
		{
			m_uncommitedChanges = std::make_unique<MD5Model::UncommitedChanges>();
		}

		if( m_uncommitedChanges != nullptr )
		{
			m_uncommitedChanges->joints = joints;
		}
	}

	void MD5Model::SetJointWeights( const std::vector<Kiwi::MD5Model::JointWeight>& jointWeights )
	{
		if( m_uncommitedChanges == nullptr )
		{
			m_uncommitedChanges = std::make_unique<MD5Model::UncommitedChanges>();
		}

		if( m_uncommitedChanges != nullptr )
		{
			m_uncommitedChanges->jointWeights = jointWeights;
		}
	}

	void MD5Model::SetSubsets( const std::vector<Kiwi::MD5Model::MD5Subset>& md5Subsets )
	{
		if( m_uncommitedChanges == nullptr )
		{
			m_uncommitedChanges = std::make_unique<MD5Model::UncommitedChanges>();
		}

		if( m_uncommitedChanges != nullptr )
		{
			m_uncommitedChanges->md5Subsets = md5Subsets;
		}
	}

	Kiwi::IModel::Subset* MD5Model::GetSubset( unsigned int subsetIndex )
	{
		return (subsetIndex >= m_md5Subsets.size()) ? nullptr : &m_md5Subsets[subsetIndex];
	}

	unsigned int MD5Model::GetSubsetCount()const
	{
		return (unsigned int)m_md5Subsets.size();
	}


	/*--method definitions for MD5Subset--*/

	MD5Model::MD5Subset::MD5Subset() :
		IModel::Subset()
	{
		weightOffset = 0;
		numWeights = numTriangles = 0;
	}

	MD5Model::MD5Subset::MD5Subset( const std::wstring& material ) :
		IModel::Subset( material )
	{
		weightOffset = 0;
		numWeights = numTriangles = 0;
	}

	MD5Model::MD5Subset::MD5Subset( const std::wstring& material, unsigned int vOffset, unsigned int vCount, unsigned int iOffset, unsigned int iCount,
			   unsigned int vBufferIndex, unsigned int iBufferIndex, unsigned int wOffset, unsigned int wCount, unsigned int tCount ) :
		IModel::Subset( material, vOffset, vCount, iOffset, iCount, vBufferIndex, iBufferIndex )
	{
		weightOffset = wOffset;
		numWeights = wCount;
		numTriangles = tCount;
	}

	MD5Model::MD5Subset::MD5Subset( const Kiwi::MD5Model::MD5Subset& subset ) :
		IModel::Subset( subset )
	{
		weightOffset = subset.weightOffset;
		numWeights = subset.numWeights;
		numTriangles = subset.numTriangles;
	}

	Kiwi::MD5Model::MD5Subset& MD5Model::MD5Subset::operator=( const Kiwi::MD5Model::MD5Subset& subset )
	{
		weightOffset = subset.weightOffset;
		numWeights = subset.numWeights;
		numTriangles = subset.numTriangles;

		vertexOffset = subset.vertexOffset;
		numVertices = subset.numVertices;
		indexOffset = subset.indexOffset;
		numIndices = subset.numIndices;
		vBufIndex = subset.vBufIndex;
		iBufIndex = subset.iBufIndex;

		if( m_material != nullptr ) m_material->Free();

		m_material = subset.m_material;
		m_materialName = subset.m_materialName;

		if( m_material != nullptr )
		{
			m_material->Reserve();

		} else if( m_materialName.size() > 0 )
		{
			this->FetchAsset( L"Material", m_materialName );
		}

		return *this;
	}

}