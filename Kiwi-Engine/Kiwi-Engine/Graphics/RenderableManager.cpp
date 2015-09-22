#include "RenderableManager.h"
#include "IRenderable.h"
#include "IShaderEffect.h"
#include "RenderTarget.h"
#include "Renderer.h"

#include "../Core/Assert.h"
#include "../Core/Scene.h"

namespace Kiwi
{

	RenderableManager::RenderableManager()
	{

	}

	RenderableManager::~RenderableManager()
	{

	}

	void RenderableManager::AddRenderable( Kiwi::IRenderable* renderable )
	{

		if( renderable == 0 ) return;

		switch( renderable->GetRenderType() )
		{
			case IRenderable::RenderType::RENDER_3D:
				{

					Kiwi::IShaderEffect* effect = renderable->GetEffect();
					Kiwi::Mesh* mesh = &renderable->GetMesh();
					if( effect != 0 && mesh != 0 )
					{

						if( mesh->HasTransparency() == true )
						{
							//add the renderable to the list of 3d opaque renderables
							//sorted by     3D   >>    RenderTarget            >>                Shader            >>          Mesh         >>       No Transparency     >>     renderable
							m_3DRenderables.rtMap[renderable->GetRenderTarget()].shaderMap[effect->GetShaderName()].meshMap[mesh->GetAssetName()].opaque_renderables.push_back( renderable );
						} else
						{
							//add the renderable to the list of 3d transparent renderables
							//sorted by     3D   >>    RenderTarget            >>                Shader            >>          Mesh         >>         Transparency          >>     renderable
							m_3DRenderables.rtMap[renderable->GetRenderTarget()].shaderMap[effect->GetShaderName()].meshMap[mesh->GetAssetName()].transparent_renderables.push_back( renderable );
						}

					}

					break;
				}
			case IRenderable::RenderType::RENDER_2D:
				{

					Kiwi::IShaderEffect* effect = renderable->GetEffect();
					Kiwi::Mesh* mesh = &renderable->GetMesh();
					if( effect != 0 && mesh != 0 )
					{

						if( mesh->HasTransparency() == false )
						{
							//add the renderable to the list of 2d opaque renderables
							//sorted by     2D   >>    RenderTarget           >>                Shader            >>          Mesh         >>       No Transparency     >>     renderable
							m_2DRenderables.rtMap[renderable->GetRenderTarget()].shaderMap[effect->GetShaderName()].meshMap[mesh->GetAssetName()].opaque_renderables.push_back( renderable );
						} else
						{
							//add the renderable to the list of 2d transparent renderables
							//sorted by    2D    >>    RenderTarget           >>                Shader            >>          Mesh         >>         Transparency           >>     renderable
							m_2DRenderables.rtMap[renderable->GetRenderTarget()].shaderMap[effect->GetShaderName()].meshMap[mesh->GetAssetName()].transparent_renderables.push_back( renderable );
						}

					}

					break;
				}
			default: break;
		}

	}

	void RenderableManager::Remove( Kiwi::IRenderable* renderable )
	{



	}

	void RenderableManager::RemoveWithName( std::wstring name )
	{



	}

	void RenderableManager::RemoveAll()
	{

		m_3DRenderables.rtMap.clear();
		m_2DRenderables.rtMap.clear();

	}

	//Kiwi::IRenderable* RenderableManager::FindWithName( std::wstring name )
	//{
	//}

	//std::vector<Kiwi::IRenderable*> RenderableManager::FindAllWithTag( std::wstring tag )
	//{
	//}

}