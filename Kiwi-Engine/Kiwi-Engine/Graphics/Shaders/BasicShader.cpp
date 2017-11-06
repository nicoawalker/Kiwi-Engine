#include "BasicShader.h"

#include "..\Renderer.h"
#include "../Material.h"
#include "..\Mesh.h"
#include "../Camera.h"
#include "..\IModel.h"

#include "..\..\Core\Scene.h"

namespace Kiwi
{

	BasicShader::BasicShader( const std::wstring shaderName, const std::wstring renderer, const std::wstring vsFile, const std::wstring psFile ):
		Kiwi::IShader( shaderName, L"BasicShader", renderer, vsFile, psFile )
	{
		D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
		{
			/*vertex data*/
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		m_inputLayout = m_renderer->CreateInputLayout( this, layoutDesc, 6 );

		//create the vertex object buffer
		D3D11_BUFFER_DESC vobDesc;
		vobDesc.Usage = D3D11_USAGE_DYNAMIC; // dynamic as it will be updated each frame
		vobDesc.ByteWidth = sizeof( Vertex_ObjectBuffer );
		vobDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		vobDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // allow the cpu to write data to the buffer
		vobDesc.MiscFlags = 0;
		vobDesc.StructureByteStride = 0;

		m_vBuffers.push_back( m_renderer->CreateBuffer( &vobDesc, 0 ) );

		//create the pixel object buffer
		D3D11_BUFFER_DESC pobDesc;
		pobDesc.Usage = D3D11_USAGE_DYNAMIC;
		pobDesc.ByteWidth = sizeof( Pixel_ObjectBuffer );
		pobDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		pobDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		pobDesc.MiscFlags = 0;
		pobDesc.StructureByteStride = 0;

		m_pBuffers.push_back( m_renderer->CreateBuffer( &pobDesc, 0 ) );

		//create the pixel frame buffer
		D3D11_BUFFER_DESC pfbDesc;
		pfbDesc.Usage = D3D11_USAGE_DYNAMIC;
		pfbDesc.ByteWidth = sizeof( Pixel_FrameBuffer );
		pfbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		pfbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		pfbDesc.MiscFlags = 0;
		pfbDesc.StructureByteStride = 0;

		m_pBuffers.push_back( m_renderer->CreateBuffer( &pfbDesc, 0 ) );

		D3D11_SAMPLER_DESC sDesc;
		//filter that decides which pixels to use to display the texture
		sDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		//any texture coordinates greater than 1.0f wrap around
		sDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sDesc.MipLODBias = 0.0f;
		sDesc.MaxAnisotropy = 1;
		sDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sDesc.BorderColor[0] = 0;
		sDesc.BorderColor[1] = 0;
		sDesc.BorderColor[2] = 0;
		sDesc.BorderColor[3] = 0;
		sDesc.MinLOD = 0;
		sDesc.MaxLOD = D3D11_FLOAT32_MAX;

		m_samplerStates.push_back( m_renderer->CreateSampler( &sDesc ) );
	}

	BasicShader::~BasicShader()
	{
	}

	void BasicShader::SetPerFrame( Kiwi::Scene& scene )
	{
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 direction;

		Kiwi::Vector4ToXMFLOAT4( scene.GetAmbientLight(), ambient );
		Kiwi::Vector4ToXMFLOAT4( scene.GetDiffuseLight(), diffuse );
		Kiwi::Vector4ToXMFLOAT4( scene.GetDiffuseLightDirection(), direction );

		Pixel_FrameBuffer buf;

		// lock the pixel frame buffer so that it can be written to
		ID3D11Buffer* pfbBuffer = m_pBuffers[1];
		if( pfbBuffer )
		{
			D3D11_MAPPED_SUBRESOURCE mappedBuffer;
			Pixel_FrameBuffer* bufferPtr = 0;

			//map the constant buffer to gain write access
			HRESULT hr = m_renderer->MapResource( pfbBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer );
			if( FAILED( hr ) )
			{
				MessageBox( NULL, L"Failed to map per frame buffer", L"BasicShader::SetPerFrame", MB_OK );
				return;
			}

			//Get a pointer to the data in the constant buffer.
			bufferPtr = (Pixel_FrameBuffer*)mappedBuffer.pData;

			//Copy the new buffer data into the constant buffer
			bufferPtr->ambientLight = ambient;
			bufferPtr->diffuseLight = diffuse;
			bufferPtr->lightDirection = direction;

			// Unlock the constant buffer.
			m_renderer->UnmapResource( pfbBuffer, 0 );
		}

	}

	void BasicShader::SetPerMaterial( Kiwi::Scene& scene, Kiwi::Material& material )
	{

		HRESULT hr;

		Kiwi::Color kDiffuseColor = material.GetColor( Material::COLOR_TYPE::DIFFUSE );
		DirectX::XMFLOAT4 diffuseColor( (float)kDiffuseColor.red, (float)kDiffuseColor.green, (float)kDiffuseColor.blue, (float)kDiffuseColor.alpha );
		DirectX::XMFLOAT4 isTextured( 0.0f, 0.0f, 0.0f, 0.0f );

		static unsigned long lastTextureID = 0; //stores the ID of the last texture so we only switch textures when a new one needs to be set

		if( material.IsTextured() )
		{
			ID3D11ShaderResourceView* matSRV[4] = { nullptr, nullptr, nullptr, nullptr };

			Kiwi::Texture* dif = material.GetTexture( Material::TEXTURE_TYPE::DIFFUSE );
			Kiwi::Texture* bump = material.GetTexture( Material::TEXTURE_TYPE::BUMP );
			Kiwi::Texture* spec = material.GetTexture( Material::TEXTURE_TYPE::SPECULAR );
			Kiwi::Texture* amb = material.GetTexture( Material::TEXTURE_TYPE::AMBIENT );
			if( dif )
			{
				matSRV[0] = dif->GetShaderResourceView();
				isTextured.x = 1.0f;
			}
			if( bump )
			{
				matSRV[1] = bump->GetShaderResourceView();
				isTextured.y = 1.0f;
			}
			if( spec )
			{
				matSRV[2] = spec->GetShaderResourceView();
				isTextured.z = 1.0f;
			}
			if( amb )
			{
				matSRV[3] = amb->GetShaderResourceView();
				isTextured.w = 1.0f;
			}

			m_renderer->SetPixelShaderResources( 0, 4, matSRV );
		}

		ID3D11Buffer* matBuffer = m_pBuffers[0];
		if( matBuffer )
		{

			D3D11_MAPPED_SUBRESOURCE mappedBuffer;
			Pixel_ObjectBuffer* bufferPtr = 0;

			//map the constant buffer to gain write access
			hr = m_renderer->MapResource( matBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer );
			if( FAILED( hr ) )
			{
				MessageBox( NULL, L"Failed to map per material buffer", L"BasicShader::SetPerMaterial", MB_OK );
				return;
			}

			//Get a pointer to the data in the constant buffer.
			bufferPtr = (Pixel_ObjectBuffer*)mappedBuffer.pData;

			//Copy the new buffer data into the constant buffer
			bufferPtr->diffuseColor = diffuseColor;
			bufferPtr->isTextured = isTextured;

			// Unlock the constant buffer.
			m_renderer->UnmapResource( matBuffer, 0 );

		} else
		{
			throw Kiwi::Exception( L"BasicShader::SetPerMaterial", L"[" + m_shaderName + L"] The pixel object buffer is null" );
		}

	}

	void BasicShader::SetPerObject( Kiwi::Scene& scene, Kiwi::Viewport& viewport, Kiwi::IModel& model, unsigned int modelSubsetIndex )
	{

		HRESULT hr;

		Kiwi::Camera* camera = viewport.GetCamera();
		Kiwi::Entity* entity = (*(model.GetLinkedMeshes().begin()))->GetEntity();
		if( entity == 0 )
		{
			return;
		}
		Kiwi::Transform* transform = entity->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );
		if( transform == 0 || camera == 0 || m_renderer == 0 )
		{
			return;
		}

		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		Kiwi::Matrix4ToXMMATRIX( transform->GetWorldMatrix(), world );
		Kiwi::Matrix4ToXMMATRIX( camera->GetViewMatrix(), view );
		Kiwi::Matrix4ToXMMATRIX( camera->GetProjectionMatrix(), projection );

		//set the worldViewProject matrix
		DirectX::XMMATRIX wvp = world * view * projection;

		// lock the vertex object buffer so that it can be written to
		ID3D11Buffer* vobBuffer = m_vBuffers[0];
		if( vobBuffer )
		{
			D3D11_MAPPED_SUBRESOURCE mappedBuffer;
			Vertex_ObjectBuffer* bufferPtr = 0;

			//map the constant buffer to gain write access
			hr = m_renderer->MapResource( vobBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer );
			if( FAILED( hr ) )
			{
				MessageBox( NULL, L"Failed to map per object buffer", L"BasicShader::SetPerObject", MB_OK );
				return;
			}

			//Get a pointer to the data in the constant buffer.
			bufferPtr = (Vertex_ObjectBuffer*)mappedBuffer.pData;

			//Copy the new buffer data into the constant buffer
			bufferPtr->world = world;
			bufferPtr->wvp = wvp;

			// Unlock the constant buffer.
			m_renderer->UnmapResource( vobBuffer, 0 );

		} else
		{
			throw Kiwi::Exception( L"BasicShader::SetPerObject", L"[" + m_shaderName + L"] The vertex object buffer is null" );
		}

	}

}