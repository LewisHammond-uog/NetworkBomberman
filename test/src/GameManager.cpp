#include "stdafx.h"
#include "GameManager.h"

//C++ Includes
#include <map>

//Raknet Includes
#include "NetworkReplicator.h"
#include "Component.h"
#include "NetworkIDManager.h"
#include "TransformComponent.h"

//Project Includes
#include "Entity.h"


GameManager::~GameManager()
{
}

void GameManager::Update(float a_fDeltaTime)
{
	
	//Get a list of and then update all of the entities
	std::map<const unsigned int, Entity*>::const_iterator xIter;
	for (xIter = Entity::GetEntityMap().begin(); xIter != Entity::GetEntityMap().end(); ++xIter)
	{
		Entity* pEntity = xIter->second;
		if (pEntity) {
			pEntity->Update(a_fDeltaTime);
		}
	}
	
}

/// <summary>
/// Creates a given number of players
/// </summary>
/// <param name="a_iPlayerCount">Given Number of Players</param>
/// <param name="a_pNetworkReplicator"></param>
void GameManager::CreatePlayers(int a_iPlayerCount, NetworkReplicator* a_pNetworkReplicator)
{
	//Loop through the required number of players
	for(int i = 0; i < a_iPlayerCount; ++i)
	{
		Entity* e = new Entity();
		Component* c = new TransformComponent(e);
		e->AddComponent(c);
		a_pNetworkReplicator->Reference(c);
		a_pNetworkReplicator->Reference(e);
		
	}
}