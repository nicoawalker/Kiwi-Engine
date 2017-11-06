#ifndef _KIWI_INSTANCEDMESH_H_
#define _KIWI_INSTANCEDMESH_H_

#include "Mesh.h"
//#include "VertexBuffer.h"

#include "../Core/Vector3d.h"

#include <string>
#include <vector>

namespace Kiwi
{

	class Renderer;

	template<class InstanceDataType>
	class InstancedMesh :
		public Kiwi::Mesh
	{
	protected:

		Kiwi::GPUBuffer<InstanceDataType>* m_instanceBuffer;
		std::vector<InstanceDataType>* m_instances;

	public:

		/*creates an empty mesh*/
		InstancedMesh( unsigned int vertexCount = 0, unsigned int instanceCount = 1 ) :
			Mesh()
		{

			m_instanceBuffer = 0;
			m_isInstanced = true;

		}

		/*creates an empty, named mesh*/
		InstancedMesh( std::wstring name, unsigned int vertexCount = 0, unsigned int instanceCount = 1 ):
			Mesh(name)
		{
			m_instanceBuffer = 0;
			m_isInstanced = true;
		}

		/*creates a mesh from an array of vertices. the vertices will automatically be placed into a single new subset*/
		InstancedMesh( std::vector<Kiwi::Vector3d>& vertices, std::vector<Kiwi::Vector2d>& uvs, std::vector<Kiwi::Vector3d>& normals, std::vector<InstanceDataType>& instances ) :
			Mesh( vertices, uvs, normals )
		{
			m_instanceBuffer = m_renderer->GetGPUBufferManager().CreateBuffer<InstanceDataType>( instances, (long)instances.size(), Kiwi::GPUBufferType::VERTEX_BUFFER, Kiwi::GPUBufferUsageFlags::DYNAMIC );
			m_instanceBuffer->Reserve();

			m_isInstanced = true;
			m_instances = instances;
		}

		/*creates a mesh from an array of vertices. the vertices will automatically be placed into a single new subset*/
		InstancedMesh( std::wstring name, std::vector<Kiwi::Vector3d>& vertices, std::vector<Kiwi::Vector2d>& uvs, std::vector<Kiwi::Vector3d>& normals, std::vector<InstanceDataType>& instances ):
			Mesh(name, vertices, uvs, normals)
		{
			m_instanceBuffer = m_renderer->GetGPUBufferManager().CreateBuffer<InstanceDataType>( instances, (long)instances.size(), Kiwi::GPUBufferType::VERTEX_BUFFER, Kiwi::GPUBufferUsageFlags::DYNAMIC );
			m_instanceBuffer->Reserve();

			m_isInstanced = true;
			m_instances = instances;

		}

		~InstancedMesh()
		{
			if( m_instanceBuffer != nullptr )m_instanceBuffer->Free();
		}

		void _OnAttached()
		{
			Mesh::_OnAttached();

			if( m_renderer )
			{
				//m_instanceBuffer = new Kiwi::VertexBuffer<InstanceDataType>( *m_renderer, m_instances.size() + 1 );
				//this->SetInstances( m_instances );
			}
		}

		void Bind( Kiwi::Renderer& renderer )
		{
			if( m_indexBuffer && m_vertexBuffer && m_instanceBuffer )
			{
				std::vector<unsigned int> strides = { sizeof( Vertex ), sizeof( InstanceDataType ) };
				std::vector<unsigned int> offsets = { 0, 0 };

				std::vector<Kiwi::IGPUBuffer*> vertexBuffers = { m_vertexBuffer, m_instanceBuffer };

				// set the vertex and instance buffers to active
				renderer.SetVertexBuffers( 0, vertexBuffers, strides, offsets );

				// same for index buffer
				renderer.SetIndexBuffer( m_indexBuffer, DXGI_FORMAT_R32_UINT, 0 );

			} else
			{
				throw Kiwi::Exception( L"InstancedMesh::Bind", L"[" + Kiwi::ToWString( m_objectID ) + L", " + m_objectName + L"] There are no buffers to bind" );
			}
		}

		void SetInstances( std::vector<InstanceDataType>& instanceData )
		{
			if( m_indexBuffer && m_vertexBuffer && m_instanceBuffer )
			{

				if( instanceData.size() > 0 )
				{
					/*if( m_instanceBuffer->GetCapacity() != instanceData.size() )
					{
						m_instanceBuffer->Resize( (long)instanceData.size() );
					}

					m_instanceBuffer->SetData( instanceData );
					m_instanceCount = (long)instanceData.size();
					m_instanceCapacity = m_instanceBuffer->GetCapacity();*/
				}

			} else
			{
				throw Kiwi::Exception( L"InstancedMesh::SetInstanceData", L"[" + Kiwi::ToWString( m_objectID ) + L", " + m_objectName + L"] Buffers do not exist" );
			}
		}

		Kiwi::IGPUBuffer* GetInstanceBuffer()const 
		{ 
			return m_instanceBuffer; 
		}

		long GetInstanceCapacity()const
		{
			return m_instanceBuffer->GetCapacity();
		}

		long GetInstanceCount()const
		{
			return m_instanceBuffer->GetElementCount();
		}

	};
}

#endif