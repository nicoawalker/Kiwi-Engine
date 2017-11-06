#include "RenderQueueGroup.h"
#include "Mesh.h"
#include "Viewport.h"
#include "Camera.h"
#include "Texture.h"
#include "Material.h"
#include "IShader.h"

#include "..\Core\Scene.h"
#include "../Core/Entity.h"
#include "..\Core\EngineRoot.h"

#include <algorithm>

namespace Kiwi
{

	RenderQueueGroup::RenderQueueGroup( std::wstring groupName, unsigned long groupID )
	{	
		m_groupName = groupName;
		m_groupID = groupID;
		m_hasChanged = false;
		//m_meshQueue.resize( 3 );
	}


	RenderQueueGroup::~RenderQueueGroup()
	{
	}

	void RenderQueueGroup::AddMesh( Kiwi::Mesh* mesh )
	{
		//if( mesh == nullptr ) return;

		///*get the mesh's entity to find out what kind of object this mesh is attached to*/
		//Kiwi::Entity* entity = mesh->GetEntity();
		//Kiwi::IModel* model = mesh->GetModel();

		//if( entity == nullptr || model == nullptr ) return;

		//if( model->IsShared() ) m_sharedModels.insert( model );

		//if( entity->GetType() == Kiwi::Entity::ENTITY_3D )
		//{
		//	for( unsigned int i = 0; i < model->GetSubsetCount(); i++ )
		//	{
		//		Kiwi::IModel::Subset* subset = model->GetSubset( i );
		//		if( subset == nullptr || subset->GetMaterial() == nullptr ) continue;

		//		long shaderID = subset->GetMaterial()->GetShaderID();
		//		if( shaderID == 0 )
		//		{
		//			/*shaderID hasn't been set yet, set it now*/
		//			Kiwi::IShader* shader = _kAssetManager.FindAsset<Kiwi::IShader>( L"Shader", subset->GetMaterial()->GetShader() );
		//			if( shader != nullptr )
		//			{
		//				subset->GetMaterial()->SetShaderID( shader->GetUID() );
		//			}
		//		}

		//		if( subset->GetMaterial()->HasTransparency() )
		//		{/*adding a subset with a material that has transparency*/
		//			//m_meshQueue[1].push_back( submesh );
		//			ModelSubsetPair mPair( model, subset, i );
		//			auto insStatus = m_queuedModels.insert( mPair );
		//			if( insStatus.second )
		//			{
		//				m_transparentModelQueue.push_back(mPair);
		//			}

		//		} else
		//		{/*adding a solid submesh*/
		//			//m_meshQueue[0].push_back( submesh );
		//			ModelSubsetPair mPair( model, subset, i );
		//			auto insStatus = m_queuedModels.insert( mPair );
		//			if( insStatus.second )
		//			{
		//				m_solidModelQueue.push_back( mPair );
		//			}
		//		}

		//		m_hasChanged = true;
		//	}

		//} else if( entity->GetType() == Kiwi::Entity::ENTITY_2D )
		//{
		//	/*adding 2D model*/
		//	for( unsigned int i = 0; i < model->GetSubsetCount(); i++ )
		//	{
		//		Kiwi::IModel::Subset* subset = model->GetSubset( i );
		//		if( subset == nullptr ) continue;

		//		//m_meshQueue[2].push_back( submesh );
		//		ModelSubsetPair mPair( model, subset, i );
		//		auto insStatus = m_queuedModels.insert( mPair );
		//		if( insStatus.second )
		//		{
		//			m_2dModelQueue.push_back( mPair );
		//			m_hasChanged = true;
		//		}
		//	}
		//}
	}

