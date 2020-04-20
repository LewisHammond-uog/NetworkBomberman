#include "stdafx.h"
#include "GameManager.h"

//C++ Includes
#include <map>

//Project Includes
#include "ServerClientBase.h"
#include "NetworkReplicator.h"
#include "Entity.h"
//Components
#include "BombSpawnerComponent.h"
#include "PlayerControlComponent.h"
#include "TransformComponent.h"
#include "SpherePrimitiveComponent.h"

//Init Statics
GameManager* GameManager::s_pInstance = nullptr;

GameManager::~GameManager()
{
}

/// <summary>
/// Get the instance of the game manager
/// </summary>
/// <returns></returns>
GameManager* GameManager::GetInstance()
{
	if(s_pInstance == nullptr)
	{
		s_pInstance = new GameManager();
	}

	return s_pInstance;
}

void GameManager::Update(const float a_fDeltaTime)
{
	
	//Get a list of and then update all of the entities
	std::map<const unsigned int, Entity*>::const_iterator xUpdateIter;
	for (xUpdateIter = Entity::GetEntityMap().begin(); xUpdateIter != Entity::GetEntityMap().end(); ++xUpdateIter)
	{
		Entity* pEntity = xUpdateIter->second;
		if (pEntity) {
			pEntity->Update(a_fDeltaTime);
			pEntity->Draw(nullptr);
		}
	}

	//Delete all entities that should be deleted after update
	std::vector<Entity*>::const_iterator xDelIter;
	for(xDelIter = m_vDeleteEntityQueue.begin(); xDelIter != m_vDeleteEntityQueue.end();)
	{
		Entity* pEntity = *xDelIter;
		if(pEntity)
		{
			//Remove from vector and delete
			xDelIter = m_vDeleteEntityQueue.erase(xDelIter);
			delete pEntity;
		}
	}
}

/// <summary>
/// Delete an entity after all other entities in the scene are updated
/// </summary>
/// <param name="a_pEntity"></param>
void GameManager::DeleteEntityAfterUpdate(Entity* a_pEntity)
{
	//Add Entity to our delete queue
	m_vDeleteEntityQueue.push_back(a_pEntity);
}


/// <summary>
/// Creates a given number of players
/// </summary>
/// <param name="a_iPlayerCount">Given Number of Players</param>
void GameManager::CreatePlayers(const int a_iPlayerCount)
{
	//Loop through the required number of players
	for(int i = 0; i < a_iPlayerCount; ++i)
	{
		//Create Player Entities with the required components
		Entity* pPlayerEntity = new Entity(); //This is added to a static entity list when created so we don't need to worry about storing it here
		TransformComponent* pPlayerTransform = new TransformComponent(pPlayerEntity); //This is the same for components
		SpherePrimitiveComponent* pSphere = new SpherePrimitiveComponent(pPlayerEntity);
		PlayerControlComponent* pPlayerControl = new PlayerControlComponent(pPlayerEntity);
		BombSpawnerComponent* pBombSpawner = new BombSpawnerComponent(pPlayerEntity);
			
		
		//Add these components to the player entity
		pPlayerEntity->AddComponent(pPlayerTransform);
		pPlayerEntity->AddComponent(pSphere);
		pPlayerEntity->AddComponent(pPlayerControl);
		pPlayerEntity->AddComponent(pBombSpawner);

		//Send the entity and components to the replica manager, it is important
		//that we send the player entity first as components rely on having
		//an owner entity
		NetworkReplicator* pNetworkReplicator = ServerClientBase::GetNetworkReplicator();
		if(pNetworkReplicator == nullptr)
		{
			return;
		}
		pNetworkReplicator->Reference(pPlayerEntity);
		pNetworkReplicator->Reference(pPlayerTransform);
		pNetworkReplicator->Reference(pSphere);
		pNetworkReplicator->Reference(pPlayerControl);
		pNetworkReplicator->Reference(pBombSpawner);
	}
}