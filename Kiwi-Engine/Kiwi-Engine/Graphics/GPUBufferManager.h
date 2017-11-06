#ifndef _KIWI_GPUBUFFERMANAGER_H_
#define _KIWI_GPUBUFFERMANAGER_H_

#include "IGPUBuffer.h"

#include <unordered_map>
#include <mutex>

namespace Kiwi
{

	class Renderer;

	template<typename DataType> class GPUBuffer;

	class GPUBufferManager
	{
	protected:

		Kiwi::Renderer* m_renderer;

		std::unordered_map<long, std::unique_ptr<Kiwi::IGPUBuffer>> m_buffers;

		std::mutex m_gpuBufferManager_M;

	protected:

		template<typename BufferDataType>
		Kiwi::GPUBuffer<BufferDataType>* _CopyBuffer( Kiwi::GPUBuffer<BufferDataType>* source )
		{
			unsigned int bufferSize = unsigned int( sizeof( BufferDataType ) * source->GetCapacity() );

			//create the vertex buffer
			D3D11_BUFFER_DESC bufferDesc;
			source->_GetD3D()->GetDesc( &bufferDesc );

			D3D11_SUBRESOURCE_DATA srcData;
			srcData.pSysMem = nullptr;
			srcData.SysMemPitch = 0;
			srcData.SysMemSlicePitch = 0;

			ID3D11Buffer* buffer = nullptr;

			HRESULT hr = m_renderer->GetDevice()->CreateBuffer( &bufferDesc, &srcData, &buffer );
			if( FAILED( hr ) )
			{
				throw Kiwi::Exception( L"GPUBufferManager::_CopyBuffer", L"Failed to create new buffer: " + Kiwi::GetD3DErrorString( hr ) );
			}

			m_renderer->CopyResource( buffer, source->_GetD3D() );

			std::unique_ptr<Kiwi::GPUBuffer<BufferDataType>> kBuffer = std::make_unique<Kiwi::GPUBuffer<BufferDataType>>( *m_renderer, *buffer, source->GetUsageFlags(), source->GetCPUAccessFlags(), source->GetOffset(), 0, source->GetCapacity(), source->IsStatic() );

			auto itr = m_buffers.insert( std::make_pair( kBuffer->GetUID(), std::move( kBuffer ) ) );
			if( itr.second )
			{
				return dynamic_cast<Kiwi::GPUBuffer<BufferDataType>*>(itr.first->second.get());
			}

			return nullptr;
		}

		template<typename BufferDataType>
		Kiwi::GPUBuffer<BufferDataType>* _CreateBuffer( unsigned long bufferCapacity, Kiwi::GPUBufferType bufferType, Kiwi::GPUBufferUsageFlags bufferUsage = Kiwi::GPUBufferUsageFlags::DEFAULT, Kiwi::GPUBufferAccessFlags cpuAccessFlags = Kiwi::GPUBufferAccessFlags::NONE )
		{
			if( bufferCapacity == 0 ) return nullptr;

			unsigned int bufferSize = unsigned int( sizeof( BufferDataType ) * bufferCapacity );

			//create the vertex buffer
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory( &bufferDesc, sizeof( bufferDesc ) );
			bufferDesc.Usage = (D3D11_USAGE)bufferUsage;
			bufferDesc.ByteWidth = bufferSize;
			bufferDesc.BindFlags = (D3D11_BIND_FLAG)bufferType;
			bufferDesc.CPUAccessFlags = (D3D11_CPU_ACCESS_FLAG)cpuAccessFlags;

			D3D11_SUBRESOURCE_DATA srcData;
			srcData.pSysMem = nullptr;
			srcData.SysMemPitch = 0;
			srcData.SysMemSlicePitch = 0;

			ID3D11Buffer* buffer = nullptr;

			HRESULT hr = m_renderer->GetDevice()->CreateBuffer( &bufferDesc, &srcData, &buffer );
			if( FAILED( hr ) )
			{
				throw Kiwi::Exception( L"GPUBufferManager::CreateBuffer", L"Failed to create buffer: " + Kiwi::GetD3DErrorString( hr ) );
			}

			std::unique_ptr<Kiwi::GPUBuffer<BufferDataType>> kBuffer = std::make_unique<Kiwi::GPUBuffer<BufferDataType>>( *m_renderer, *buffer, bufferUsage, cpuAccessFlags, 0, 0, bufferCapacity, false );

			auto itr = m_buffers.insert( std::make_pair( kBuffer->GetUID(), std::move( kBuffer ) ) );
			if( itr.second )
			{
				return dynamic_cast<Kiwi::GPUBuffer<BufferDataType>*>(itr.first->second.get());
			}

			return nullptr;
		}

