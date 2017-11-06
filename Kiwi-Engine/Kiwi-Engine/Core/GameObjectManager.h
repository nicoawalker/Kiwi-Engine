#ifndef _KIWI_GAMEOBJECTMANAGER_H_
#define _KIWI_GAMEOBJECTMANAGER_H_

#include "GameObject.h"

#include <unordered_map>

namespace Kiwi
{

	class GameObjectManager
	{
	protected:

		/*stores the active entities (these are updated every frame)*/
		std::unordered_multimap<std::wstring, std::unique_ptr<Kiwi::GameObject>> m_gameObjects;

		/*stores pointers to gameobjects that haven't yet been intialized*/
		std::unordered_set<Kiwi::GameObject*> m_unitializedObjects;

		/*buffer used to hold newly added game objects that are added from Start and FirstUpdate
		calls made while iterating over the unitializedObjects list*/
		std::vector<Kiwi::GameObject*> m_newObjectBuffer;

		bool m_started;
		bool m_bufferNewObjects;

	protected:

		/*performs 'first-run' tasks for all new game objects that have been added, then removes them from the list of new objects*/
		void _InitializeNewGameObjects();

	public:

		GameObjectManager();
		~GameObjectManager();

		/*starts all game objects currently in the manager*/
		void Start();

		/*updates all active game objects that have not been shutdown*/
		void Update();

		/*updates all active game objects that have not been shutdown and checks for and destroys
		game objects that have been shutdown and who's reference count is 0*/
		void FixedUpdate();

		/*transfers ownership of the game object to the game object manager*/
		Kiwi::GameObject* ManageGameObject( std::unique_ptr<Kiwi::GameObject> gameObject );

		/*destroys all game objects that have been shut down and who's reference count is 0*/
		void CleanUp();

		/*shuts down all game objects*/
		void ShutdownAll();

		/*returns the first game object found that has a matching name*/
		Kiwi::GameObject* Find( std::wstring name );

		/*returns a vector containing pointers to all game objects that have a matching name*/
		Kiwi::GameObjectList FindAll( std::wstring name );

		/*returns a vector containing pointers to all game objects that have a matching tag*/
		Kiwi::GameObjectList FindAllWithTag( std::wstring tag );

	};

}

#endif