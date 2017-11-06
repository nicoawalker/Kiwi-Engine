#include "IModel.h"
#include "GPUBufferManager.h"
#include "GPUBuffer.h"
#include "Renderer.h"
#include "Viewport.h"
#include "Camera.h"

#include "../Core/Scene.h"
#include "..\Core\EngineRoot.h"

#include <algorithm>

namespace Kiwi
{

	IModel::IModel( const std::wstring& modelName, const std::wstring& modelType ) :
		Kiwi::IAsset( modelName, L"Model", modelType )
	{
		m_isShared = false;
		m_hasTransparency = false;
		m_calculateTangents = true;
		m_calculateNormals = true;
		m_instanceGPUBuffer = nullptr;
		m_primitiveTopology = Kiwi::PrimitiveTopology::TRIANGLE_LIST;	}

	IModel::IModel( const Kiwi::IModel& source ) :
		Kiwi::IAsset( source )
	{
		m_vertices = source.m_vertices;
		m_indices = source.m_indices;
		m_isShared = source.m_isShared;
		m_calculateTangents = source.m_calculateTangents;
		m_calculateNormals = source.m_calculateNormals;
		m_hasTransparency = source.m_hasTransparency;
		m_primitiveTopology = source.m_primitiveTopology;
		m_instanceGPUBuffer = nullptr;
	}

	IModel::~IModel()
	{
		/*free all the gpu buffers so they can be garbage collected*/
		for( Kiwi::GPUBuffer<Kiwi::D3DVertex>* buffer : m_vertexGPUBuffers )
		{
			if( buffer != nullptr ) buffer->Free();
		}

		for( Kiwi::GPUBuffer<unsigned long>* buffer : m_indexGPUBuffers )
		{
			if( buffer != nullptr ) buffer->Free();
		}

		if( m_instanceGPUBuffer != nullptr ) m_instanceGPUBuffer->Free();
	}

	void IModel::_CalculateVectors()
	{
		if( !m_calculateNormals && !m_calculateTangents ) return;

		/*http://www.rastertek.com/dx11tut20.html*/

		/*calculate tangent and bitangent for each face of the model*/
		for( unsigned int i = 0; i + 2 < (unsigned int)m_vertices.size(); i += 3 )
		{
			Kiwi::Vector3d tangent, bitangent;

			Kiwi::Vertex A = m_vertices[i];
			Kiwi::Vertex B = m_vertices[i + 1];
			Kiwi::Vertex C = m_vertices[i + 2];

			if( m_calculateTangents )
			{
				Kiwi::Vector3d tangent, bitangent;

				this->_CalculateTangents( tangent, bitangent, A, B, C );

				m_vertices[i].tangent = tangent;
				m_vertices[i + 1].tangent = tangent;
				m_vertices[i + 2].tangent = tangent;

				m_vertices[i].bitangent = bitangent;
				m_vertices[i + 1].bitangent = bitangent;
				m_vertices[i + 2].bitangent = bitangent;

				if( m_calculateNormals )
				{
					Kiwi::Vector3d normal = tangent.Cross( bitangent ).Normalized();
					m_vertices[i].normal = normal;
					m_vertices[i + 1].normal = normal;
					m_vertices[i + 2].normal = normal;
				}

			} else if( m_calculateNormals )
			{
				Kiwi::Vector3d normal;
			}
		}
	}

	void IModel::_CalculateNormal( Kiwi::Vector3d& _normal, const Kiwi::Vector3d& tangent, const Kiwi::Vector3d& bitangent )
	{

	}

