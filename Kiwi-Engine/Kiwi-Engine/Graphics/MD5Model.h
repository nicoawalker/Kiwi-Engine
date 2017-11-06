#ifndef _KIWI_MD5MODEL_H_
#define _KIWI_MD5MODEL_H_

#include "IModel.h"

#include "..\Core\Vector2d.h"
#include "../Core/Vector3d.h"

#include <vector>
#include <string>

namespace Kiwi
{

	class MD5Model : 
		public Kiwi::IModel
	{
	public:

		struct Joint
		{
			std::wstring name;
			int parentID;

			Kiwi::Vector3d position;
			Kiwi::Quaternion orientation;
		};

		struct JointWeight
		{
			int jointID;
			float bias;
			Kiwi::Vector3d position;
		};

		class MD5Subset : 
			public IModel::Subset
		{
		public:
			unsigned int weightOffset; //start position of the subset's weights in the model's JointWeight array
			unsigned int numWeights;
			unsigned int numTriangles;

			MD5Subset();

			MD5Subset( const std::wstring& material );

			MD5Subset( const std::wstring& material, unsigned int vOffset, unsigned int vCount, unsigned int iOffset, unsigned int iCount,
					   unsigned int vBufferIndex, unsigned int iBufferIndex, unsigned int wOffset, unsigned int wCount, unsigned int tCount );

			MD5Subset( const Kiwi::MD5Model::MD5Subset& subset );

			Kiwi::MD5Model::MD5Subset& operator=( const Kiwi::MD5Model::MD5Subset& subset );

		};

	protected:

		/*stores changes to the model to be commited*/
		struct UncommitedChanges
		{
			std::vector<Kiwi::MD5Model::MD5Subset> md5Subsets;
			std::vector<Kiwi::Vertex> vertices;
			std::vector<unsigned long> indices;
			std::vector<Kiwi::MD5Model::JointWeight> jointWeights;
			std::vector<Kiwi::MD5Model::Joint> joints;
		};

		std::unique_ptr<UncommitedChanges> m_uncommitedChanges;

		std::vector<Kiwi::MD5Model::MD5Subset> m_md5Subsets;
		std::vector<Kiwi::MD5Model::JointWeight> m_jointWeights;
		std::vector<Kiwi::MD5Model::Joint> m_joints;

	protected:

		virtual bool _LinkMesh( Kiwi::Mesh& mesh );

		virtual bool _UpdateModel();

	public:

		MD5Model( const std::wstring& name ) :
			Kiwi::IModel( name, L"MD5Model" )
		{ }

		MD5Model ( const std::wstring& name, const std::vector<Kiwi::Vertex>& vertices, const std::vector<unsigned long>& indices, const std::vector<Kiwi::MD5Model::MD5Subset>& md5Subsets,
				   const std::vector<Kiwi::MD5Model::Joint>& joints, const std::vector<Kiwi::MD5Model::JointWeight>& weights );

		MD5Model ( const MD5Model& source );

		/*binds the correct vertex and index buffers to render a given subset
		|-renderer: renderer that will bind the buffers
		|-subsetIndex: index of the subset that will be rendered*/
		virtual bool Bind( Kiwi::Renderer& renderer, unsigned int subsetIndex );

		/*applies the latest changes made to the model's vertices, indices, subsets, etc.
		and then updates the model's gpu buffers with the new data
		|-throws a KiwiException if an error occurs with the gpu buffers*/
		void CommitChanges();

		void Clear();

		virtual std::unique_ptr<Kiwi::IAsset> Clone();

		void SetVertices( const std::vector<Kiwi::Vertex>& vertices );
		void SetIndices( const std::vector<unsigned long>& indices );
		void SetJoints( const std::vector<Kiwi::MD5Model::Joint>& joints );
		void SetJointWeights( const std::vector<Kiwi::MD5Model::JointWeight>& jointWeights );
		void SetSubsets( const std::vector<Kiwi::MD5Model::MD5Subset>& md5Subsets );

		const std::vector<Kiwi::MD5Model::Joint>& GetJoints()const { return m_joints; }
		const std::vector<Kiwi::MD5Model::JointWeight>& GetJointWeights()const { return m_jointWeights; }
		const std::vector<Kiwi::MD5Model::MD5Subset>& GetSubsetList()const { return m_md5Subsets; }

		Kiwi::IModel::Subset* GetSubset( unsigned int subsetIndex );
		unsigned int GetSubsetCount()const;

	};
}

#endif