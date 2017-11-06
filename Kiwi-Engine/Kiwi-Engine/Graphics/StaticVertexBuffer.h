#ifndef _KIWI_STATICVERTEXBUFFER_H_
#define _KIWI_STATICVERTEXBUFFER_H_

#include "IBuffer.h"
#include "DirectX.h"

#include "../Core/Utilities.h"
#include "../Core/Exception.h"

#include <string>
#include <vector>

namespace Kiwi
{

	template<class BufferDataType>
	class StaticVertexBuffer :
		public IBuffer
	{
	protected:



	public:

		StaticVertexBuffer(std::wstring name, Kiwi::Renderer* renderer, std::vector<BufferDataType> bufferData) :
			IBuffer(name, renderer, bufferData.size())
		{

			m_elementCount = bufferData.size();
			m_bufferSize = sizeof(BufferDataType) * m_bufferCapacity;

			//create the static vertex buffer
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.ByteWidth = m_bufferSize;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA vertexData;
			vertexData.pSysMem = &bufferData[0];
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;

			HRESULT hr = m_device->CreateBuffer(&bufferDesc, &vertexData, &m_buffer);
			if (FAILED(hr))
			{
				throw Kiwi::Exception(L"StaticVertexBuffer", L"Failed to create buffer");
			}

		}

		~StaticVertexBuffer() {}

	};

};

#endif