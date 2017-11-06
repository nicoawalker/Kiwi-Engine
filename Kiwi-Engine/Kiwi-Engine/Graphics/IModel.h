#ifndef _KIWI_IMODEL_H_
#define _KIWI_IMODEL_H_

#include "Color.h"
#include "Mesh.h"

#include "..\Core\Vector2d.h"
#include "../Core/Vector3d.h"
#include "..\Core\IReferencedObject.h"
#include "../Core/IAssetConsumer.h"

#include <vector>
#include <string>
#include <unordered_set>

namespace Kiwi
{

	class RenderTarget;
	class Viewport;

	enum MODEL_PRIMITIVE_TYPE { QUAD = 0, CUBE = 1 };

	struct Vertex
	{
		Kiwi::Vector3d position;
		Kiwi::Vector2d tex;
		Kiwi::Vector3d normal;
		/*per-vertex color*/
		Kiwi::Color color;
		/*used for bump mapping*/
		Kiwi::Vector3d tangent;
		Kiwi::Vector3d bitangent;

		//used for md5 models
		unsigned int startWeight; //index of first weight the vertex is weighted to
		unsigned int weightCount; //number of weights for this vertex

		Vertex()
		{
			color = Kiwi::Color( 1.0f, 1.0f, 1.0f, 1.0f );
			startWeight = 0;
			weightCount = 0;
		}

		Vertex( const Kiwi::Vector3d& pos, const Kiwi::Vector2d& textureUVs, const Kiwi::Vector3d& normals, const Kiwi::Color& color = Kiwi::Color( 1.0f, 1.0f, 1.0f, 1.0f ) )
		{
			position = pos;
			tex = textureUVs;
			normal = normals;
			this->color = color;
			startWeight = 0;
			weightCount = 0;
		}
	};

