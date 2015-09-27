#ifndef _KIWI_VERTEXBUFFER_H_
#define _KIWI_VERTEXBUFFER_H_

#include "IBuffer.h"
#include "DirectX.h"

#include "../Core/Utilities.h"
#include "../Core/Exception.h"

#include <string>
#include <vector>

namespace Kiwi
{

	template<class BufferDataType>
	class VertexBuffer :
		public IBuffer
	{
	protected:



	public:

		VertexBuffer(std::wstring name, Kiwi::Renderer* renderer, long elementCapacity):
			IBuffer(name, renderer, elementCapacity)
		{

			m_elementCount = 0;
			m_bufferSize = sizeof(BufferDataType) * m_elementCapacity;

			//create the buffer
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth = m_bufferSize;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			HRESULT hr = m_device->CreateBuffer(&bufferDesc, 0, &m_buffer);
			if (FAILED(hr))
			{
				throw Kiwi::Exception(L"VertexBuffer", L"Failed to create buffer");
			}

		}

		~VertexBuffer() {}

		virtual void SetData(std::vector<BufferDataType>& bufferData)
		{

			assert( m_deviceContext != 0 );

			if( bufferData.size() == 0 ) return;

			D3D11_MAPPED_SUBRESOURCE mr;
			BufferDataType* dataPtr = 0;

			if( m_buffer )
			{
				HRESULT hr = m_deviceContext->Map( m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr );
				if( FAILED( hr ) )
				{
					throw Kiwi::Exception( L"VertexBuffer::SetBufferData", L"["+m_name+L"] Failed to map buffer" );
				}

				const BufferDataType* newData = &bufferData[0];

				if( bufferData.size() <= m_elementCapacity )
				{
					memcpy( mr.pData, newData, bufferData.size() * sizeof( BufferDataType ) );
					m_elementCount = (long)bufferData.size();
				} else
				{
					//too many elements were passed in, copy the maximum amount allowed into the buffer
					m_deviceContext->Unmap( m_buffer, 0 );
					throw Kiwi::Exception( L"VertexBuffer::SetData", L"[" + m_name + L"] Vertex buffer is too small (size: " + Kiwi::ToWString( m_elementCapacity ) + L") to contain passed data (size: " + Kiwi::ToWString( bufferData.size() ) + L")" );
				}

				m_deviceContext->Unmap( m_buffer, 0 );
			}

		}

		virtual void Resize( unsigned int maxElementCount )
		{

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

			HRESULT hr = m_device->CreateBuffer( &bufferDesc, 0, &m_buffer );
			if( FAILED( hr ) )
			{
				throw Kiwi::Exception( L"VertexBuffer", L"Failed to create buffer" );
			}

		}

	};

};

#endif