#include "GameObject.h"
#include "Utilities.h"

namespace Kiwi
{

	int GameObject::GlobalObjectID = 1;
	std::mutex GameObject::GlobalObjectIDMutex;

	GameObject::GameObject( std::wstring name, std::wstring objectType, Kiwi::Scene& scene ):
		Kiwi::IReferencedObject( name )
	{
		GlobalObjectIDMutex.lock();
		m_objectID = GlobalObjectID++;
		if( GlobalObjectID == 0 )
		{
			GlobalObjectID++;
		}
		GlobalObjectIDMutex.unlock();

		m_objectName = name;
		m_isShutdown = false;
		m_isActive = true;
		m_started = false;
		m_firstUpdate = false;
		m_objectType = objectType;
		m_scene = &scene;
	}

	GameObject::~GameObject()
	{
		if( !m_isShutdown )
		{
			this->Shutdown();
		}
	}

	void GameObject::Update()
	{
		if( m_firstUpdate )
		{
			m_firstUpdate = false;
			this->_OnFirstUpdate();
		}

		this->_OnUpdate();
	}

	void GameObject::FixedUpdate()
	{
		if( m_firstUpdate )
		{
			m_firstUpdate = false;
			this->_OnFirstUpdate();
		}

		this->_OnFixedUpdate();
	}

	void GameObject::Start()
	{
		if( m_started ) return;

		m_started = true;

		this->_OnStart();
	}

	void GameObject::Shutdown()
	{
		if( !m_isShutdown )
		{
			m_isShutdown = true;
			m_isActive = false;

			this->_OnShutdown();
		}
	}

	void GameObject::RemoveAllTags()
	{

		//empty the tag list and free the memory
		Kiwi::FreeMemory( m_tags );

	}

	/*checks if the entity has a certain tag*/
	bool GameObject::HasTag( std::wstring tag )
	{

		auto it = m_tags.find( tag );
		if( it != m_tags.end() )
		{
			return true;

		} else
		{
			return false;
		}

	}

	void GameObject::SetActive( bool isActive )
	{
		if( isActive != m_isActive )
		{
			m_isActive = isActive;
			if( m_isActive )
			{
				this->_OnActivate();

			} else
			{
				this->_OnDeactivate();
			}
		}
	}

}