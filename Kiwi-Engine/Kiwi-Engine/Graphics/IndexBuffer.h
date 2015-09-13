#ifndef _KIWI_INDEXBUFFER_H_
#define _KIWI_INDEXBUFFER_H_

#include "IBuffer.h"
#include "DirectX.h"

#include "../Core/Utilities.h"
#include "../Core/Exception.h"

#include <string>
#include <vector>

namespace Kiwi
{

	class IndexBuffer :
		public IBuffer
	{
	protected:


	public:

		IndexBuffer(std::wstring name, Kiwi::Renderer* renderer, long elementCapacity):
			IBuffer(name, renderer, elementCapacity)
		{

			m_elementCount = 0;
			m_bufferSize = sizeof(unsigned long) * m_elementCapacity;

			//create the buffer
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth = m_bufferSize;
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			HRESULT hr = m_device->CreateBuffer(&bufferDesc, 0, &m_buffer);
			if (FAILED(hr))
			{
				throw Kiwi::Exception(L"IndexBuffer", L"Failed to create buffer");
			}

		}

		~IndexBuffer() {}

		virtual void SetData(std::vector<unsigned long>& bufferData)
		{

			assert( m_deviceContext != 0 );

			if( bufferData.size() == 0 ) return;

			D3D11_MAPPED_SUBRESOURCE mr;
			unsigned long* dataPtr = 0;

			if( m_buffer )
			{
				HRESULT hr = m_deviceContext->Map( m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr );
				if( FAILED( hr ) )
				{
					throw Kiwi::Exception( L"IndexBuffer::SetBufferData", L"Failed to map buffer" );
				}

				const unsigned long* newData = &bufferData[0];

				if( bufferData.size() <= m_elementCapacity )
				{
					memcpy( mr.pData, newData, bufferData.size() * sizeof( unsigned long ) );
					m_elementCount = (long)bufferData.size();
				} else
				{
					//too many elements were passed in, copy the maximum amount allowed into the buffer
					memcpy( mr.pData, newData, m_elementCapacity * sizeof( unsigned long ) );
					m_elementCount = m_elementCapacity;
				}

				m_deviceContext->Unmap( m_buffer, 0 );
			}

		}

	};
};

#endif