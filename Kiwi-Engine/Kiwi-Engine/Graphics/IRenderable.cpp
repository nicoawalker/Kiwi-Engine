#include "IRenderable.h"
#include "IShaderEffect.h"

#include "../Core/Exception.h"
#include "../Core/Utilities.h"
#include "../Core/Assert.h"

namespace Kiwi
{

	IRenderable::IRenderable( std::wstring name, Kiwi::Entity* parentEntity, Kiwi::Mesh* mesh, std::wstring shader )
	{

		if( parentEntity == 0 )
		{
			throw Kiwi::Exception( L"IRenderable", L"[" + name + L"] Renderable initialized with no parent entity" );
		}

		m_parentEntity = parentEntity;
		m_renderableName = name;
		m_shaderEffect = 0;
		m_renderType = RENDER_3D;
		m_mesh = mesh;
		m_renderTarget = L"BackBuffer";
		m_currentMeshSubset = 0;
		m_shader = shader;

	}

	IRenderable::IRenderable(std::wstring name, Kiwi::Entity* parentEntity, Kiwi::Mesh* mesh, Kiwi::IShaderEffect* effect )
	{

		if( parentEntity == 0 )
		{
			throw Kiwi::Exception( L"IRenderable", L"["+name+L"] Renderable initialized with no parent entity" );
		}

		m_parentEntity = parentEntity;
		m_renderableName = name;
		m_shaderEffect = effect;
		m_mesh = mesh;
		if( m_shaderEffect ) m_shader = m_shaderEffect->GetShaderName();
		m_renderType = RENDER_3D;
		m_renderTarget = L"BackBuffer";
		m_currentMeshSubset = 0;

	}

	IRenderable::~IRenderable()
	{

		SAFE_DELETE( m_mesh );
		SAFE_DELETE( m_shaderEffect );

	}

	/*void IRenderable::SetEffect( Kiwi::IShaderEffect* effect )
	{

		if( m_shaderEffect )
		{
			SAFE_DELETE( m_shaderEffect );
		}

		m_shaderEffect = effect;

	}*/

	void IRenderable::SetMesh( Kiwi::Mesh* mesh )
	{
		
		m_mesh = mesh;

	}

	//sets one of the mesh's subsets as the currently active/rendering subset
	void IRenderable::SetCurrentMeshSubset( unsigned int subsetIndex )
	{

		if( m_mesh )
		{
			if( subsetIndex <= m_mesh->GetSubsetCount() )
			{
				m_currentMeshSubset = subsetIndex;
			}
		}

	}

	Kiwi::Mesh::Subset* IRenderable::GetCurrentMeshSubset()
	{

		return (m_mesh) ? m_mesh->GetSubset( m_currentMeshSubset ) : 0;

	}

	/*std::wstring IRenderable::GetShader()const
	{

		return (m_shaderEffect != 0) ? m_shaderEffect->GetShaderName() : L"";

	}*/

};