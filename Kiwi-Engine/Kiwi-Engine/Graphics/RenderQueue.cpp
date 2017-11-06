#include "RenderQueue.h"
#include "RenderQueueGroup.h"
#include "Camera.h"
#include "RenderTarget.h"

#include "../Core/Entity.h"
#include "..\Core\Scene.h"
#include "../Core/EngineRoot.h"
#include "..\Core\Console.h"
#include "../Core/EntityManager.h"
#include "..\Core\Utilities.h"

namespace Kiwi
{

	RenderQueue::RenderQueue()
	{
		m_lastQueueSize = 0;
	}

	RenderQueue::RenderQueue( Kiwi::RenderTarget& renderTarget )
	{
		m_renderTarget = &renderTarget;
		m_lastQueueSize = 0;

		/*initialize the group pool to store up to 511 unique ids (max number of render groups - default '0' group)*/
		std::vector<unsigned short> idPool( 511 );
		std::generate_n( idPool.begin(), 511, [](){
			static int i = 1;
			return i++;
		} );
		m_renderGroupUIDPool.Initialize( idPool );
	}

	RenderQueue::~RenderQueue()
	{
	}

	//clears the render queue and all render queue groups
	void RenderQueue::Clear()
	{
		Kiwi::FreeMemory( m_renderGroups );
		Kiwi::FreeMemory( m_meshList );
		Kiwi::FreeMemory( m_renderQueue );
		m_renderGroupUIDPool.Reset();
	}

	void RenderQueue::Reset()
	{
		Kiwi::FreeMemory( m_renderQueue );
	}

	void RenderQueue::AddMesh( Kiwi::Mesh& mesh )
	{
		if( mesh.GetModel() == nullptr ) return;

		std::lock_guard<std::mutex> lock( m_renderQueue_M );

		/*only add the mesh if another mesh with the same model hasn't already been added*/
		auto insResult = m_existingModelList.insert( mesh.GetModel() );
		if( insResult.second )
		{
			m_meshList.insert( &mesh );
		}
	}

	void RenderQueue::Build()
	{
		std::lock_guard<std::mutex> lock( m_renderQueue_M );

		m_renderQueue.reserve( m_lastQueueSize );
		m_lastQueueSize = 0;

		for( Kiwi::Mesh* mesh : m_meshList )
		{
			if( mesh->GetViewport() >= m_renderTarget->GetViewportCount() )continue;

			Kiwi::Entity* entity = mesh->GetEntity();
			if( entity == nullptr ) continue;

			Kiwi::IModel* model = mesh->GetModel();
			if( model == nullptr ) continue;

			Kiwi::Camera* camera = m_renderTarget->GetViewport( mesh->GetViewport() )->GetCamera();
			if( camera == nullptr ) continue;

			double depthFromCamera = camera->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform )->GetPosition().z;
			depthFromCamera -= entity->FindComponent<Kiwi::Transform>( Kiwi::ComponentType::Transform )->GetPosition().z;

			for( unsigned int i = 0; i < model->GetSubsetCount(); i++ )
			{
				if( model->GetSubset( i )->GetMaterial() == nullptr ) continue;
				m_renderQueue.push_back( Kiwi::Renderable( *model, i, (char)model->GetSubset(i)->GetMaterial()->HasTransparency(), mesh->GetRenderGroupID(), mesh->GetViewport(), static_cast<long>(depthFromCamera * 1000.0) ) );

				m_lastQueueSize++;
			}
		}
		
		/*sort the renderables by render key*/
		std::sort( m_renderQueue.begin(), m_renderQueue.end(), []( const Kiwi::Renderable& r1, const Kiwi::Renderable& r2 ) {
			return (r1.renderKey < r2.renderKey) || (r1.renderKey == r2.renderKey && r1.model < r2.model);
		} );

		/*erase duplicate renderables*/
		auto last = std::unique( m_renderQueue.begin(), m_renderQueue.end(), []( const Kiwi::Renderable& r1, const Kiwi::Renderable& r2 ){
			return (r1.model == r2.model) && (r1.subsetIndex == r2.subsetIndex);
		} );
		m_renderQueue.erase( last, m_renderQueue.end() );
	}

	void RenderQueue::CreateRenderGroup( const std::wstring& groupName, unsigned short viewportIndex )
	{
		RenderGroup* rg = this->FindRenderGroup( groupName );
		if( rg != nullptr )
		{
			rg->viewportIndex = viewportIndex;
			return;
		}

		unsigned short rgID = 0;
		if( m_renderGroupUIDPool.Get( rgID ) )
		{
			m_renderGroups.insert( std::make_pair( groupName, RenderGroup( groupName, rgID, viewportIndex ) ) );
		}
	}

	void RenderQueue::DestroyRenderGroup( const std::wstring& groupName )
	{
		auto itr = m_renderGroups.find( groupName );
		if( itr != m_renderGroups.end() )
		{
			m_renderGroupUIDPool.Release( itr->second.id );
			m_renderGroups.erase( itr );
		}
	}

	RenderGroup* RenderQueue::FindRenderGroup( const std::wstring& groupName )
	{
		auto itr = m_renderGroups.find( groupName );
		if( itr != m_renderGroups.end() )
		{
			return &itr->second;
		}

		return nullptr;
	}

	void RenderQueue::OnPostRender()
	{
		for( Kiwi::Renderable& renderable : m_renderQueue )
		{
			renderable.model->OnPreRender( *m_renderTarget, static_cast<unsigned short>((renderable.renderKey & Kiwi::RenderableBitmask::ViewportIndex) >> 51) );
		}
	}

	void RenderQueue::OnPreRender()
	{

	}

	void RenderQueue::RemoveMesh( Kiwi::Mesh& mesh )
	{
		std::lock_guard<std::mutex> lock( m_renderQueue_M );

		if( m_meshList.find( &mesh ) != m_meshList.end() )
		{
			m_existingModelList.erase( mesh.GetModel() );
			m_meshList.erase( &mesh );
		}
	}

}