		template<typename BufferDataType>
		Kiwi::GPUBuffer<BufferDataType>* _CreateBuffer( const std::vector<BufferDataType>& bufferData, unsigned long bufferCapacity, Kiwi::GPUBufferType bufferType, Kiwi::GPUBufferUsageFlags bufferUsage = Kiwi::GPUBufferUsageFlags::DEFAULT, Kiwi::GPUBufferAccessFlags cpuAccessFlags = Kiwi::GPUBufferAccessFlags::NONE )
		{
			if( bufferCapacity == 0 ) return nullptr;

			unsigned int bufferSize = unsigned int( sizeof( BufferDataType ) * bufferCapacity );

			//create the vertex buffer
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory( &bufferDesc, sizeof( bufferDesc ) );
			bufferDesc.Usage = (D3D11_USAGE)bufferUsage;
			bufferDesc.ByteWidth = bufferSize;
			bufferDesc.BindFlags = (D3D11_BIND_FLAG)bufferType;
			bufferDesc.CPUAccessFlags = (D3D11_CPU_ACCESS_FLAG)cpuAccessFlags;

			D3D11_SUBRESOURCE_DATA srcData;
			srcData.pSysMem = &bufferData[0];
			srcData.SysMemPitch = 0;
			srcData.SysMemSlicePitch = 0;

			ID3D11Buffer* buffer = nullptr;

			HRESULT hr = m_renderer->GetDevice()->CreateBuffer( &bufferDesc, &srcData, &buffer );
			if( FAILED( hr ) )
			{
				throw Kiwi::Exception( L"GPUBufferManager::CreateBuffer", L"Failed to create buffer: " + Kiwi::GetD3DErrorString( hr ) );
			}

			std::unique_ptr<Kiwi::GPUBuffer<BufferDataType>> kBuffer = std::make_unique<Kiwi::GPUBuffer<BufferDataType>>( *m_renderer, *buffer, bufferUsage, cpuAccessFlags, 0, bufferData.size(), bufferCapacity, false );

			auto itr = m_buffers.insert( std::make_pair( kBuffer->GetUID(), std::move( kBuffer ) ) );
			if( itr.second )
			{
				return dynamic_cast<Kiwi::GPUBuffer<BufferDataType>*>(itr.first->second.get());
			}

			return nullptr;
		}

		Kiwi::IGPUBuffer* _FindBuffer( long uid );

	public:

		GPUBufferManager( Kiwi::Renderer& renderer ) :
			m_renderer(&renderer)
		{
		}

		~GPUBufferManager();

		template<typename BufferDataType>
		Kiwi::GPUBuffer<BufferDataType>* CopyBuffer( long uid )
		{
			return this->CopyBuffer( this->FindBuffer( uid ) );
		}

		template<typename BufferDataType>
		Kiwi::GPUBuffer<BufferDataType>* CopyBuffer( Kiwi::GPUBuffer<BufferDataType>* source )
		{
			if( source == nullptr ) return nullptr;

			std::lock_guard<std::mutex> lock( m_gpuBufferManager_M );

			return this->_CopyBuffer( source );
		}

		template<typename BufferDataType>
		Kiwi::GPUBuffer<BufferDataType>* CreateBuffer( unsigned long bufferCapacity, Kiwi::GPUBufferType bufferType, Kiwi::GPUBufferUsageFlags bufferUsage = Kiwi::GPUBufferUsageFlags::DEFAULT, Kiwi::GPUBufferAccessFlags cpuAccessFlags = Kiwi::GPUBufferAccessFlags::NONE )
		{
			std::lock_guard<std::mutex> lock( m_gpuBufferManager_M );

			return this->_CreateBuffer<BufferDataType>( bufferCapacity, bufferType, bufferUsage, cpuAccessFlags );
		}

		template<typename BufferDataType>
		Kiwi::GPUBuffer<BufferDataType>* CreateBuffer( const std::vector<BufferDataType>& bufferData, unsigned long bufferCapacity, Kiwi::GPUBufferType bufferType, Kiwi::GPUBufferUsageFlags bufferUsage = Kiwi::GPUBufferUsageFlags::DEFAULT, Kiwi::GPUBufferAccessFlags cpuAccessFlags = Kiwi::GPUBufferAccessFlags::NONE )
		{
			std::lock_guard<std::mutex> lock( m_gpuBufferManager_M );

			return this->_CreateBuffer<BufferDataType>( bufferData, bufferCapacity, bufferType, bufferUsage, cpuAccessFlags );
		}

		Kiwi::IGPUBuffer* FindBuffer( long uid );

		void Update();

		void Shutdown();

	};
}

#endif