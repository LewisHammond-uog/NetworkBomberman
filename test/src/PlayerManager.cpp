#include "stdafx.h"
#include "PlayerManager.h"
//C++ Includes
#include <algorithm>

//Project Includes
#include "Entity.h"
#include "PlayerControlComponent.h"
#include "PlayerDataComponent.h"
#include "CylinderPrimitiveComponent.h"
#include "TransformComponent.h"
#include "BombSpawnerComponent.h"
#include "ColliderComponent.h"
#include "GameManager.h"
#include "Level.h"
#include "LevelManager.h"
#include "MapUtils.h"

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
		//Get the spawn point for this player, only spawn if there is a valid spawn
		glm::vec3 v3PlayerSpawn;//Spawn pos, passed by ref
		if(LevelManager::GetCurrentLevel()->GetPlayerSpawn(i, v3PlayerSpawn)) {

			//Create player with GUID of the connected client
			Entity* pPlayer = CreatePlayer(a_vConnectedClients[i].m_clientGUID, v3PlayerSpawn);

			//Set the colour of the player, only if we created one
			if (pPlayer != nullptr) {
				SetPlayerColour(a_vConnectedClients[i].m_clientGUID, Colours::RandomColour());
			}
		}
	}
}

/// <summary>
/// Creates a player entity
/// </summary>
/// <param name="a_ownerGUID">System that owns the player we are creating</param>
/// <param name="a_v3Position">Position to Create player at</param>
/// <returns>Created Player</returns>
Entity* PlayerManager::CreatePlayer(const RakNet::RakNetGUID a_ownerGUID, glm::vec3 a_v3Position)
{
	//Size of the player
	constexpr float fPlayerSize = 1.f;
	
	//Create Player Entity with the required components
	Entity* pPlayerEntity = new Entity(); //This is added to a static entity list when created so we don't need to worry about storing it here
	
	TransformComponent* pPlayerTransform = new TransformComponent(pPlayerEntity); //This is the same for components
	pPlayerTransform->SetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR, a_v3Position);
	pPlayerEntity->AddComponent(pPlayerTransform);

	//Add Drawing Cylinder
	CylinderPrimitiveComponent* pCylinder = new CylinderPrimitiveComponent(pPlayerEntity, fPlayerSize, fPlayerSize);
	pPlayerEntity->AddComponent(pCylinder);

	//Add Collider and Sphere Collider to it
	ColliderComponent* pCollider = new ColliderComponent(pPlayerEntity);
	pCollider->AddSphereCollider(fPlayerSize, glm::vec3(0, 0, 0));
	pPlayerEntity->AddComponent(pCollider);

	//Add Player Control
	PlayerControlComponent* pPlayerControl = new PlayerControlComponent(pPlayerEntity);
	pPlayerEntity->AddComponent(pPlayerControl);

	//Add Bomb Spawner
	BombSpawnerComponent* pBombSpawner = new BombSpawnerComponent(pPlayerEntity);
	pPlayerEntity->AddComponent(pBombSpawner);

	//Add Player Data
	PlayerDataComponent* pPlayerData = new PlayerDataComponent(pPlayerEntity, a_ownerGUID, this);
	pPlayerEntity->AddComponent(pPlayerData);

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
	pNetworkReplicator->Reference(pCylinder);
	pNetworkReplicator->Reference(pPlayerControl);
	pNetworkReplicator->Reference(pBombSpawner);
	pNetworkReplicator->Reference(pPlayerData);

	//Add Player to list of players, if a player already
	//exists then delete it
	if(m_xPlayers[a_ownerGUID] != nullptr)
	{
		delete m_xPlayers[a_ownerGUID];
	}
	m_xPlayers[a_ownerGUID] = pPlayerEntity;

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
		if (playerIter->second != nullptr) {
			playerIter->second->SetEnabled(a_bEnabled);
		}
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
/// <param name="a_bRemoveFromList">If we should remove the player from the list</param>
void PlayerManager::DestroyPlayer(const RakNet::RakNetGUID a_playerGUID)
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
	
	//Destory the player entity after the current update loop has finished
	GameManager::GetInstance()->DeleteEntityAfterUpdate(pPlayerEntity);
}

/// <summary>
/// Destroys all players that have been created by the player manager
/// </summary>
void PlayerManager::DestroyAllPlayers()
{
	//Destroy all of the players by clearing the player map
	//and freeing the memory
	if (!m_xPlayers.empty()) {
		MapUitls::FreeClear(m_xPlayers);
	}
}

/// <summary>
/// Gets the number of players that are alive
/// </summary>
/// <returns></returns>
unsigned PlayerManager::GetPlayerCount() const
{
	return m_xPlayers.size();
}
