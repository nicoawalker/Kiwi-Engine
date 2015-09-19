#ifndef _KIWI_RENDERABLE_H_
#define _KIWI_RENDERABLE_H_

#include "Mesh.h"

#include "../Core/Matrix4.h"

#include <vector>
#include <string>

namespace Kiwi
{

	class Renderer;
	class Scene;
	class Entity;
	//class IShaderEffect;

	class IRenderable
	{
	public:

		enum RENDER_TYPE { RENDERABLE_2D = 0, RENDERABLE_3D = 1 };

	protected:

		Kiwi::Entity* m_parentEntity;

		Kiwi::Mesh m_mesh;

		//Kiwi::IShaderEffect m_shaderEffect;

		std::wstring m_renderableName;
		std::wstring m_renderTarget;

		RENDER_TYPE m_renderType;

		//stores in the index value of the mesh subset returned from the GetCurrentMeshSubset function
		unsigned int m_currentMeshSubset;

	public:

		IRenderable( std::wstring name, Kiwi::Entity* parentEntity, const Kiwi::Mesh& mesh );
		virtual ~IRenderable();

		/*binds the renderable's mesh to the rendering pipeline*/
		virtual void Bind( Kiwi::Renderer* renderer ) { m_mesh.Bind( renderer ); }

		virtual void OnPreRender(Kiwi::Renderer* renderer, Kiwi::Scene* scene) {}
		virtual void OnPostRender(Kiwi::Renderer* renderer, Kiwi::Scene* scene) {}

		virtual void SetRenderTarget( std::wstring renderTarget ) { m_renderTarget = renderTarget; }
		//virtual void SetEffect( const Kiwi::IShaderEffect& effect );
		virtual void SetMesh( const Kiwi::Mesh& mesh );

		//sets one of the mesh's subsets as the currently active/rendering subset
		virtual void SetCurrentMeshSubset( unsigned int subsetIndex );

		virtual Kiwi::Mesh::Subset* GetCurrentMeshSubset();
		
		virtual std::wstring GetName()const { return m_renderableName; }
		virtual Kiwi::Mesh& GetMesh() { return m_mesh; }
		virtual RENDER_TYPE GetRenderType()const { return m_renderType; }
		virtual std::wstring GetRenderTarget()const { return m_renderTarget; }
		virtual Kiwi::Entity* GetParentEntity()const { return m_parentEntity; }
		//virtual const Kiwi::IShaderEffect& GetEffect()const { return m_shaderEffect; }

	};
};

#endif