	void IModel::_CalculateTangents( Kiwi::Vector3d& _tangent, Kiwi::Vector3d& _bitangent, const Kiwi::Vertex& A,
									 const Kiwi::Vertex& B, const Kiwi::Vertex& C )
	{
		/*http://www.rastertek.com/dx11tut20.html*/

		Kiwi::Vector3d edgeBA( B.position.x - A.position.x, B.position.y - A.position.y, B.position.z - A.position.z );
		Kiwi::Vector3d edgeCA( C.position.x - A.position.x, C.position.y - A.position.y, C.position.z - A.position.z );
		Kiwi::Vector2d texBA( B.tex.x - A.tex.x, B.tex.y - A.tex.y );
		Kiwi::Vector2d texCA( C.tex.x - A.tex.x, C.tex.y - A.tex.y );

		/*calculate face area*/
		double area = texBA.x * texCA.y - texCA.x * texBA.y;

		if( area != 0.0 )
		{
			double delta = 1.0 / area;

			/*calculate face tangent to current triangle*/
			_tangent.x = delta * ((edgeBA.x * texCA.y) - (edgeCA.x * texBA.y));
			_tangent.y = delta * ((edgeBA.y * texCA.y) - (edgeCA.y * texBA.y));
			_tangent.z = delta * ((edgeBA.z * texCA.y) - (edgeCA.z * texBA.y));

			_tangent = _tangent.Normalized();

			/*--calculate bitangent--*/
			_bitangent.x = delta * ((texBA.x * edgeCA.x) - (texCA.x * edgeBA.x));
			_bitangent.y = delta * ((texBA.x * edgeCA.y) - (texCA.x * edgeBA.y));
			_bitangent.z = delta * ((texBA.x * edgeCA.z) - (texCA.x * edgeBA.z));

			_bitangent = _bitangent.Normalized();
		}
	}

	//void IModel::_CopySharedData()
	//{
	//	/*must be shared and have linked meshes to require an instance buffer update*/
	//	if( m_instanceGPUBuffer == nullptr || m_isShared == false ||
	//		m_linkedMeshes.size() == 0 || m_instanceData.size() == 0 ) return;

	//	if( m_instanceData.size() > m_instanceGPUBuffer->GetCapacity() )
	//	{
	//		this->_InitializeInstanceBuffer();

	//	} else
	//	{
	//		/*update the data inside the existing instance buffer*/
	//		m_instanceGPUBuffer->Update( m_instanceData, Kiwi::GPUBufferMapOp::DISCARD );
	//	}
	//}

	Kiwi::GPUBuffer<Kiwi::D3DVertex>* IModel::_CreateVertexGPUBuffer( unsigned int startIndex, unsigned int endIndex )
	{
		/*don't try to create an empty buffer*/
		if( m_vertices.size() == 0 ) return nullptr;

		Kiwi::Renderer* renderer = _kGraphics.GetActiveRenderer();
		if( renderer == nullptr ) return nullptr;

		Kiwi::clamp( startIndex, (unsigned int)0, (unsigned int)m_vertices.size() - 1 );
		Kiwi::clamp( endIndex, startIndex, (unsigned int)m_vertices.size() - 1 );

		if( startIndex >= endIndex ) return nullptr;

		std::vector<Kiwi::Vertex> subArray;
		subArray.insert( subArray.end(), m_vertices.begin() + startIndex, m_vertices.begin() + endIndex );

		/*convert the model's vertices into d3d vertices to place in the gpu buffer*/
		std::vector<Kiwi::D3DVertex> d3dVertices( subArray.size() );
		for( unsigned int i = 0; i < (unsigned int)d3dVertices.size(); i++ )
		{
			Kiwi::Vector3dToXMFLOAT3( m_vertices[i].position, d3dVertices[i].position );
			Kiwi::Vector3dToXMFLOAT3( m_vertices[i].normal, d3dVertices[i].normal );
			Kiwi::Vector2dToXMFLOAT2( m_vertices[i].tex, d3dVertices[i].tex );
			Kiwi::ColorToXMFLOAT4( m_vertices[i].color, d3dVertices[i].color );
			Kiwi::Vector3dToXMFLOAT3( m_vertices[i].tangent, d3dVertices[i].tangent );
			Kiwi::Vector3dToXMFLOAT3( m_vertices[i].bitangent, d3dVertices[i].bitangent );
		}

		Kiwi::GPUBuffer<Kiwi::D3DVertex>* vertexBuffer = nullptr;

		try
		{
			/*use the renderer to create a new gpu vertex buffer*/
			vertexBuffer = renderer->GetGPUBufferManager().CreateBuffer<Kiwi::D3DVertex>( d3dVertices, (long)d3dVertices.size(), Kiwi::GPUBufferType::VERTEX_BUFFER, Kiwi::GPUBufferUsageFlags::DYNAMIC, Kiwi::GPUBufferAccessFlags::CPU_WRITE );

		} catch( Kiwi::Exception& e )
		{
			_kConsole.Print( Kiwi::ConsoleMessage( ConsoleMessageType::Error, L"[" + m_assetName + L"] Failed to create vertex buffer: " + e.GetError() ) );
			return nullptr;
		}
		
		vertexBuffer->Reserve();

		return vertexBuffer;
	}

