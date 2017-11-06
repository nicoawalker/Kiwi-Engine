#include "Basic2DShader.h"

#include "..\Renderer.h"
#include "../Material.h"
#include "..\Mesh.h"
#include "../Camera.h"
#include "..\IModel.h"

#include "..\UI\UITextbox.h"

#include "../../Core/Scene.h"

namespace Kiwi
{

	Basic2DShader::Basic2DShader( const std::wstring shaderName, const std::wstring renderer, const std::wstring vsFile, const std::wstring psFile ) :
		Kiwi::IShader( shaderName, L"Basic2DShader",renderer, vsFile, psFile )
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
		vobDesc.ByteWidth = sizeof( Vertex_ObjectBuffer );
		vobDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		vobDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // allow the cpu to write data to the buffer
		vobDesc.MiscFlags = 0;
		vobDesc.StructureByteStride = 0;

		m_vBuffers.push_back( m_renderer->CreateBuffer( &vobDesc, 0 ) );

		//create the material settings object buffer
		D3D11_BUFFER_DESC msDesc;
		msDesc.Usage = D3D11_USAGE_DYNAMIC;
		msDesc.ByteWidth = sizeof( MaterialSettings_PS );
		if( msDesc.ByteWidth % 16 != 0 ) msDesc.ByteWidth += 16 - (msDesc.ByteWidth % 16); //fix buffer size to keep it as a multiple of 16
		msDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		msDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		msDesc.MiscFlags = 0;
		msDesc.StructureByteStride = 0;

		m_pBuffers.push_back( m_renderer->CreateBuffer( &msDesc, 0 ) );

		//create the pixel object buffer
		D3D11_BUFFER_DESC pobDesc;
		pobDesc.Usage = D3D11_USAGE_DYNAMIC;
		pobDesc.ByteWidth = sizeof( ObjectSettings_PS );
		if( pobDesc.ByteWidth % 16 != 0 ) pobDesc.ByteWidth += 16 - (pobDesc.ByteWidth % 16); //fix buffer size to keep it as a multiple of 16
		pobDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		pobDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		pobDesc.MiscFlags = 0;
		pobDesc.StructureByteStride = 0;

		m_pBuffers.push_back( m_renderer->CreateBuffer( &pobDesc, 0 ) );

		D3D11_SAMPLER_DESC sDesc;
		//filter that decides which pixels to use to display the texture
		sDesc.Filter = D3D11_FILTER_ANISOTROPIC;

		//Method to use for resolving a u texture coordinate that is outside the 0 to 1 range
		sDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		sDesc.MipLODBias = 0.0f; //Offset from the calculated mipmap level. For example, if Direct3D calculates that a texture should be sampled at mipmap level 3 and MipLODBias is 2, then the texture will be sampled at mipmap level 5.
		sDesc.MaxAnisotropy = 1; //Clamping value used if D3D11_FILTER_ANISOTROPIC or D3D11_FILTER_COMPARISON_ANISOTROPIC is specified in Filter. Valid values are between 1 and 16.
		sDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS; //A function that compares sampled data against existing sampled data. The function options are listed in D3D11_COMPARISON_FUNC. 
		
		//Border color to use if D3D11_TEXTURE_ADDRESS_BORDER is specified for AddressU, AddressV, or AddressW.Range must be between 0.0 and 1.0 inclusive.
		sDesc.BorderColor[0] = 0;
		sDesc.BorderColor[1] = 0;
		sDesc.BorderColor[2] = 0;
		sDesc.BorderColor[3] = 0;

		sDesc.MinLOD = 0; //Lower end of the mipmap range to clamp access to, where 0 is the largest and most detailed mipmap level and any level higher than that is less detailed.
		sDesc.MaxLOD = D3D11_FLOAT32_MAX; //Upper end of the mipmap range to clamp access to, where 0 is the largest and most detailed mipmap level and any level higher than that is less detailed. This value must be greater than or equal to MinLOD. To have no upper limit on LOD set this to a large value such as D3D11_FLOAT32_MAX.

