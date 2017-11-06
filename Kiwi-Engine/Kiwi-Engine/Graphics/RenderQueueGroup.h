#ifndef _KIWI_RENDERQUEUEGROUP_H_
#define _KIWI_RENDERQUEUEGROUP_H_

#include "IModel.h"

#include <vector>
#include <set>

namespace Kiwi
{

	class Scene;
	class Viewport;
	class Mesh;
	class IShader;

	struct ModelSubsetPair;
	struct ModelSubsetPairHash;
	struct ModelSubsetPairComparator;

	class RenderQueueGroup
	{
	public:

		struct ModelSubsetPair
		{
			Kiwi::IModel* model;
			Kiwi::IModel::Subset* subset;
			unsigned int subsetIndex;

			ModelSubsetPair( Kiwi::IModel* m, Kiwi::IModel::Subset* s, unsigned int sIndex )
			{
				model = m;
				subset = s;
				subsetIndex = sIndex;
			}
		};

		struct ModelSubsetPairHash
		{
			size_t operator()( const ModelSubsetPair& p1 )const
			{
				return std::hash<Kiwi::IModel*>()(p1.model);
			}
		};

		struct ModelSubsetPairComparator
		{
			bool operator()( const ModelSubsetPair& p1, const ModelSubsetPair& p2 )const
			{
				return p1.model == p2.model && p1.subset == p2.subset;
			}
		};

		using ModelQueue = std::vector<ModelSubsetPair>;

	protected:

		/*unique ID of the render group*/
		unsigned long m_groupID;

		/*unique string-based name for the group*/
		std::wstring m_groupName;

		//Kiwi::SubmeshList m_sMeshes; //solid meshes
		//Kiwi::SubmeshList m_tMeshes; //transparent meshes
		//Kiwi::SubmeshList m_2DMeshes; //2d meshes

		/*comparator for sorting submeshes by mesh ID*/
		/*struct SubmeshNameComparator
		{
			bool operator() ( Kiwi::Submesh* sm1, Kiwi::Submesh* sm2 )
			{
				return sm1->parent->GetID() == sm2->parent->GetID();
			}
		};*/

		/*stores all of the submeshes in the render group
		the outer vector contains 3 elements, 0 is solid meshes, 1 is transparent meshes, and 2 is 2d meshes
		the key of the first unordered map is the name of the shader used by all submeshes in the map
		the key of the second unordered map is the name of the material used by all submeshes in the map
		the inner vector contains the submeshes sorted by the mesh they belong to (sorted vector is faster than set in this case)*/
		//std::vector<std::unordered_map<std::wstring, std::unordered_map<std::wstring, std::vector<Kiwi::Submesh*>>>> m_meshQueue;

		//std::vector<Kiwi::SubmeshList> m_meshQueue;

		//stores the model pairs that have already been added so they aren't added multiple times
		//std::unordered_set<ModelSubsetPair> m_queuedModels;

		/*stores shared models that need to have their instance data updated*/
		std::unordered_set<Kiwi::IModel*> m_sharedModels;

		std::vector<ModelSubsetPair> m_solidModelQueue;
		std::vector<ModelSubsetPair> m_transparentModelQueue;
		std::vector<ModelSubsetPair> m_2dModelQueue;

		/*keeps track of whether the group has been changed since the last sort*/
		bool m_hasChanged;

	public:

		RenderQueueGroup( std::wstring groupName, unsigned long groupID );
		~RenderQueueGroup();

		void AddMesh( Kiwi::Mesh* mesh );

		//sorts all of the renderables in the group relative to the passed viewport
		void Sort( Kiwi::Viewport& viewport );

		std::wstring GetName()const { return m_groupName; }

		unsigned long GetID()const { return m_groupID; }

		ModelQueue& GetSolidModelQueue() { return m_solidModelQueue; }
		ModelQueue& GetTransparentModelQueue() { return m_transparentModelQueue; }
		ModelQueue& Get2DModelQueue() { return m_2dModelQueue; }

		/*returns a vector containing all the solid meshes in the render queue group*/
		//Kiwi::SubmeshList& GetSolids() { return m_meshQueue[0]; }

		/*returns a vector containing all the transparent meshes in the render queue group*/
		//Kiwi::SubmeshList& GetTransparents() { return m_meshQueue[1]; }

		/*returns a vector containing all the 2D meshes in the render queue group*/
		//Kiwi::SubmeshList& Get2D() { return m_meshQueue[2]; }

		//empties the group
		void Clear();
	};

	

}

#endif