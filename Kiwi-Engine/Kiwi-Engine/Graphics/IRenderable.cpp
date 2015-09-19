#include "IRenderable.h"
//#include "IShaderEffect.h"

#include "../Core/Exception.h"
#include "../Core/Utilities.h"
#include "../Core/Assert.h"

namespace Kiwi
{

	IRenderable::IRenderable(std::wstring name, Kiwi::Entity* parentEntity, const Kiwi::Mesh& mesh):
		m_mesh(mesh)
	{

		if( parentEntity == 0 )
		{
			throw Kiwi::Exception( L"IRenderable", L"["+name+L"] Renderable initialized with no parent entity" );
		}

		m_parentEntity = parentEntity;
		m_renderableName = name;
		//m_shaderEffect = 0;
		m_renderType = RENDERABLE_3D;
		m_renderTarget = L"BackBuffer";
		m_currentMeshSubset = 0;

	}

	IRenderable::~IRenderable()
	{

		//SAFE_DELETE( m_mesh );
		//SAFE_DELETE( m_shaderEffect );

	}

	/*void IRenderable::SetEffect( Kiwi::IShaderEffect* effect )
	{

		if( m_shaderEffect )
		{
			SAFE_DELETE( m_shaderEffect );
		}

		m_shaderEffect = effect;

	}*/

	void IRenderable::SetMesh( const Kiwi::Mesh& mesh )
	{
		
		m_mesh = mesh;

	}

	//sets one of the mesh's subsets as the currently active/rendering subset
	void IRenderable::SetCurrentMeshSubset( unsigned int subsetIndex )
	{

		if( subsetIndex <= m_mesh.GetSubsetCount() )
		{
			m_currentMeshSubset = subsetIndex;
		}

	}

	Kiwi::Mesh::Subset* IRenderable::GetCurrentMeshSubset()
	{

		return m_mesh.GetSubset( m_currentMeshSubset );

	}

	/*std::wstring IRenderable::GetShader()const
	{

		return (m_shaderEffect != 0) ? m_shaderEffect->GetShaderName() : L"";

	}*/

};