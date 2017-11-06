#ifndef _KIWI_IGPUBUFFER_H_
#define _KIWI_IGPUBUFFER_H_

#include "DirectX.h"

#include "../Core/IReferencedObject.h"

#include <string>

namespace Kiwi
{
	class Renderer;
	class GPUBufferManager;

	enum class GPUBufferType { NONE = 0x0L, VERTEX_BUFFER = 0x1L, INDEX_BUFFER = 0x2L, CONSTANT_BUFFER = 0x4L, SHADER_RESOURCE = 0x8L, STREAM_OUTPUT = 0x10L, RENDER_TARGET = 0x20L, DEPTH_STENCIL = 0x40L, UNORDERED_ACCESS = 0x80L, DECODER = 0x200L, VIDEO_ENCODER = 0x400L };
	enum class GPUBufferUsageFlags { DEFAULT = 0, IMMUTABLE = 1, DYNAMIC = 2, STAGING = 3, NONE = 4 };
	enum class GPUBufferMapOp { READ = 1, WRITE = 2, READ_WRITE = 3, DISCARD = 4, WRITE_NO_OVERWRITE = 5 };
	enum class GPUBufferAccessFlags { NONE = 0, CPU_WRITE = 0x10000L, CPU_READ = 0x20000L };

	class IGPUBuffer:
		public Kiwi::IReferencedObject
	{
		friend class Kiwi::GPUBufferManager;
		friend class Kiwi::Renderer;
	protected:

		Kiwi::Renderer* m_renderer;

		ID3D11Buffer* m_buffer;

		Kiwi::GPUBufferUsageFlags m_usageFlags;

		Kiwi::GPUBufferAccessFlags m_cpuAccessFlags;

		long m_bufferUID;

		long m_elementCount;

		//stores the maximum number of elements that can fit in the buffer
		long m_elementCapacity;

		//stores the size, in bytes of the buffer
		long m_bufferSize;

		/*index within the buffer where the contents of this buffer start (will be 0 unless one 
		buffer is being shared between multiple buffer instances)*/
		long m_bufferOffset;

		/*static buffers are created as IMMUTABLE, while non-static buffers are created as dynamic*/
		bool m_isStatic;

	private:

		ID3D11Buffer* _GetD3D()const { return m_buffer; }

	public:

		IGPUBuffer( Kiwi::Renderer& renderer, ID3D11Buffer& buffer, Kiwi::GPUBufferUsageFlags bufferUsage, Kiwi::GPUBufferAccessFlags cpuAccessFlags, long bufferOffset, long elementCount, long elementCapacity, bool isStatic );

		virtual ~IGPUBuffer() = 0;

		//returns the number of elements the buffer can hold
		long GetCapacity()const { return m_elementCapacity; }

		Kiwi::GPUBufferAccessFlags GetCPUAccessFlags()const { return m_cpuAccessFlags; }

		//returns the current number of elements in the buffer
		long GetElementCount()const { return m_elementCount;  }

		long GetOffset()const { return m_bufferOffset; }

		Kiwi::Renderer* GetRenderer() { return m_renderer; }

		//returns the size of the buffer in bytes
		long GetSize()const { return m_bufferSize; }

		GPUBufferType GetType();

		long GetUID()const { return m_bufferUID; }

		Kiwi::GPUBufferUsageFlags GetUsageFlags()const { return m_usageFlags; }

		bool IsStatic()const { return m_isStatic; }

		virtual unsigned int GetStride()const = 0;

	};
};

#endif