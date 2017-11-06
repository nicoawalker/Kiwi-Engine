#ifndef _KIWI_STATICMODEL_H_
#define _KIWI_STATICMODEL_H_

#include "IModel.h"

namespace Kiwi
{

	class StaticModel :
		public Kiwi::IModel
	{
	protected:

		/*stores changes to the model to be commited*/
		struct UncommitedChanges
		{
			bool subsetsChanged;
			bool verticesChanged;
			bool indicesChanged;
			bool clearModel;
			std::vector<Kiwi::IModel::Subset> subsets;
			std::vector<Kiwi::Vertex> vertices;
			std::vector<unsigned long> indices;

			UncommitedChanges()
			{
				subsetsChanged = false;
				verticesChanged = false;
				indicesChanged = false;
				clearModel = false;
			}

			void Reset()
			{
				subsetsChanged = false;
				verticesChanged = false;
				indicesChanged = false;
				clearModel = false;
				Kiwi::FreeMemory( subsets );
				Kiwi::FreeMemory( vertices );
				Kiwi::FreeMemory( indices );
			}
		};

		UncommitedChanges m_uncommitedChanges;

		std::vector<Kiwi::IModel::Subset> m_subsets;

		/*indicates that all of the subsets should be re-checked
		for transparency the next time transparency is requested*/
		bool m_recheckTransparency;

		bool m_updateGPUBuffers;

	protected:

		/*updates the model's vertex and index buffers, freeing the old
		ones and creating new ones if needed*/
		void _UpdateGPUBuffers();

	public:

		/*initializes the model to empty*/
		StaticModel( const std::wstring& modelName );

		/*initializes the model with existing model data*/
		StaticModel( const std::wstring& modelName, const std::vector<Kiwi::IModel::Subset>& subsets,
					 const std::vector<Kiwi::Vertex>& vertices, const std::vector<unsigned long>& indices );

		/*initializes the model as a pre-built primitive type
		|-modelName: name of the model. must be unique
		|-primitiveType: type of model that should be created. valid values are quad and cube*/
		StaticModel( const std::wstring& modelName, MODEL_PRIMITIVE_TYPE primitiveType );

		StaticModel( const Kiwi::StaticModel& source );

		~StaticModel() {}

		void AddSubset();

		void AddSubset( const Kiwi::IModel::Subset& subset );

		/*binds the correct vertex and index buffers to render a given subset
		|-renderer: renderer that will bind the buffers
		|-subsetIndex: index of the subset that will be rendered*/
		virtual bool Bind( Kiwi::Renderer& renderer, unsigned int subsetIndex );

		/*applies the latest changes made to the model's vertices, indices, and subsets
		and then updates the model's gpu buffers with the new data
		|-throws a KiwiException if an error occurs with the gpu buffers*/
		void CommitChanges();

		/*empties the model*/
		void Clear ();

		/*returns a copy of the model*/
		virtual std::unique_ptr<Kiwi::IAsset> Clone();

		void SetVertices( const std::vector<Kiwi::Vertex>& vertices );

		void SetIndices( const std::vector<unsigned long>& indices );

		void SetSubsets( const std::vector<Kiwi::IModel::Subset>& subsets );

		bool UsingTransparency();

		const std::vector<Kiwi::Vertex>& GetVertices ()const { return m_vertices; }
		const std::vector<unsigned long>& GetIndices ()const { return m_indices; }
		const std::vector<Kiwi::IModel::Subset>& GetSubsetList()const { return m_subsets; }

		Kiwi::IModel::Subset* GetSubset( unsigned int subsetIndex );
		unsigned int GetSubsetCount()const;

	};
}

#endif