#ifndef _KIWI_VERTEXBUFFER_H_
#define _KIWI_VERTEXBUFFER_H_

#include "IBuffer.h"
#include "DirectX.h"
#include "Renderer.h"

#include "../Core/Utilities.h"
#include "../Core/Exception.h"

#include <string>
#include <vector>
#include <mutex>

namespace Kiwi
{

	template<class BufferDataType>
	class VertexBuffer :
		public IBuffer
	{
	protected:

		

	public:

		VertexBuffer(Kiwi::Renderer& renderer, long elementCapacity):
			IBuffer(renderer, elementCapacity, false)
		{
			if( elementCapacity < 1 )
			{
				m_elementCapacity = 1;
			}

			m_elementCount = 0;
			m_bufferSize = sizeof(BufferDataType) * m_elementCapacity;

			if( m_bufferSize < 4 )
			{
				m_bufferSize = 4;
			}

			//create the buffer
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth = m_bufferSize;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			HRESULT hr = m_renderer->GetDevice()->CreateBuffer(&bufferDesc, 0, &m_buffer);
			if (FAILED(hr))
			{
				throw Kiwi::Exception(L"VertexBuffer", L"Failed to create buffer: " + Kiwi::GetD3DErrorString( hr ) );
			}
		}

		VertexBuffer( Kiwi::Renderer& renderer, long elementCapacity, std::vector<BufferDataType>& bufferData, D3D11_USAGE usageFlag = D3D11_USAGE_DEFAULT, UINT cpuAccessFlags = 0 ) :
			IBuffer( renderer, elementCapacity, true )
		{
			if( bufferData.size() > m_elementCapacity )
			{
				m_elementCapacity = (long)bufferData.size();
			}

			m_bufferSize = sizeof( BufferDataType ) * m_elementCapacity;
			if( m_bufferSize < 4 )
			{
				m_bufferSize = 4;
			}

			//create the static vertex buffer
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory( &bufferDesc, sizeof( bufferDesc ) );
			bufferDesc.Usage = usageFlag;
			bufferDesc.ByteWidth = m_bufferSize;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = cpuAccessFlags;

			D3D11_SUBRESOURCE_DATA vertexData;
			vertexData.pSysMem = &bufferData[0];
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;

			HRESULT hr = m_renderer->GetDevice()->CreateBuffer( &bufferDesc, &vertexData, &m_buffer );
			if( FAILED( hr ) )
			{
				throw Kiwi::Exception( L"VertexBuffer", L"Failed to create buffer: " + Kiwi::GetD3DErrorString( hr ) );
			}



			//if( elementCapacity < 1 )
			//{
			//	elementCapacity = 1;
			//}

			//m_elementCount = 0;
			//m_bufferSize = sizeof( BufferDataType ) * m_elementCapacity;

			//if( m_bufferSize < 4 )
			//{
			//	m_bufferSize = 4;
			//}

			////create the buffer
			//D3D11_BUFFER_DESC bufferDesc;
			//ZeroMemory( &bufferDesc, sizeof( bufferDesc ) );
			//bufferDesc.Usage = usageFlag;
			//bufferDesc.ByteWidth = m_bufferSize;
			//bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			//bufferDesc.CPUAccessFlags = cpuAccessFlags;

			//HRESULT hr = m_renderer->GetDevice()->CreateBuffer( &bufferDesc, 0, &m_buffer );
			//if( FAILED( hr ) )
			//{
			//	throw Kiwi::Exception( L"VertexBuffer", L"Failed to create buffer: " + Kiwi::GetD3DErrorString( hr ) );
			//}

		}

		~VertexBuffer() {}

		virtual void SetData(std::vector<BufferDataType>& bufferData)
		{

			assert( m_renderer != 0 );

			if( bufferData.size() == 0 )
			{
				this->Resize( 1 );
				return;
			}

			D3D11_MAPPED_SUBRESOURCE mr;
			BufferDataType* dataPtr = 0;

			if( m_buffer )
			{
				HRESULT hr = m_renderer->MapResource( m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr );
				if( FAILED( hr ) )
				{
					throw Kiwi::Exception( L"VertexBuffer::SetData", L"Failed to map vertex buffer: " + Kiwi::GetD3DErrorString( hr ) );
				}

				const BufferDataType* newData = &bufferData[0];

				if( bufferData.size() <= m_elementCapacity )
				{
					memcpy( mr.pData, newData, bufferData.size() * sizeof( BufferDataType ) );
					m_elementCount = (long)bufferData.size();
				} else
				{
					//too many elements were passed in
					m_renderer->UnmapResource( m_buffer, 0 );
					throw Kiwi::Exception( L"VertexBuffer::SetData", L"Vertex buffer is too small (size: " + Kiwi::ToWString( m_elementCapacity ) + L") to contain passed data (size: " + Kiwi::ToWString( bufferData.size() ) + L")" );
				}

				m_renderer->UnmapResource( m_buffer, 0 );
			}

		}

		virtual void Resize( unsigned int maxElementCount )
		{

			assert( m_renderer != 0 );

			if( m_buffer )
			{
				SAFE_RELEASE( m_buffer );
			}

			m_elementCount = 0;
			m_elementCapacity = maxElementCount;
			m_bufferSize = sizeof( BufferDataType ) * m_elementCapacity;

			//recreate the buffer
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory( &bufferDesc, sizeof( bufferDesc ) );
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth = m_bufferSize;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			HRESULT hr = m_renderer->GetDevice()->CreateBuffer( &bufferDesc, 0, &m_buffer );
			if( FAILED( hr ) )
			{
				throw Kiwi::Exception( L"VertexBuffer::Resize", L"Failed to create buffer: " + Kiwi::GetD3DErrorString(hr) );
			}

		}

	};

};

#endif