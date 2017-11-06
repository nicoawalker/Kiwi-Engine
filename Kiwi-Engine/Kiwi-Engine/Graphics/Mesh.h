#ifndef _KIWI_MESH_H_
#define _KIWI_MESH_H_

#include "Material.h"

#include "..\Types.h"

#include "../Core/Component.h"
#include "..\Core\Math.h"
#include "../Core/IAsset.h"
#include "..\Core\Vector2d.h"
#include "../Core/Vector3d.h"
#include "..\Core\IAssetConsumer.h"

#include "..\Events\EventHandling.h"

#include <vector>
#include <string>

namespace Kiwi
{

	//class StaticMeshAsset;
	class IMeshAsset;
	class Renderer;
	class RenderQueue;
	class IGPUBuffer;
	class IModel;

	struct D3DVertex;

	template<class BufferDataType> class GPUBuffer;

	class Mesh :
		public Kiwi::Component,
		public Kiwi::IAssetConsumer
	{
		friend class Material;
		friend class RenderQueue;
	public:

		struct Face
		{
			Kiwi::Vector3d v1, v2, v3; //vertices
			Kiwi::Color c1, c2, c3; //colors
			Kiwi::Vector3d n1, n2, n3; //normals
			unsigned long i1, i2, i3; //indices

			Face()
			{
				i1 = i2 = i3 = 0;
			}
		};

	private:

		/*numerical ID for the render group this mesh will be using. this is retrieved and set internally from the user-defined render group name*/
		unsigned short m_renderGroupID;

		/*id of the render target*/
		unsigned long m_renderTargetID;

	protected:

		Kiwi::IModel* m_meshModel;

		/*stores whether the mesh has been marked as shared - shared meshes are used by multiple entities at the same time
		shared meshes are automatically batched into instance buffers, unless the shared mesh was created as an instanced mesh,
		in which case it's instance buffer will be used instead*/
		bool m_isShared;

		/*specifies whether or not a fetched asset should be copied or used directly*/
		bool m_copyModelOnFetch;

		std::wstring m_renderGroup;

		std::wstring m_renderTarget;

		std::wstring m_modelName;

		/*index of the target viewport in the render target*/
		unsigned short m_viewportIndex;

		//keeps track of whether or not the mesh has been added to a render target queue
		bool m_addedToRenderTargetQueue;

	protected:

		/*adds the mesh to the mesh's target render target's render queue*/
		bool _AddToRenderTargetQueue();

		void _OnActivate();

		Kiwi::EventResponse _OnAssetFetched( Kiwi::IAsset* asset );

		void _OnAttached();

		void _OnDeactivate();

		void _OnShutdown();
		
		void _OnStart();

		/*removes the mesh from the mesh's target render target's render queue*/
		void _RemoveFromRenderTargetQueue();

		bool _SetRenderTarget( const std::wstring& renderTarget );

		void _UnsetRenderTarget();

	public:

		Mesh( Kiwi::Scene& scene, const std::wstring& name );

		Mesh( Kiwi::Scene& scene, const std::wstring& name, const std::wstring& model );

		virtual ~Mesh();

		/*empties the vertex, normal, color, uv, and index arrays, but leaves the vertex and index buffers intact*/
		virtual void Clear();

		/*returns a pointer to the model used by the mesh*/
		Kiwi::IModel* GetModel()const { return m_meshModel; }

		std::wstring GetRenderGroup()const { return m_renderGroup; }

		unsigned short GetRenderGroupID()const { return m_renderGroupID; }

		unsigned short GetViewport()const { return m_viewportIndex; }

		/*tests for intersection between a cast ray and the faces in this mesh. returns true if a collision
		occured and false otherwise.
		|-rayOrigin: global coordinates if the start point of the cast ray
		|-rayDirection: orientation of the ray vector
		|-maxDepth: maximum z depth that will be checked for collisions along the path of the ray
		|-collisions: vector containing all faces in the mesh that were intersected by the ray
		|-maxCollisions: maximum number of collisions before this function will return
		|-includeBackFaces: if true, back faces will be included in collision results*/
		virtual bool IntersectRay( const Kiwi::Vector3d& rayOrigin, const Kiwi::Vector3d& rayDirection, double maxDepth, std::vector<Kiwi::Mesh::Face>& collisionFaces, int maxCollisions = -1, bool includeBackFaces = false );

		void SetModel( const std::wstring& modelName, bool copyOriginal = true );

		void SetModel( Kiwi::IModel* model, bool copyOriginal = true );

		bool SetRenderGroup( const std::wstring& renderGroup );

		/*attempts to set the mesh's render target and add the mesh to its render queue.
		|-renderTarget: unique name of the render target
		|-returns: true if the render target was found, false otherwise*/
		bool SetRenderTarget( const std::wstring& renderTarget );

		/*sets the viewport within the mesh's render target to which the mesh will be rendered*/
		void SetViewport( unsigned short viewportIndex ) { m_viewportIndex = viewportIndex; }
	};

	typedef std::vector<Kiwi::Mesh*> MeshList;

};

#endif