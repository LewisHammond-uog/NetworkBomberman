#include "stdafx.h"
#include "GameManager.h"

//C++ Includes
#include <map>

//Raknet Includes
#include <NetworkReplicator.h>

//Project Includes
#include "Entity.h"

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

void GameManager::CreatePlayers(int a_iPlayerCount, NetworkReplicator* a_pNetworkReplicator)
{
	//Loop through the required number of players
	for(int i = 0; i < a_iPlayerCount; ++i)
	{
		Entity* e;
		a_pNetworkReplicator->Reference(e = new Entity());
	}
}
