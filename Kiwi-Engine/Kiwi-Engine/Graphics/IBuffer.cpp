#include "IBuffer.h"
#include "Renderer.h"
#include "DirectX.h"

#include "../Core/Utilities.h"
#include "../Core/Exception.h"

namespace Kiwi
{

	IBuffer::IBuffer(std::wstring name, Kiwi::Renderer* renderer, long elementCapacity)
	{

		if(renderer == 0) throw Kiwi::Exception(L"IBuffer", L"Invalid Renderer");

		m_name = name;
		m_elementCapacity = elementCapacity;
		m_device = renderer->GetDevice();
		m_deviceContext = renderer->GetDeviceContext();

		m_buffer = 0;

	}

	IBuffer::~IBuffer()
	{

		m_device = 0;
		m_deviceContext = 0;

		SAFE_RELEASE(m_buffer);

	}

};