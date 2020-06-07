#include "stdafx.h"
#include "PlayerManager.h"
//C++ Includes
#include <algorithm>

//Project Includes
#include "Entity.h"
#include "Component.h"
#include "PlayerControlComponent.h"
#include "PlayerDataComponent.h"
#include "SpherePrimitiveComponent.h"
#include "TransformComponent.h"
#include "BombSpawnerComponent.h"
#include "ColliderComponent.h"

//Define pairs for our map
typedef std::pair<RakNet::RakNetGUID, Entity*> GUIDPlayerPair;

/// <summary>
/// Creates players for all of the clients connected
/// </summary>
/// <param name="a_vConnectedClients">A list of clients that are connected to the server</param>
void PlayerManager::CreatePlayersForAllClients(const std::vector<ConnectedClientInfo>& a_vConnectedClients)
{
	//Loop through all of the connected clients and create a player for them
	const int iClientCount = a_vConnectedClients.size();
	for (int i = 0; i < iClientCount; ++i)
	{
		//Create player with GUID of the connected client
		CreatePlayer(a_vConnectedClients[i].m_clientGUID);
	}
}

/// <summary>
/// Creates a player entity
/// </summary>
/// <param name="a_ownerGUID">System that owns the player we are creating</param>
/// <returns>Created Player</returns>
Entity* PlayerManager::CreatePlayer(const RakNet::RakNetGUID a_ownerGUID)
{
	//Create Player Entities with the required components
	Entity* pPlayerEntity = new Entity(); //This is added to a static entity list when created so we don't need to worry about storing it here
	TransformComponent* pPlayerTransform = new TransformComponent(pPlayerEntity); //This is the same for components
	SpherePrimitiveComponent* pSphere = new SpherePrimitiveComponent(pPlayerEntity);
	ColliderComponent* pCollider = new ColliderComponent(pPlayerEntity);
	PlayerControlComponent* pPlayerControl = new PlayerControlComponent(pPlayerEntity);
	BombSpawnerComponent* pBombSpawner = new BombSpawnerComponent(pPlayerEntity);
	PlayerDataComponent* pPlayerData = new PlayerDataComponent(pPlayerEntity, a_ownerGUID);

	//Add these components to the player entity
	pPlayerEntity->AddComponent(pPlayerTransform);
	pPlayerEntity->AddComponent(pSphere);
	pPlayerEntity->AddComponent(pCollider);
	pPlayerEntity->AddComponent(pPlayerControl);
	pPlayerEntity->AddComponent(pBombSpawner);
	pPlayerEntity->AddComponent(pPlayerData);

	//Add Spehere Collider
	pCollider->AddSphereCollider(1.f, glm::vec3(0, 0, 0));

	//Send the entity and components to the replica manager, it is important
	//that we send the player entity first as components rely on having
	//an owner entity
	NetworkReplicator* pNetworkReplicator = ServerClientBase::GetNetworkReplicator();
	if (pNetworkReplicator == nullptr)
	{
		return nullptr;
	}
	pNetworkReplicator->Reference(pPlayerEntity);
	pNetworkReplicator->Reference(pPlayerTransform);
	pNetworkReplicator->Reference(pSphere);
	pNetworkReplicator->Reference(pPlayerControl);
	pNetworkReplicator->Reference(pBombSpawner);
	pNetworkReplicator->Reference(pPlayerData);

	//Add Player to list of players
	m_xPlayers.insert(GUIDPlayerPair(a_ownerGUID, pPlayerEntity));

	return pPlayerEntity;
}

/// <summary>
/// Sets all players to be either enabled or disabled
/// </summary>
/// <param name="a_bEnabled">If players should be enabled</param>
void PlayerManager::SetAllPlayersEnabled(const bool a_bEnabled)
{
	//Loop all of the players and set enabled state
	for(std::map<RakNet::RakNetGUID, Entity*>::const_iterator playerIter = m_xPlayers.begin(); playerIter != m_xPlayers.end(); ++playerIter)
	{
		playerIter->second->SetEnabled(a_bEnabled);
	}
}

/// <summary>
/// Sets the Colour of a given player
/// </summary>
/// <param name="a_ownerGUID">GUID of the system that owns the player</param>
/// <param name="a_v4Colour">Colour to set the player tod</param>
void PlayerManager::SetPlayerColour(RakNet::RakNetGUID a_ownerGUID, glm::vec4 a_v4Colour)
{
	//Try and find the player entity
	Entity* pPlayerEntity = m_xPlayers[a_ownerGUID];
	if(!pPlayerEntity)
	{
		return;
	}

	//Get the player data component
	PlayerDataComponent* pPlayerData = pPlayerEntity->GetComponent<PlayerDataComponent*>();
	if(!pPlayerEntity)
	{
		return;
	}

	//Set the colour of the player
	pPlayerData->SetPlayerColour(a_v4Colour);
}


/// <summary>
/// Destroy a player entity
/// </summary>
/// <param name="a_playerGUID">System that owns the player</param>
void PlayerManager::DestroyPlayer(RakNet::RakNetGUID a_playerGUID)
{
	//Find the player in the map to retrive the entity
	Entity* pPlayerEntity = m_xPlayers[a_playerGUID];

	//Check that we have a vaid player
	if(pPlayerEntity == nullptr)
	{
		return;
	}

	//Check that this entity is a player
	PlayerDataComponent* pPlayerData = pPlayerEntity->GetComponent<PlayerDataComponent*>();
	if (!pPlayerData)
	{
		return;
	}

	//Remove from the players list
	m_xPlayers.erase(a_playerGUID);

	//Destory the player entity
	delete pPlayerEntity;
}