	Kiwi::GPUBuffer<unsigned long>* IModel::_CreateIndexGPUBuffer( unsigned int startIndex, unsigned int endIndex )
	{
		/*don't try to create an empty buffer*/
		if( m_indices.size() == 0 ) return nullptr;

		Kiwi::Renderer* renderer = _kGraphics.GetActiveRenderer();
		if( renderer == nullptr ) return nullptr;

		Kiwi::clamp( startIndex, (unsigned int)0, (unsigned int)m_indices.size() - 1 );
		Kiwi::clamp( endIndex, startIndex, (unsigned int)m_indices.size() - 1 );

		if( startIndex >= endIndex ) return nullptr;

		std::vector<unsigned long> subArray;
		subArray.insert( subArray.end(), m_indices.begin() + startIndex, m_indices.begin() + endIndex );

		Kiwi::GPUBuffer<unsigned long>* indexBuffer = nullptr;

		try
		{
			/*use the renderer to create a new gpu vertex buffer*/
			indexBuffer = renderer->GetGPUBufferManager().CreateBuffer<unsigned long>( subArray, (long)subArray.size(), Kiwi::GPUBufferType::INDEX_BUFFER, Kiwi::GPUBufferUsageFlags::DYNAMIC, Kiwi::GPUBufferAccessFlags::CPU_WRITE );

		} catch( Kiwi::Exception& e )
		{
			_kConsole.Print( Kiwi::ConsoleMessage( ConsoleMessageType::Error, L"[" + m_assetName + L"] Failed to create index buffer: " + e.GetError() ) );
			return nullptr;
		}
		
		indexBuffer->Reserve();

		return indexBuffer;
	}

	void IModel::_FreeVertexBuffer( unsigned int bufferIndex )
	{
		if( bufferIndex < m_vertexGPUBuffers.size() )
		{
			if( m_vertexGPUBuffers[bufferIndex] != nullptr ) m_vertexGPUBuffers[bufferIndex]->Free();

			m_vertexGPUBuffers[bufferIndex] = nullptr;
		}
	}

	void IModel::_FreeIndexBuffer( unsigned int bufferIndex )
	{
		if( bufferIndex < m_indexGPUBuffers.size() )
		{
			if( m_indexGPUBuffers[bufferIndex] != nullptr ) m_indexGPUBuffers[bufferIndex]->Free();

			m_indexGPUBuffers[bufferIndex] = nullptr;
		}
	}

	void IModel::_FreeInstanceBuffer()
	{
		if( m_instanceGPUBuffer != nullptr ) m_instanceGPUBuffer->Free();
		m_instanceGPUBuffer = nullptr;
	}