	class IModel :
		public Kiwi::IAsset
	{
		friend class Kiwi::Mesh;
	public:

		class Subset:
			public Kiwi::IAssetConsumer
		{
		protected:

			Kiwi::Material* m_material;

			std::wstring m_materialName;

		public:

			Kiwi::IModel* parent;

			unsigned int vertexOffset; //start position of the submesh's vertices in the mesh's vertex array
			unsigned int numVertices;
			unsigned int indexOffset; //start position of the submesh's indices in the mesh's indices array
			unsigned int numIndices;

			unsigned int vBufIndex;
			unsigned int iBufIndex;

			/*indicates whether this subset's gpu buffers need to be updated*/
			bool updateGPUBuffer;

		protected:

			virtual Kiwi::EventResponse _OnAssetFetched( Kiwi::IAsset* asset );

		public:

			Subset();

			Subset( const std::wstring& material );

			Subset( Kiwi::Material& material );

			Subset( const std::wstring& material, unsigned int vOffset, unsigned int vCount, unsigned int iOffset, unsigned int iCount,
					unsigned int vBufferIndex = 0, unsigned int iBufferIndex = 0 );

			Subset( const Kiwi::IModel::Subset& subset );

			~Subset();

			void SetMaterial( const std::wstring& material );

			void SetMaterial( Kiwi::Material& material );

			Kiwi::Material* GetMaterial()const { return m_material; }

			std::wstring GetMaterialName()const { return m_materialName; }

			Kiwi::IModel::Subset& operator=( const Kiwi::IModel::Subset& subset );

		};

		struct ModelInstanceData
		{
			/*world matrix for each model position*/
			DirectX::XMFLOAT4X4 world;
		};

	protected:

		/*stores all of the vertices and indices for the model*/
		std::vector<Kiwi::Vertex> m_vertices;
		std::vector<unsigned long> m_indices;

		/*stores the vertex and index buffers used by the model's subsets. each subset will have one 
		vertex and index buffer stored in the list*/
		std::vector<Kiwi::GPUBuffer<Kiwi::D3DVertex>*> m_vertexGPUBuffers;
		std::vector<Kiwi::GPUBuffer<unsigned long>*> m_indexGPUBuffers;

		/*stores the per-instance data for the model, if the model is shared*/
		std::vector<ModelInstanceData> m_instanceData;

		Kiwi::GPUBuffer<ModelInstanceData>* m_instanceGPUBuffer;

		/*center point of all the instances*/
		Kiwi::Vector3d m_instanceCenter;

		/*stores the meshes that are currently using this model. if the model is shared then all meshes sharing
		it will be stored here. if not, it will only contain the single mesh that is using the model*/
		std::unordered_set<Kiwi::Mesh*> m_linkedMeshes;

		Kiwi::PrimitiveTopology m_primitiveTopology;
		
		bool m_isShared;
		bool m_hasTransparency;
		bool m_calculateTangents;
		bool m_calculateNormals;

	protected:

		/*calculates the tangents and bitangents for the model's vertices*/
		void _CalculateTangents( Kiwi::Vector3d& _tangent, Kiwi::Vector3d& _bitangent, const Kiwi::Vertex& vertex1, 
										 const Kiwi::Vertex& vertex2, const Kiwi::Vertex& vertex3 );

		/*calculates a normal from a tangent and bitangent*/
		void _CalculateNormal( Kiwi::Vector3d& _normal, const Kiwi::Vector3d& tangent, const Kiwi::Vector3d& bitangent );

		/*calculates the tangents, bitanges, and normals for the model, based on the settings*/
		void _CalculateVectors();

		/*copies the current instance data to the instance buffer, resizing/recreating the
		instance buffer if needed*/
		//virtual void _CopySharedData();

		Kiwi::GPUBuffer<Kiwi::D3DVertex>* _CreateVertexGPUBuffer( unsigned int startIndex, unsigned int endIndex );
		Kiwi::GPUBuffer<unsigned long>* _CreateIndexGPUBuffer( unsigned int startIndex, unsigned int endIndex );

		/*frees and removes a vertex buffer from the model*/
		virtual void _FreeVertexBuffer( unsigned int bufferIndex );

		/*frees and removes a vertex buffer from the model*/
		virtual void _FreeIndexBuffer( unsigned int bufferIndex );

		void _FreeInstanceBuffer();

		void _InitializeInstanceBuffer();

		/*private functions to link and unlink this model to a mesh. linking is done
		via the mesh and the mesh then calls these
		if the model isn't shared, linking will fail if there is already a linked mesh*/
		virtual bool _LinkMesh( Kiwi::Mesh& mesh );

		/*performs a depth-sort of the current instance data in relation to
		the camera attached to the given transform*/
		virtual void _SortSharedData( Kiwi::Viewport& viewport );

		virtual void _UnlinkMesh( Kiwi::Mesh& mesh );

		virtual void _UpdateGPUBuffers() { }

	public:

		IModel( const std::wstring& modelName, const std::wstring& modelType );

		IModel ( const Kiwi::IModel& cpySource );

		virtual ~IModel () = 0;

		/*appends an empty subset to the end of the model's subset list.
		Must call CommitChanges() to apply the change*/
		virtual void AddSubset() {}

		/*appends a new subset to the end of the model's subset list.
		Must call CommitChanges() to apply the change*/
		void AddSubset( const Kiwi::IModel::Subset& subset ) {}

		/*binds the correct vertex and index buffers to render a given subset
		|-renderer: renderer that will bind the buffers
		|-subsetIndex: index of the subset that will be rendered*/
		virtual bool Bind( Kiwi::Renderer& renderer, unsigned int subsetIndex ) = 0;

		virtual void RecalculateVectors() { this->_CalculateVectors(); }

		/*called after UpdateSharedData/SortSharedData to write the updated data to the GPU*/
		//virtual void CommitSharedData() { this->_CopySharedData(); }

		/*empties the model*/
		virtual void Clear() {}

		/*called immediately before rendering begins. used to update/create necessary
		GPU buffers prior to rendering, and to update instance data and buffer*/
		virtual void OnPreRender( Kiwi::RenderTarget& renderTarget, unsigned short viewportIndex );

		/*retrieves the current instance data from all linked meshes*/
		virtual void UpdateSharedData();

		virtual bool UsingTransparency() { return false; }

		virtual const std::vector<Kiwi::Vertex>& GetVertices()const { return m_vertices; }
		virtual const std::vector<unsigned long>& GetIndices()const { return m_indices; }

		/*returns the averaged center-point of all the instances*/
		Kiwi::Vector3d& GetInstanceCenter() { return m_instanceCenter; }

		std::vector<ModelInstanceData>& GetInstanceData() { return m_instanceData; }

		virtual const std::unordered_set<Kiwi::Mesh*>& GetLinkedMeshes()const { return m_linkedMeshes; }
		Kiwi::PrimitiveTopology GetPrimitiveTopology()const { return m_primitiveTopology; }

		virtual Kiwi::IModel::Subset* GetSubset( unsigned int subsetIndex ) = 0;
		virtual unsigned int GetSubsetCount()const = 0;

		virtual void SetShared( bool isShared ) { m_isShared = isShared; }

		bool IsShared()const { return m_isShared; }

		void SetPrimitiveTopology( Kiwi::PrimitiveTopology topology ) { m_primitiveTopology = topology; }

	};
}

#endif