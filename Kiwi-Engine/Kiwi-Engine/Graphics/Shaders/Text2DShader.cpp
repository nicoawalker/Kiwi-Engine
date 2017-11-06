#include "Text2DShader.h"

#include "..\Renderer.h"
#include "../Material.h"
#include "..\Mesh.h"
#include "../Camera.h"
#include "..\IModel.h"

#include "..\UI\UITextbox.h"

#include "../../Core/Scene.h"

namespace Kiwi
{

	Text2DShader::Text2DShader( const std::wstring shaderName, const std::wstring renderer, const std::wstring vsFile, const std::wstring psFile ) :
		Kiwi::IShader( shaderName, L"Text2DShader", renderer, vsFile, psFile )
	{

		try
		{
			D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			m_inputLayout = m_renderer->CreateInputLayout( this, layoutDesc, 4 );

			//create the vertex object buffer
			D3D11_BUFFER_DESC vobDesc;
			vobDesc.Usage = D3D11_USAGE_DYNAMIC; // dynamic as it will be updated each frame
			vobDesc.ByteWidth = sizeof( PerObjectData_VS );
			vobDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			vobDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // allow the cpu to write data to the buffer
			vobDesc.MiscFlags = 0;
			vobDesc.StructureByteStride = 0;

			m_vBuffers.push_back( m_renderer->CreateBuffer( &vobDesc, 0 ) );

			//create the text properties buffer
			D3D11_BUFFER_DESC pobDesc;
			pobDesc.Usage = D3D11_USAGE_DYNAMIC;
			pobDesc.ByteWidth = sizeof( TextProperties_PS );
			pobDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			pobDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			pobDesc.MiscFlags = 0;
			pobDesc.StructureByteStride = 0;

			m_pBuffers.push_back( m_renderer->CreateBuffer( &pobDesc, 0 ) );

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

		} catch( ... )
		{
			throw;
		}

	}

	void Text2DShader::SetPerMaterial( Kiwi::Scene& scene, Kiwi::Material& material )
	{
		HRESULT hr;

		//if( material.GetMesh() == nullptr ) return;

		//Kiwi::UITextbox* entity = dynamic_cast<Kiwi::UITextbox*>(material.GetMesh()->GetEntity());
		//if( entity == nullptr ) return;

		///*set the text sharpening value*/
		//DirectX::XMFLOAT4 textProperties( (float)entity->GetTextSharpening(), 0.0f, 0.0f, 0.0f );

		DirectX::XMFLOAT4 textProperties( 0.0f, 0.0f, 0.0f, 0.0f );

		static unsigned long lastTextureID = 0; //stores the ID of the last texture so we only switch textures when a new one needs to be set

		Kiwi::Texture* matTexture = material.GetTexture( Material::TEXTURE_TYPE::DIFFUSE );
		if( matTexture != nullptr )
		{
			ID3D11ShaderResourceView* matSRV = matTexture->GetShaderResourceView();
			m_renderer->SetPixelShaderResources( 0, 1, &matSRV );

		} else
		{
			return; //can't render text without a texture
		}

		ID3D11Buffer* matBuffer = m_pBuffers[0];
		if( matBuffer != nullptr )
		{
			D3D11_MAPPED_SUBRESOURCE mappedBuffer;
			TextProperties_PS* bufferPtr = 0;

			//map the constant buffer to gain write access
			hr = m_renderer->MapResource( matBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer );
			if( FAILED( hr ) )
			{
				MessageBox( NULL, Kiwi::ToWString( L"Failed to map per material buffer " + Kiwi::GetD3DErrorString( hr ) ).c_str(), L"Default2DShader::SetPerMaterial", MB_OK );
				return;
			}

			//Get a pointer to the data in the constant buffer.
			bufferPtr = (TextProperties_PS*)mappedBuffer.pData;

			//Copy the new buffer data into the constant buffer
			bufferPtr->properties = textProperties;

			// Unlock the constant buffer.
			m_renderer->UnmapResource( matBuffer, 0 );

		} else
		{
			throw Kiwi::Exception( L"DefaultShader::SetPerMaterial", L"[" + m_shaderName + L"] The pixel object buffer is null" );
		}
	}

	void Text2DShader::SetPerObject( Kiwi::Scene& scene, Kiwi::Viewport& viewport, Kiwi::IModel& model, unsigned int modelSubsetIndex )
	{
		Kiwi::Camera* camera = viewport.GetCamera();
		Kiwi::Entity* entity = (*(model.GetLinkedMeshes().begin()))->GetEntity();
		if( entity == nullptr )
		{
			return;
		}
		Kiwi::Transform* transform = entity->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );
		if( transform == nullptr || camera == nullptr || m_renderer == nullptr )
		{
			return;
		}

		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		Kiwi::Matrix4ToXMMATRIX( transform->GetWorldMatrix(), world );
		Kiwi::Matrix4ToXMMATRIX( camera->GetViewMatrix2D(), view );
		Kiwi::Matrix4ToXMMATRIX( camera->GetOrthoMatrix(), projection );

		//set the worldViewProject matrix
		DirectX::XMMATRIX wvp = world * view * projection;

		// lock the vertex object buffer so that it can be written to
		ID3D11Buffer* vobBuffer = m_vBuffers[0];
		if( vobBuffer )
		{
			D3D11_MAPPED_SUBRESOURCE mappedBuffer;
			PerObjectData_VS* bufferPtr = 0;

			//map the constant buffer to gain write access
			HRESULT hr = m_renderer->MapResource( vobBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer );
			if( FAILED( hr ) )
			{
				MessageBox( NULL, L"Failed to map per object buffer", L"Default2DShader::SetPerObject", MB_OK );
				return;
			}

			//Get a pointer to the data in the constant buffer.
			bufferPtr = (PerObjectData_VS*)mappedBuffer.pData;

			//Copy the new buffer data into the constant buffer
			bufferPtr->wvp = wvp;

			// Unlock the constant buffer.
			m_renderer->UnmapResource( vobBuffer, 0 );

		} else
		{
			throw Kiwi::Exception( L"Default2DShader::SetEntityParameters", L"[" + m_shaderName + L"] The vertex object buffer is null" );
		}
	}

}