	void IModel::_InitializeInstanceBuffer()
	{
		if( m_instanceGPUBuffer == nullptr || m_instanceGPUBuffer->GetCapacity() < m_instanceData.size() )
		{
			if( m_instanceGPUBuffer != nullptr ) this->_FreeInstanceBuffer();

			Kiwi::Renderer* renderer = _kGraphics.GetActiveRenderer();
			if( renderer == nullptr ) return;

			try
			{
				if( m_instanceData.size() > 0 )
				{
					/*create a new instance buffer and insert the instance data at the same time*/
					m_instanceGPUBuffer = renderer->GetGPUBufferManager().CreateBuffer<Kiwi::IModel::ModelInstanceData>( m_instanceData, (unsigned long)m_instanceData.size(), Kiwi::GPUBufferType::VERTEX_BUFFER, Kiwi::GPUBufferUsageFlags::DYNAMIC, Kiwi::GPUBufferAccessFlags::CPU_WRITE );

				} else
				{
					m_instanceGPUBuffer = renderer->GetGPUBufferManager().CreateBuffer<Kiwi::IModel::ModelInstanceData>( 1, Kiwi::GPUBufferType::VERTEX_BUFFER, Kiwi::GPUBufferUsageFlags::DYNAMIC, Kiwi::GPUBufferAccessFlags::CPU_WRITE );
				}

			} catch( Kiwi::Exception& e )
			{
				_kConsole.Print( Kiwi::ConsoleMessage( ConsoleMessageType::Error, L"[" + m_assetName + L"] Failed to create instance buffer: " + e.GetError() ) );
				return;
			}

			m_instanceGPUBuffer->Reserve();
		}
	}

	bool IModel::_LinkMesh( Kiwi::Mesh& mesh )
	{
		/*if the model isn't shared, linking will fail if there is already a linked mesh*/
		if( m_isShared == false && m_linkedMeshes.size() > 0 ) return false;

		m_linkedMeshes.insert( &mesh );
		
		return true;
	}

	void IModel::_UnlinkMesh( Kiwi::Mesh& mesh )
	{
		m_linkedMeshes.erase( &mesh );

		if( m_linkedMeshes.size() == 0 )
		{
			this->_FreeInstanceBuffer();
		}
	}

	void IModel::UpdateSharedData()
	{
		if( m_isShared == false ) return;

		Kiwi::FreeMemory( m_instanceData );

		m_instanceCenter.Set( 0.0, 0.0, 0.0 );

		/*add each model's instance data to the list*/
		for( Kiwi::Mesh* mesh : m_linkedMeshes )
		{
			if( mesh == nullptr || mesh->GetEntity() == nullptr ) continue;

			Kiwi::Transform* meshTransform = mesh->GetEntity()->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );
			if( meshTransform == nullptr ) continue;

			m_instanceCenter += meshTransform->GetPosition();

			ModelInstanceData m;
			Kiwi::Matrix4ToXMFLOAT4X4( meshTransform->GetWorldMatrix(), m.world );
			m_instanceData.push_back( m );
		}

