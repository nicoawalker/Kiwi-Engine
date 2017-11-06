#ifndef _KIWI_INDEXBUFFER_H_
#define _KIWI_INDEXBUFFER_H_

#include "IBuffer.h"
#include "DirectX.h"

#include "../Core/Utilities.h"
#include "..\Core\Exception.h"

#include <string>
#include <vector>

namespace Kiwi
{

	class IndexBuffer :
		public IBuffer
	{
	protected:


	public:

		IndexBuffer( Kiwi::Renderer& renderer, long elementCapacity ) :
			IBuffer( renderer, elementCapacity, false )
		{

			if( elementCapacity < 1 )
			{
				elementCapacity = 1;
			}
			m_elementCount = 0;
			m_bufferSize = sizeof( unsigned long ) * m_elementCapacity;

			//create the buffer
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory( &bufferDesc, sizeof( bufferDesc ) );
			bufferDesc.ByteWidth = m_bufferSize;
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			HRESULT hr = m_renderer->GetDevice()->CreateBuffer(&bufferDesc, 0, &m_buffer);
			if (FAILED(hr))
			{
				throw Kiwi::Exception(L"IndexBuffer", L"Failed to create buffer: " + Kiwi::GetD3DErrorString( hr ) );
			}

		}

		IndexBuffer( Kiwi::Renderer& renderer, long elementCapacity, bool isStatic, std::vector<unsigned long> & indexData ) :
			IBuffer( renderer, elementCapacity, isStatic )
		{

			if( elementCapacity < 1 )
			{
				elementCapacity = 1;
			}
			m_elementCount = 0;
			m_bufferSize = sizeof( unsigned long ) * m_elementCapacity;

			//create the buffer
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory( &bufferDesc, sizeof( bufferDesc ) );
			bufferDesc.ByteWidth = m_bufferSize;
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			if( m_isStatic )
			{
				bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
				bufferDesc.CPUAccessFlags = 0;
			} else
			{
				bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
				bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			}

			D3D11_SUBRESOURCE_DATA indexSub;
			indexSub.pSysMem = &indexData[0];
			indexSub.SysMemPitch = 0;
			indexSub.SysMemSlicePitch = 0;

			HRESULT hr = m_renderer->GetDevice()->CreateBuffer( &bufferDesc, &indexSub, &m_buffer );
			if( FAILED( hr ) )
			{
				throw Kiwi::Exception( L"IndexBuffer", L"Failed to create buffer: " + Kiwi::GetD3DErrorString( hr ) );
			}

		}

		~IndexBuffer() {}

		virtual void SetData(std::vector<unsigned long>& bufferData)
		{

			assert( m_renderer != 0 );

			if( bufferData.size() == 0 )
			{
				this->Resize( 1 );
				return;
			}

			D3D11_MAPPED_SUBRESOURCE mr;
			unsigned long* dataPtr = 0;

			if( m_buffer )
			{
				HRESULT hr = m_renderer->MapResource( m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mr );
				if( FAILED( hr ) )
				{
					throw Kiwi::Exception( L"IndexBuffer::SetData", L"Failed to map buffer: " + Kiwi::GetD3DErrorString( hr ) );
				}

				const unsigned long* newData = &bufferData[0];

				if( bufferData.size() <= m_elementCapacity )
				{
					memcpy( mr.pData, newData, bufferData.size() * sizeof( unsigned long ) );
					m_elementCount = (long)bufferData.size();
				} else
				{
					//too many elements were passed in
					m_renderer->UnmapResource( m_buffer, 0 );
					throw Kiwi::Exception( L"IndexBuffer::SetData", L"Index buffer is too small (size: " + Kiwi::ToWString( m_elementCapacity ) + L") to contain passed data (size: " + Kiwi::ToWString( bufferData.size() ) + L")" );
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
			m_bufferSize = sizeof( unsigned long ) * m_elementCapacity;

			//recreate the buffer
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory( &bufferDesc, sizeof( bufferDesc ) );
			bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDesc.ByteWidth = m_bufferSize;
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;

			HRESULT hr = m_renderer->GetDevice()->CreateBuffer( &bufferDesc, 0, &m_buffer );
			if( FAILED( hr ) )
			{
				throw Kiwi::Exception( L"IndexBuffer::Resize", L"Failed to create buffer: " + Kiwi::GetD3DErrorString( hr ) );
			}

		}

	};
};

#endif