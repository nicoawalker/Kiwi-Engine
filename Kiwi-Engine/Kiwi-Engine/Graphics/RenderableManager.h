#ifndef _KIWI_RENDERABLEMANAGER_H_
#define _KIWI_RENDERABLEMANAGER_H_

#include <unordered_map>
#include <vector>

namespace Kiwi
{

	class IRenderable;
	class Scene;

	class RenderableManager
	{
	public:

		struct RenderableListPair
		{
			std::vector<Kiwi::IRenderable*> opaque_renderables;
			std::vector<Kiwi::IRenderable*> transparent_renderables;
		};

		struct RenderableMeshMap
		{
			//stores the name of a mesh, and a vector of renderable pairs that store the opaque and transparent objects using that mesh
			std::unordered_map<std::wstring, RenderableListPair> meshMap;
		};

		struct RenderableShaderMap
		{
			//stores the name of a shader, and a RenderableMeshMap of renderables that use the shader
			std::unordered_map<std::wstring, RenderableMeshMap> shaderMap;

			//stores the shaders for the 2d renderables using this render target
			std::unordered_map<std::wstring, RenderableMeshMap> shaderMap2D;
		};

		struct RenderableMap
		{
			//stores the name of a render target and a RenderableShaderMap of renderables that should be drawn onto that render target
			std::unordered_map<std::wstring, RenderableShaderMap> rtMap;
		};

	protected:

		RenderableMap m_renderables;

	public:

		RenderableManager();
		~RenderableManager();

		void AddRenderable( Kiwi::IRenderable* renderable );

		void Remove( Kiwi::IRenderable* renderable );
		void RemoveWithName( std::wstring name );

		void RemoveAll();

		//Kiwi::IRenderable* FindWithName( std::wstring name );
		//std::vector<Kiwi::IRenderable*> FindAllWithTag( std::wstring tag );

		RenderableManager::RenderableMap* GetRenderables() { return &m_renderables; }

	};
}

#endif