		//average instance positions to get center point
		m_instanceCenter = m_instanceCenter / (double)m_instanceData.size();
	}

	void IModel::_SortSharedData( Kiwi::Viewport& viewport )
	{
		std::sort( m_instanceData.begin(), m_instanceData.end(), [viewport]( const ModelInstanceData& d1, const ModelInstanceData& d2 ){

			Kiwi::Camera* cam = viewport.GetCamera();
			Kiwi::Transform* camT = cam->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );
			Kiwi::Vector3d d1Pos( d1.world._14, d1.world._24, d1.world._34 );
			Kiwi::Vector3d d2Pos( d2.world._14, d2.world._24, d2.world._34 );

			return Kiwi::SquareDistance(d2Pos, camT->GetPosition()) < Kiwi::SquareDistance( d1Pos, camT->GetPosition());
		} );
	}

	void IModel::OnPreRender( Kiwi::RenderTarget& renderTarget, unsigned short viewportIndex )
	{
		//update instance data from each linked mesh
		this->UpdateSharedData();

		/*if the model uses transparency, instance data must be sorted prior to rendering
		to ensure correct transparency rendering*/
		if( m_hasTransparency && renderTarget.GetViewportCount() > viewportIndex )
		{
			this->_SortSharedData( *renderTarget.GetViewport( viewportIndex ) );
		}

		//this->CommitSharedData();

		this->_UpdateGPUBuffers();
	}



	/*--method definitions for subset subclass--*/


	IModel::Subset::Subset()
	{
		m_material = nullptr;
		parent = nullptr;
		vertexOffset = 0;
		numVertices = 0;
		indexOffset = 0;
		numIndices = 0;
		vBufIndex = 0;
		iBufIndex = 0;
		updateGPUBuffer = true;
	}

	IModel::Subset::Subset( const std::wstring& material )
	{
		m_material = nullptr;
		parent = nullptr;
		m_materialName = material;
		vertexOffset = 0;
		numVertices = 0;
		indexOffset = 0;
		numIndices = 0;
		vBufIndex = 0;
		iBufIndex = 0;
		updateGPUBuffer = true;

		if( m_materialName.size() > 0 ) this->FetchAsset( L"Material", m_materialName );
	}

	IModel::Subset::Subset( Kiwi::Material& material )
	{
		m_material = &material;
		parent = nullptr;
		m_materialName = material.GetAssetName();
		vertexOffset = 0;
		numVertices = 0;
		indexOffset = 0;
		numIndices = 0;
		vBufIndex = 0;
		iBufIndex = 0;
		updateGPUBuffer = true;
	}

	IModel::Subset::Subset( const std::wstring& material, unsigned int vOffset, unsigned int vCount, unsigned int iOffset, unsigned int iCount,
							unsigned int vBufferIndex, unsigned int iBufferIndex )
	{
		vertexOffset = vOffset;
		numVertices = vCount;
		indexOffset = iOffset;
		numIndices = iCount;
		vBufIndex = vBufferIndex;
		iBufIndex = iBufferIndex;
		m_material = nullptr;
		parent = nullptr;
		m_materialName = material;
		updateGPUBuffer = true;

		if( m_materialName.size() > 0 ) this->FetchAsset( L"Material", m_materialName );
	}

	IModel::Subset::Subset( const Kiwi::IModel::Subset& subset )
	{
		vertexOffset = subset.vertexOffset;
		numVertices = subset.numVertices;
		indexOffset = subset.indexOffset;
		numIndices = subset.numIndices;
		vBufIndex = subset.vBufIndex;
		iBufIndex = subset.iBufIndex;
		m_material = subset.m_material;
		m_materialName = subset.m_materialName;
		parent = subset.parent;
		updateGPUBuffer = true;

		if( m_material != nullptr )
		{
			m_material->Reserve();

		} else if( m_materialName.size() > 0 )
		{
			this->FetchAsset( L"Material", m_materialName );
		}
	}

	IModel::Subset::~Subset()
	{
		if( m_material != nullptr ) m_material->Free();
	}

	Kiwi::EventResponse IModel::Subset::_OnAssetFetched( Kiwi::IAsset* asset )
	{
		Material* matAsset = dynamic_cast<Kiwi::Material*>(asset);
		if( matAsset != nullptr && matAsset->GetAssetName().compare( m_materialName ) == 0 )
		{
			if( m_material != nullptr ) m_material->Free();

			m_material = matAsset;
			if( m_material != nullptr )
			{
				m_material->Reserve();
				return Kiwi::EventResponse::DISCONNECT_THIS;
			}
		}

		return Kiwi::EventResponse::NONE;
	}

	void IModel::Subset::SetMaterial( const std::wstring& material )
	{
		m_materialName = material;

		if( m_materialName.size() > 0 ) this->FetchAsset( L"Material", m_materialName );
	}

	void IModel::Subset::SetMaterial( Kiwi::Material& material )
	{
		m_materialName = material.GetAssetName();

		m_material = &material;
	}

	Kiwi::IModel::Subset& IModel::Subset::operator=( const Kiwi::IModel::Subset& subset )
	{
		vertexOffset = subset.vertexOffset;
		numVertices = subset.numVertices;
		indexOffset = subset.indexOffset;
		numIndices = subset.numIndices;
		vBufIndex = subset.vBufIndex;
		iBufIndex = subset.iBufIndex;
		parent = subset.parent;
		updateGPUBuffer = true;

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