#ifndef _KIWI_INSTANCEDMESH_H_
#define _KIWI_INSTANCEDMESH_H_

#include "Mesh.h"
#include "VertexBuffer.h"

#include <string>

namespace Kiwi
{

	class Renderer;

	template<class InstanceDataType>
	class InstancedMesh :
		public Kiwi::Mesh
	{
	protected:

		Kiwi::VertexBuffer<InstanceDataType>* m_instanceBuffer;

	public:

		/*creates an empty mesh*/
		InstancedMesh( std::wstring name, std::wstring file, Kiwi::Renderer* renderer, unsigned int vertexCount = 0, unsigned int instanceCount = 0 ):
			Mesh(name, file, renderer, vertexCount)
		{

			m_isInstanced = true;
			m_instanceBuffer = 0;

			if( instanceCount != 0 )
			{
				//create an empty instance buffer large enough to fit all of the instances
				m_instanceBuffer = new Kiwi::VertexBuffer<InstanceDataType>( m_name + L"/InstanceBuffer", m_renderer, instanceCount );
			}

		}

		/*creates a mesh from an array of vertices. the vertices will automatically be placed into a single new subset*/
		InstancedMesh( std::wstring name, std::wstring file, Kiwi::Renderer* renderer, std::vector<Kiwi::Mesh::Vertex> vertices ):
			Mesh(name, file, renderer, vertices)
		{

			m_isInstanced = true;
			m_instanceBuffer = 0;

		}

		InstancedMesh( std::wstring name, std::wstring file, Kiwi::Renderer* renderer, std::vector<Kiwi::Mesh::Subset> subsetList ):
			Mesh(name, file renderer, subsetList)
		{

			m_isInstanced = true;
			m_instanceBuffer = 0;

		}

		~InstancedMesh()
		{
			SAFE_DELETE( m_instanceBuffer );
		}

		void Bind( Kiwi::Renderer* renderer )
		{

			if( renderer == 0 )
			{
				throw Kiwi::Exception( L"InstancedMesh::Bind", L"[" + m_name + L"] Invalid Renderer" );
			}

			if( m_vertexBuffer == 0 || m_indexBuffer == 0 || m_instanceBuffer == 0 )
			{
				throw Kiwi::Exception( L"InstancedMesh::Bind", L"[" + m_name + L"] Mesh buffers invalid" );
			}

			ID3D11DeviceContext* deviceCon = renderer->GetDeviceContext();

			unsigned int strides[2] = { sizeof( ShaderVertex ), sizeof( InstanceDataType ) };
			unsigned int offsets[2] = { 0, 0 };

			ID3D11Buffer* vBuffer = m_vertexBuffer->GetD3DBuffer();
			ID3D11Buffer* instanceBuffer = m_instanceBuffer->GetD3DBuffer();

			ID3D11Buffer* vertexBuffers[2] = { vBuffer, instanceBuffer };

			// set the vertex and instance buffers to active
			deviceCon->IASetVertexBuffers( 0, 2, vertexBuffers, strides, offsets );

			// same for index buffer
			ID3D11Buffer* iBuffer = m_indexBuffer->GetD3DBuffer();
			deviceCon->IASetIndexBuffer( iBuffer, DXGI_FORMAT_R32_UINT, 0 );

		}

		void SetInstanceData( std::vector<InstanceDataType>& instanceData )
		{

			if( m_renderer == 0 )
			{
				throw Kiwi::Exception( L"InstancedMesh::SetInstanceData", L"[" + m_name + L"] Invalid Renderer" );
			}

			if( m_instanceBuffer == 0 )
			{
				m_instanceBuffer = new Kiwi::VertexBuffer<InstanceDataType>( m_name + L"/InstanceBuffer", m_renderer, (long)instanceData.size() );

			} else if( m_instanceBuffer->GetCapacity() < instanceData.size() )
			{
				m_instanceBuffer->Resize( (long)instanceData.size() );
			}

			m_instanceBuffer->SetData( instanceData );
			m_instanceCount = (long)instanceData.size();
			m_instanceCapacity = m_instanceBuffer->GetCapacity();

		}

		Kiwi::IBuffer* GetInstanceBuffer()const 
		{ 
			return m_instanceBuffer; 
		}

		long GetInstanceCapacity()const
		{
			return (m_instanceBuffer != 0) ? m_instanceBuffer->GetCapacity() : 0;
		}

		long GetInstanceCount()const
		{
			return (m_instanceBuffer != 0) ? m_instanceBuffer->GetElementCount() : 0;
		}

	};
}

#endif