	void RenderQueueGroup::Sort( Kiwi::Viewport& viewport )
	{
		///*if the group hasnt changed, no need to sort again*/
		//if( m_hasChanged == false ) return;

		///*sort the solid models
		//sort order is shader > material texture > model*/
		//std::sort( m_solidModelQueue.begin(), m_solidModelQueue.end(), []( const ModelSubsetPair& p1, const ModelSubsetPair& p2 ){
		//	if( p1.subset->GetMaterial() == nullptr ) return false;

		//	if( p1.subset->GetMaterial()->GetShaderID() != p2.subset->GetMaterial()->GetShaderID() ) return false;

		//	if( p1.subset->GetMaterial()->GetTexture( Material::TEXTURE_TYPE::DIFFUSE ) != p2.subset->GetMaterial()->GetTexture( Material::TEXTURE_TYPE::DIFFUSE ) ||
		//		p1.subset->GetMaterial()->GetTexture( Material::TEXTURE_TYPE::AMBIENT ) != p2.subset->GetMaterial()->GetTexture( Material::TEXTURE_TYPE::AMBIENT ) ||
		//		p1.subset->GetMaterial()->GetTexture( Material::TEXTURE_TYPE::BUMP ) != p2.subset->GetMaterial()->GetTexture( Material::TEXTURE_TYPE::BUMP ) ||
		//		p1.subset->GetMaterial()->GetTexture( Material::TEXTURE_TYPE::SPECULAR ) != p2.subset->GetMaterial()->GetTexture( Material::TEXTURE_TYPE::SPECULAR ) ) return false;

		//	return p1.model == p2.model;
		//});

		///*sort the transparent models
		//sort order is simply by depth from camera*/
		//std::sort( m_transparentModelQueue.begin(), m_transparentModelQueue.end(), [this, viewport]( const ModelSubsetPair& p1, const ModelSubsetPair& p2 ){
		//	Kiwi::Camera* cam = viewport.GetCamera();
		//	Kiwi::Transform* camT = cam->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform );

		//	/*if the model is shared, use the center point of all the instances for the depth test, otherwise use the position of the entity the model is linked to*/
		//	Kiwi::Vector3d p1Pos = (p1.model->IsShared()) ? p1.model->GetInstanceCenter() : (*(p1.model->GetLinkedMeshes().begin()))->GetEntity()->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform )->GetPosition();
		//	Kiwi::Vector3d p2Pos = (p2.model->IsShared()) ? p2.model->GetInstanceCenter() : (*(p2.model->GetLinkedMeshes().begin()))->GetEntity()->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform )->GetPosition();

		//	return Kiwi::SquareDistance( p2Pos, camT->GetPosition() ) < Kiwi::SquareDistance( p1Pos, camT->GetPosition() );
		//} );

		///*sort the 2D models
		//sort order is z depth lowest to highest*/
		//std::sort( m_2dModelQueue.begin(), m_2dModelQueue.end(), [this, viewport]( const ModelSubsetPair& p1, const ModelSubsetPair& p2 ){
		//	/*if the model is shared, use the center point of all the instances for the depth test, otherwise use the position of the entity the model is linked to*/
		//	double p1Z = (p1.model->IsShared()) ? p1.model->GetInstanceCenter().z : (*(p1.model->GetLinkedMeshes().begin()))->GetEntity()->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform )->GetPosition().z;
		//	double p2Z = (p2.model->IsShared()) ? p2.model->GetInstanceCenter().z : (*(p2.model->GetLinkedMeshes().begin()))->GetEntity()->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform )->GetPosition().z;

		//	return p1Z < p2Z;
		//} );

		//for( Kiwi::IModel* model : m_sharedModels )
		//{
		//	model->UpdateSharedData();

		//	if( model->UsingTransparency() )
		//	{
		//		/*if a model uses transparency, the instance data has to be depth sorted*/
		//		model->SortSharedData( viewport );
		//	}

		//	model->CommitSharedData();
		//}

		//m_hasChanged = false;
	}

	//empties the group
	void RenderQueueGroup::Clear()
	{
		/*Kiwi::FreeMemory( m_solidModelQueue );
		Kiwi::FreeMemory( m_transparentModelQueue );
		Kiwi::FreeMemory( m_2dModelQueue );
		Kiwi::FreeMemory( m_queuedModels );
		Kiwi::FreeMemory( m_sharedModels );

		m_hasChanged = false;*/
	}

}