#ifndef _KIWI_GAMEOBJECT_H_
#define _KIWI_GAMEOBJECT_H_

#include "IReferencedObject.h"

#include <string>
#include <mutex>
#include <unordered_set>
#include <vector>

namespace Kiwi
{

	class Scene;

	class GameObject:
		public Kiwi::IReferencedObject
	{
	protected:
		//stores the current object ID that will be assigned to the next created object
		static int GlobalObjectID; 
		static std::mutex GlobalObjectIDMutex;

		Kiwi::Scene* m_scene;

		/*ID unique to each game object that can be used to identify it in place of the name*/
		int m_objectID;

		std::wstring m_objectName;

		std::wstring m_objectType;

		/*optional, non-unique identifiers*/
		std::unordered_set<std::wstring> m_tags;

		bool m_isShutdown;
		bool m_isActive;
		bool m_started;
		bool m_firstUpdate; //true until the gameobject has been updated for the first time

	protected:

		/*this is called when the game object is shutdown*/
		virtual void _OnShutdown() {}

		/*called once during every fixed update for all active game objects*/
		virtual void _OnFixedUpdate() {}

		/*called once during every update for all active game objects*/
		virtual void _OnUpdate() {}

		/*called when the game object is set to be active (receiving updates)*/
		virtual void _OnActivate() {}

		/*called when the game object is set to be inactive (not receiving updates)*/
		virtual void _OnDeactivate() {}

		/*called once immediately after the game object is added to a scene, or when the scene the object belongs to is started*/
		virtual void _OnStart() {}

		/*called once before the first update of the game object*/
		virtual void _OnFirstUpdate() {}
		
	public:

		GameObject( std::wstring name, std::wstring objectType, Kiwi::Scene& scene );

		virtual ~GameObject() = 0;

		virtual void Shutdown();

		/*called once during every fixed update for all active game objects*/
		virtual void FixedUpdate();

		/*called once during every update for all active game objects*/
		virtual void Update();

		/*called once immediately after the game object is created*/
		virtual void FirstUpdate() { this->_OnFirstUpdate(); }

		/*called once before the first update of the game object*/
		virtual void Start();

		/*adds a new tag string to the game object*/
		virtual void AddTag( std::wstring tag ) { m_tags.insert( tag ); }

		/*removes the matching string from the entity*/
		virtual void RemoveTag( std::wstring tag ) { m_tags.erase( tag ); }

		/*removes all tags from the entity*/
		virtual void RemoveAllTags();

		/*checks if the entity has a certain tag*/
		virtual bool HasTag( std::wstring tag );

		bool IsShutdown()const { return m_isShutdown; }
		bool IsActive()const { return m_isActive; }
		bool IsStarted()const { return m_started; }

		/*either enables or disables the game object
		disabled objects will not receive update calls until re-enabled*/
		void SetActive( bool isActive );

		void SetName( std::wstring name ) { m_objectName = name; }

		std::wstring GetName()const { return m_objectName; }

		/*returns the unique ID of this game object*/
		int GetID()const { return m_objectID; }

		std::wstring GetObjectType()const { return m_objectType; }

	};

	//vector containing game object pointers
	typedef std::vector<Kiwi::GameObject*> GameObjectList;

}

#endif