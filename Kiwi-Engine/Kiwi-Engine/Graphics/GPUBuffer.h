#ifndef _KIWI_GPUBUFFER_H_
#define _KIWI_GPUBUFFER_H_

#include "IGPUBuffer.h"
#include "Renderer.h"

#include <mutex>

namespace Kiwi
{
	template<typename BufferDataType>
	class GPUBuffer:
		public Kiwi::IGPUBuffer
	{
	protected:

		std::mutex m_gpuBuffer_M;

	public:

		GPUBuffer( Kiwi::Renderer& renderer, ID3D11Buffer& buffer, Kiwi::GPUBufferUsageFlags bufferUsage, Kiwi::GPUBufferAccessFlags cpuAccessFlags, long bufferOffset, long elementCount, long elementCapacity, bool isStatic ):
			Kiwi::IGPUBuffer( renderer, buffer, bufferUsage, cpuAccessFlags, bufferOffset, elementCount, elementCapacity, isStatic )
		{
		}

		~GPUBuffer()
		{
			SAFE_RELEASE( m_buffer );
		}

		virtual void Update( std::vector<BufferDataType>& bufferData, Kiwi::GPUBufferMapOp mapOp = Kiwi::GPUBufferMapOp::DISCARD )
		{
			std::lock_guard < std::mutex> lock( m_gpuBuffer_M );

			if( m_buffer )
			{
				D3D11_MAPPED_SUBRESOURCE mr;

				HRESULT hr = m_renderer->MapResource( m_buffer, 0, (D3D11_MAP)mapOp, 0, &mr );
				if( FAILED( hr ) )
				{
					throw Kiwi::Exception( L"VertexBuffer::SetData", L"Failed to map vertex buffer: " + Kiwi::GetD3DErrorString( hr ) );
				}

				const BufferDataType* newData = &bufferData[0];

				/*calculate where in the buffer to begin copying the new data (will be the start
				of the buffer unless this GPUBuffer is using a shared buffer)*/
				void* bufferPtr = static_cast<BufferDataType*>(mr.pData) + m_bufferOffset;

				if( bufferData.size() <= m_elementCapacity )
				{
					memcpy( bufferPtr, newData, bufferData.size() * sizeof( BufferDataType ) );
					m_elementCount = (long)bufferData.size();

					m_renderer->UnmapResource( m_buffer, 0 );

				} else
				{
					memcpy( bufferPtr, newData, m_elementCapacity * sizeof( BufferDataType ) );
					m_elementCount = m_elementCapacity;

					m_renderer->UnmapResource( m_buffer, 0 );

					KIWIDEBUGSTRING( Kiwi::ToWString( L"WARNING: Attempted to copy " + Kiwi::ToWString(bufferData.size()) + L" elements to GPUBuffer of size " + Kiwi::ToWString(m_elementCapacity) ).c_str() );
				}
			}
		}

		virtual unsigned int GetStride()const
		{
			return sizeof( BufferDataType );
		}

	};
}

#endif