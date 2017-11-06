#include "IGPUBuffer.h"
#include "Renderer.h"
#include "DirectX.h"

#include "../Core/Utilities.h"
#include "..\Core\Exception.h"

namespace Kiwi
{

	IGPUBuffer::IGPUBuffer( Kiwi::Renderer& renderer, ID3D11Buffer& buffer, Kiwi::GPUBufferUsageFlags bufferUsage, Kiwi::GPUBufferAccessFlags cpuAccessFlags, long bufferOffset, long elementCount, long elementCapacity, bool isStatic):
		Kiwi::IReferencedObject( L"" )
	{
		m_renderer = &renderer;
		m_buffer = &buffer;
		m_bufferOffset = bufferOffset;
		m_elementCapacity = elementCapacity;
		m_elementCount = elementCount;
		m_isStatic = isStatic;
		m_bufferSize = 0;
		m_usageFlags = bufferUsage;
		m_cpuAccessFlags = cpuAccessFlags;
		m_bufferUID = Kiwi::NewUID();
	}

	IGPUBuffer::~IGPUBuffer()
	{
		SAFE_RELEASE(m_buffer);
	}

	Kiwi::GPUBufferType IGPUBuffer::GetType()
	{
		if( m_buffer != nullptr )
		{
			D3D11_BUFFER_DESC desc;
			m_buffer->GetDesc( &desc );

			return (Kiwi::GPUBufferType)desc.BindFlags;

		} else
		{
			return Kiwi::GPUBufferType::NONE;
		}
	}

};