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

	class StaticIndexBuffer :
		public IBuffer
	{
	protected:


	public:

		StaticIndexBuffer(std::wstring name, Kiwi::Renderer* renderer, std::vector<unsigned long> bufferData) :
			IBuffer(name, renderer, bufferData.size())
		{

			m_elementCount = bufferData.size();
			m_bufferSize = sizeof(unsigned long) * m_elementCapacity;

			//create the buffer
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.ByteWidth = m_bufferSize;
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA indexData;
			indexData.pSysMem = &bufferData[0];
			indexData.SysMemPitch = 0;
			indexData.SysMemSlicePitch = 0;

			HRESULT hr = m_device->CreateBuffer(&bufferDesc, &indexData, &m_buffer);
			if (FAILED(hr))
			{
				throw Kiwi::Exception(L"StaticIndexBuffer", L"Failed to create buffer");
			}

		}

		~StaticIndexBuffer() {}

	};
};

#endif