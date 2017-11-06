#ifndef _KIWI_RENDERQUEUE_H_
#define _KIWI_RENDERQUEUE_H_

#include "IModel.h"

#include "../Core/Entity.h"

#include "..\Utilities\UIDPool.h"

#include <string>
#include <unordered_map>
#include <set>
#include <mutex>

namespace Kiwi
{
	class Scene;
	class RenderQueueGroup;
	class RenderTarget;

	using RenderGroupMap = std::unordered_map<unsigned long, std::unique_ptr<Kiwi::RenderQueueGroup>>;

	struct RenderableBitmask
	{
		static const long long Material = 0x00000000000FFFFF;
		static const long long Shader = 0x000000001FF00000;
		static const long long Depth = 0x0001FFFFE0000000;
		static const long long TranslucencyType = 0x0006000000000000;
		static const long long ViewportIndex = 0x0038000000000000;
		static const long long RenderGroup = 0x7FC0000000000000;
		static const long long Is2D = 0x8000000000000000;

		struct Bit
		{
			static const char Material = 0;
			static const char Shader = 20;
			static const char Depth = 29;
			static const char TranslucencyType = 49;
			static const char ViewportIndex = 51;
			static const char RenderGroup = 54;
			static const char Is2D = 63;
		};
	};

	struct Renderable
	{
		/*2d? (1)|render group id (9)|viewport index(3)|trans. type(2)|depth(20)|shader id (9)|material id(20)*/
		long long renderKey;
		unsigned int subsetIndex;
		Kiwi::IModel* model;

		Renderable( Kiwi::IModel& model, unsigned int subsetIndex, char is2D, unsigned short renderGroupID,  char viewportIndex, long depthFromCamera )
		{
			renderKey = 0;
			this->subsetIndex = subsetIndex;
			this->model = &model;

			//material ID: 20 bits (0-19)
			renderKey |= (RenderableBitmask::Material & model.GetSubset( subsetIndex )->GetMaterial()->GetUID());

			//shader id: 9 bits (20-28)
			renderKey |= (RenderableBitmask::Shader & ((long long)model.GetSubset( subsetIndex )->GetMaterial()->GetShaderID() << RenderableBitmask::Bit::Shader));

			//depth (from camera): 20 bits (29-48)
			renderKey |= (RenderableBitmask::Depth & ((long long)depthFromCamera << RenderableBitmask::Bit::Depth));

			//translucency type: 2 bits (49-50)
			renderKey |= (RenderableBitmask::TranslucencyType & ((long long)model.GetSubset( subsetIndex )->GetMaterial()->GetTranslucencyType() << RenderableBitmask::Bit::TranslucencyType));

			//viewport index: 3 bits (51-53)
			renderKey |= (RenderableBitmask::ViewportIndex & ((long long)viewportIndex << RenderableBitmask::Bit::ViewportIndex));

			//render group id: 9 bits (54-62)
			renderKey |= (RenderableBitmask::RenderGroup & ((long long)renderGroupID << RenderableBitmask::Bit::RenderGroup));

			//is 2d: 1 bit (63)
			renderKey |= (RenderableBitmask::Is2D & ((long long)is2D << RenderableBitmask::Bit::Is2D));
		}
	};

	struct RenderGroup
	{
		std::wstring name; //unique name
		unsigned short id; //unique id

		/*index of the viewport the models in this group are rendered to*/
		unsigned short viewportIndex;

		RenderGroup( const std::wstring n, unsigned short i, unsigned short v )
		{
			name = n;
			id = i;
			viewportIndex = v;
		}
	};

	class RenderQueue
	{
	protected:

		std::unordered_set<Kiwi::Mesh*> m_meshList;

		std::unordered_set<Kiwi::IModel*> m_existingModelList;

		std::unordered_map<std::wstring, RenderGroup> m_renderGroups;

		Kiwi::UIDPool<unsigned short> m_renderGroupUIDPool;

		Kiwi::RenderTarget* m_renderTarget;

		std::vector<Renderable> m_renderQueue;

		Kiwi::RenderQueueGroup* m_defaultGroup;

		std::mutex m_renderQueue_M;

		unsigned int m_defaultPriority; //default priority to use for renderables that do not specify a priority

		unsigned int m_lastQueueSize;

	public:

		RenderQueue();

		RenderQueue( Kiwi::RenderTarget& renderTarget );

		~RenderQueue();

		void AddMesh( Kiwi::Mesh& mesh );

		void AddMeshes( std::vector<Kiwi::Mesh*> meshes );

		std::vector<Renderable>::iterator Begin() { return m_renderQueue.begin(); }

		void Build();

		/*empties the render queue of all meshes and renderqueuegroups*/
		virtual void Clear();

		void CreateRenderGroup( const std::wstring& groupName, unsigned short viewportIndex );

		void DestroyRenderGroup( const std::wstring& groupName );

		std::vector<Renderable>::iterator End() { return m_renderQueue.end(); }

		RenderGroup* FindRenderGroup( const std::wstring& groupName );

		void OnPostRender();

		void OnPreRender();

		void RemoveMesh( Kiwi::Mesh& mesh );

		/*empties the render queue so it can be rebuilt*/
		virtual void Reset();
	};
}

#endif