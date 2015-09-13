#include "IShader.h"
#include "Renderer.h"

#include "../Core/Exception.h"
#include "../Core/Utilities.h"

namespace Kiwi
{

	IShader::IShader( std::wstring shaderName, Kiwi::Renderer* renderer, std::wstring vertexShaderFile, std::wstring pixelShaderFile )
	{

		assert( renderer != 0 );

		m_numPixelBuffers = 0;
		m_numSamplers = 0;
		m_numVertexBuffers = 0;
		m_samplers = 0;
		m_pixelBuffers = 0;
		m_vertexBuffers = 0;
		m_vertexBlob = 0;
		m_pixelBlob = 0;
		m_vertexShader = 0;
		m_pixelShader = 0;
		m_inputLayout = 0;

		m_shaderName = shaderName;
		m_vertexShaderFile = vertexShaderFile;
		m_pixelShaderFile = pixelShaderFile;

		//first load the compiled shaders from the cso files
		//load vertex shader
		HRESULT hr = D3DReadFileToBlob( vertexShaderFile.c_str(), &m_vertexBlob );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"IShader", L"[" + m_shaderName + L"] Call to D3DReadFileToBlob failed for file '" + vertexShaderFile + L"': " + Kiwi::GetD3DErrorString( hr ) );
		}
		//load pixel shader
		hr = D3DReadFileToBlob( pixelShaderFile.c_str(), &m_pixelBlob );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"IShader", L"[" + m_shaderName + L"] Call to D3DReadFileToBlob failed for file '" + pixelShaderFile + L"': " + Kiwi::GetD3DErrorString( hr ) );
		}

		//create the vertex shader from the compiled data
		hr = renderer->GetDevice()->CreateVertexShader( m_vertexBlob->GetBufferPointer(), m_vertexBlob->GetBufferSize(), NULL, &m_vertexShader );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"IShader", L"[" + m_shaderName + L"] Call to CreateVertexShader failed: " + Kiwi::GetD3DErrorString( hr ) );
		}

		//create the pixel shader from the compiled data
		hr = renderer->GetDevice()->CreatePixelShader( m_pixelBlob->GetBufferPointer(), m_pixelBlob->GetBufferSize(), NULL, &m_pixelShader );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"IShader", L"[" + m_shaderName + L"] Call to CreatePixelShader failed: " + Kiwi::GetD3DErrorString( hr ) );
		}

	}

	IShader::~IShader()
	{

		for( unsigned int i = 0; i < m_numSamplers; i++ )
		{
			SAFE_RELEASE( m_samplers[i] );
		}

		for( unsigned int i = 0; i < m_numPixelBuffers; i++ )
		{
			SAFE_RELEASE( m_pixelBuffers[i] );
		}

		for( unsigned int i = 0; i < m_numVertexBuffers; i++ )
		{
			SAFE_RELEASE( m_vertexBuffers[i] );
		}

		SAFE_RELEASE( m_inputLayout );
		SAFE_RELEASE( m_vertexBlob );
		SAFE_RELEASE( m_pixelBlob );
		SAFE_RELEASE( m_vertexShader );
		SAFE_RELEASE( m_pixelShader );

	}

	void IShader::_CreateInputLayout( Kiwi::Renderer* renderer, D3D11_INPUT_ELEMENT_DESC* polygonLayout, unsigned int layoutSize, ID3D11InputLayout** layout )
	{

		assert( renderer != 0 );

		HRESULT hr = renderer->GetDevice()->CreateInputLayout(  polygonLayout, // pointer to the layout description
																layoutSize, // number of elements in the layout
																m_vertexBlob->GetBufferPointer(), // pointer to the vertex shader
																m_vertexBlob->GetBufferSize(), // length of the shader file
																layout ); // the created object
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"Shader", L"[" + m_shaderName + L"] Call to CreateInputLayout failed: " + Kiwi::GetD3DErrorString( hr ) );
		}

	}

	void IShader::_CreateSampler( Kiwi::Renderer* renderer, D3D11_SAMPLER_DESC samplerDesc, ID3D11SamplerState** sampler )
	{

		assert( renderer != 0 );

		//create a new sampler state
		HRESULT hr = renderer->GetDevice()->CreateSamplerState( &samplerDesc, sampler );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"Shader", L"[" + m_shaderName + L"] Call to CreateSamplerState failed: " + Kiwi::GetD3DErrorString( hr ) );
		}

	}

	void IShader::_CreateBuffer( Kiwi::Renderer* renderer, D3D11_BUFFER_DESC bufferDesc, ID3D11Buffer** buffer )
	{

		assert( renderer != 0 );

		// create a new constant buffer
		HRESULT hr = renderer->GetDevice()->CreateBuffer( &bufferDesc, NULL, buffer );
		if( FAILED( hr ) )
		{
			throw Kiwi::Exception( L"Shader", L"[" + m_shaderName + L"] Call to CreateBuffer failed: " + Kiwi::GetD3DErrorString( hr ) );
		}

	}

	void IShader::Bind( Kiwi::Renderer* renderer )
	{

		assert( renderer != 0 );

		//bind the vertex and pixel shaders to use
		renderer->GetDeviceContext()->VSSetShader( m_vertexShader, NULL, 0 );
		renderer->GetDeviceContext()->PSSetShader( m_pixelShader, NULL, 0 );

		//bind the vertex input layout
		renderer->GetDeviceContext()->IASetInputLayout( m_inputLayout );

		//bind all of the vertex buffers
		if( m_numVertexBuffers > 0 )
		{
			renderer->GetDeviceContext()->VSSetConstantBuffers( 0, m_numVertexBuffers, m_vertexBuffers );
		}
		//bind all of the pixel buffers
		if( m_numPixelBuffers > 0 )
		{
			renderer->GetDeviceContext()->PSSetConstantBuffers( 0, m_numPixelBuffers, m_pixelBuffers );
		}
		//bind all of the samplers
		if( m_numSamplers > 0 )
		{
			renderer->GetDeviceContext()->PSSetSamplers( 0, m_numSamplers, m_samplers );
		}

	}

}