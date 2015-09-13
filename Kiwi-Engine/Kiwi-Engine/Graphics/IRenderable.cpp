#include "IRenderable.h"
//#include "IShaderEffect.h"

#include "../Core/Exception.h"
#include "../Core/Utilities.h"
#include "../Core/Assert.h"

namespace Kiwi
{

	IRenderable::IRenderable(std::wstring name, Kiwi::Entity* parentEntity )
	{

		if( parentEntity == 0 )
		{
			throw Kiwi::Exception( L"IRenderable", L"["+name+L"] Renderable initialized with no parent entity" );
		}

		m_parentEntity = parentEntity;
		m_renderableName = name;
		//m_shaderEffect = 0;
		//m_mesh = 0;
		m_renderType = RENDERABLE_3D;
		m_renderTarget = L"BackBuffer";

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

	/*void IRenderable::SetMesh( Kiwi::Mesh* mesh )
	{
		
		if( mesh != 0 )
		{
			if( m_mesh != 0 )
			{
				*m_mesh = *mesh;

			} else
			{
				m_mesh = new Kiwi::Mesh( *mesh );
			}
		}

	}*/

	/*std::wstring IRenderable::GetShader()const
	{

		return (m_shaderEffect != 0) ? m_shaderEffect->GetShaderName() : L"";

	}*/

};