		m_samplerStates.push_back( m_renderer->CreateSampler( &sDesc ) );
	}

	void Basic2DShader::SetPerMaterial( Kiwi::Scene& scene, Kiwi::Material& material )
	{
		HRESULT hr;

		Kiwi::Color kDiffuseColor = material.GetColor( Material::COLOR_TYPE::DIFFUSE );
		DirectX::XMFLOAT4 diffuseColor( (float)kDiffuseColor.red, (float)kDiffuseColor.green, (float)kDiffuseColor.blue, (float)kDiffuseColor.alpha );
		float hasDiffuse = 0.0f;
		float diffuseIsRenderTexture = 0.0f;		

		Kiwi::Texture* matTexture = material.GetTexture( Material::TEXTURE_TYPE::DIFFUSE );
		if( matTexture != nullptr )
		{
			ID3D11ShaderResourceView* matSRV = matTexture->GetShaderResourceView();
			m_renderer->SetPixelShaderResources( 0, 1, &matSRV );
			hasDiffuse = 1.0f; //indicate to the shader that this material is using a diffuse texture
			if( matTexture->IsRenderTexture() )
			{
				diffuseIsRenderTexture = 1.0f; //indicate to the shader that this texture is a render texture
			}
		}

		ID3D11Buffer* matBuffer = m_pBuffers[0];
		if( matBuffer != nullptr )
		{
			D3D11_MAPPED_SUBRESOURCE mappedBuffer;
			MaterialSettings_PS* bufferPtr = 0;

			//map the constant buffer to gain write access
			hr = m_renderer->MapResource( matBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer );
			if( FAILED( hr ) )
			{
				MessageBox( NULL, Kiwi::ToWString(L"Failed to map per material buffer " + Kiwi::GetD3DErrorString(hr)).c_str(), L"Basic2DShader::SetPerMaterial", MB_OK );
				return;
			}

			//Get a pointer to the data in the constant buffer.
			bufferPtr = (MaterialSettings_PS*)mappedBuffer.pData;

			//Copy the new buffer data into the constant buffer
			bufferPtr->diffuseColor = diffuseColor;
			bufferPtr->diffuseIsRenderTexture = diffuseIsRenderTexture;
			bufferPtr->hasDiffuse = hasDiffuse;

			// Unlock the constant buffer.
			m_renderer->UnmapResource( matBuffer, 0 );

		} else
		{
			throw Kiwi::Exception( L"Basic2DShader::SetPerMaterial", L"[" + m_shaderName + L"] The pixel object buffer is null" );
		}
	}

	void Basic2DShader::SetPerObject( Kiwi::Scene& scene, Kiwi::Viewport& viewport, Kiwi::IModel& model, unsigned int modelSubsetIndex )
	{
		HRESULT hr;

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

		float renderingText = 0.0f;
		float textSharpening = 0.0f;
		float usePerVertexColor = 0.0f;

		Kiwi::UITextbox* text = dynamic_cast<Kiwi::UITextbox*>(entity);
		if( text != nullptr )
		{
			renderingText = 1.0f;
			usePerVertexColor = 1.0f;
			textSharpening = (float)text->GetTextSharpening();
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
			Vertex_ObjectBuffer* bufferPtr = 0;

			//map the constant buffer to gain write access
			hr = m_renderer->MapResource( vobBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer );
			if( FAILED( hr ) )
			{
				MessageBox( NULL, L"Failed to map per object vertex buffer", L"Basic2DShader::SetPerObject", MB_OK );
				return;
			}

			//Get a pointer to the data in the constant buffer.
			bufferPtr = (Vertex_ObjectBuffer*)mappedBuffer.pData;

			//Copy the new buffer data into the constant buffer
			bufferPtr->wvp = wvp;

			// Unlock the constant buffer.
			m_renderer->UnmapResource( vobBuffer, 0 );

		} else
		{
			throw Kiwi::Exception( L"Basic2DShader::SetPerObject", L"[" + m_shaderName + L"] The vertex object buffer is null" );
		}

		ID3D11Buffer* pobBuffer = m_pBuffers[1];
		if( pobBuffer )
		{
			D3D11_MAPPED_SUBRESOURCE mappedBuffer;
			ObjectSettings_PS* bufferPtr = 0;

			//map the constant buffer to gain write access
			hr = m_renderer->MapResource( pobBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer );
			if( FAILED( hr ) )
			{
				MessageBox( NULL, L"Failed to map per object pixel buffer", L"Basic2DShader::SetPerObject", MB_OK );
				return;
			}

			//Get a pointer to the data in the constant buffer.
			bufferPtr = (ObjectSettings_PS*)mappedBuffer.pData;

			//Copy the new buffer data into the constant buffer
			bufferPtr->renderingText = renderingText;
			bufferPtr->usePerVertexColor = usePerVertexColor;
			bufferPtr->textSharpening = textSharpening;

			// Unlock the constant buffer.
			m_renderer->UnmapResource( pobBuffer, 0 );

		} else
		{
			throw Kiwi::Exception( L"Basic2DShader::SetPerObject", L"[" + m_shaderName + L"] The pixel object buffer is null" );
		}
	}

}