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
#include "PlayerDataComponent.h"
#include "TransformComponent.h"
#include "SpherePrimitiveComponent.h"

//Init Statics
GameManager* GameManager::s_pInstance = nullptr;

/// <summary>
/// Create the Game Manager
/// </summary>
GameManager::GameManager()
{
	//Create Collision World
	m_pCollisionWorld = new rp3d::CollisionWorld();
}

/// <summary>
/// Destory the Game Manager
/// </summary>
GameManager::~GameManager()
{
	//Destroy the collision world
	delete m_pCollisionWorld;
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

/// <summary>
/// Update the game world
/// </summary>
/// <param name="a_fDeltaTime"></param>
void GameManager::Update(const float a_fDeltaTime)
{
	
	//Get a list of and then update all of the entities
	std::map<const unsigned int, Entity*>::const_iterator xUpdateIter;
	for (xUpdateIter = Entity::GetEntityMap().begin(); xUpdateIter != Entity::GetEntityMap().end(); ++xUpdateIter)
	{
		Entity* pEntity = xUpdateIter->second;
		if (pEntity) {
			if (pEntity->IsEnabled()) {
				pEntity->Update(a_fDeltaTime);
			}
		}
	}

	//Frame is over, delete objects that need to be deleted
	ProcessDeletions();
}

/// <summary>
/// Draw the Game World
/// </summary>
void GameManager::Draw()
{
	//Get a list of and then draw all of the entities
	std::map<const unsigned int, Entity*>::const_iterator xUpdateIter;
	for (xUpdateIter = Entity::GetEntityMap().begin(); xUpdateIter != Entity::GetEntityMap().end(); ++xUpdateIter)
	{
		Entity* pEntity = xUpdateIter->second;
		if (pEntity) {
			if (pEntity->IsEnabled()) {
				pEntity->Draw(nullptr);
			}
		}
	}
}

/// <summary>
/// Delete an entity after all other entities in the scene are updated
/// Also forces the memory free 
/// </summary>
/// <param name="a_pEntity"></param>
void GameManager::DeleteEntityAfterUpdate(Entity* a_pEntity)
{
	//Add Entity to our delete queue
	m_vDeleteEntityQueue.push_back(a_pEntity);
}

/// <summary>
/// Process the deletions of entities
/// </summary>
void GameManager::ProcessDeletions()
{
	//Delete all entities that should be deleted after update
	std::vector<Entity*>::const_iterator xDelIter;
	for (xDelIter = m_vDeleteEntityQueue.begin(); xDelIter != m_vDeleteEntityQueue.end();)
	{
		Entity* pEntity = *xDelIter;
		if (pEntity)
		{
			//Remove from vector and delete
			xDelIter = m_vDeleteEntityQueue.erase(xDelIter);
			delete pEntity;
		}
	}
}

/// <summary>
/// Creates players for all of the clients connected
/// </summary>
/// <param name="a_vConnectedClients">A list of clients that are connected to the server</param>
void GameManager::CreatePlayersForAllClients(const std::vector<ConnectedClientInfo>& a_vConnectedClients)
{
	//Loop through all of the connected clients and create a player for them
	const int iClientCount = a_vConnectedClients.size();
	for(int i = 0; i < iClientCount; ++i)
	{
		//Create player with GUID of the connected client
		CreatePlayer(a_vConnectedClients[i].m_clientGUID);
	}
}

/// <summary>
/// Creates a player entity
/// </summary>
/// <param name="a_ownerGUID">System that owns the player we are creating</param>
void GameManager::CreatePlayer(const RakNet::RakNetGUID a_ownerGUID)
{
	//Create Player Entities with the required components
	Entity* pPlayerEntity = new Entity(); //This is added to a static entity list when created so we don't need to worry about storing it here
	TransformComponent* pPlayerTransform = new TransformComponent(pPlayerEntity); //This is the same for components
	SpherePrimitiveComponent* pSphere = new SpherePrimitiveComponent(pPlayerEntity);
	PlayerControlComponent* pPlayerControl = new PlayerControlComponent(pPlayerEntity);
	BombSpawnerComponent* pBombSpawner = new BombSpawnerComponent(pPlayerEntity);
	PlayerDataComponent* pPlayerData = new PlayerDataComponent(pPlayerEntity, a_ownerGUID);

	//Add these components to the player entity
	pPlayerEntity->AddComponent(pPlayerTransform);
	pPlayerEntity->AddComponent(pSphere);
	pPlayerEntity->AddComponent(pPlayerControl);
	pPlayerEntity->AddComponent(pBombSpawner);
	pPlayerEntity->AddComponent(pPlayerData);

	//Send the entity and components to the replica manager, it is important
	//that we send the player entity first as components rely on having
	//an owner entity
	NetworkReplicator* pNetworkReplicator = ServerClientBase::GetNetworkReplicator();
	if (pNetworkReplicator == nullptr)
	{
		return;
	}
	pNetworkReplicator->Reference(pPlayerEntity);
	pNetworkReplicator->Reference(pPlayerTransform);
	pNetworkReplicator->Reference(pSphere);
	pNetworkReplicator->Reference(pPlayerControl);
	pNetworkReplicator->Reference(pBombSpawner);
	pNetworkReplicator->Reference(pPlayerData);
}

/// <summary>
/// Destroy a player entity
/// </summary>
/// <param name="a_pPlayer">Player Entity</param>
void GameManager::DestroyPlayer(Entity* a_pPlayer)
{
	//Check that the entity we have been passed is valid
	if(!a_pPlayer)
	{
		return;
	}
	
	//Check that this entity is a player
	PlayerDataComponent* pPlayerData = dynamic_cast<PlayerDataComponent*>(a_pPlayer->GetComponent(COMPONENT_TYPE::PLAYER_DATA));
	if(!pPlayerData)
	{
		return;
	}

	//todo - request that we delete all of the objects that we have created

	//Destory the player entity
	delete a_pPlayer;
}

/// <summary>
/// Destroy a player that is assigned to a specific GUID
/// </summary>
/// <param name="a_playerGUID">GUID that owns the player</param>
void GameManager::DestroyPlayer(RakNet::RakNetGUID a_playerGUID)
{
	//Attempt to find an entity that has the player ID of the player
	//we want to destroy
	std::map<const unsigned int, Entity*>::const_iterator xPlayerIter;
	for (xPlayerIter = Entity::GetEntityMap().begin(); xPlayerIter != Entity::GetEntityMap().end(); ++xPlayerIter)
	{
		Entity* pEntity = xPlayerIter->second;
		if(!pEntity)
		{
			break;
		}
		
		//Check if entity is a player and has data component
		PlayerDataComponent* pPlayerData = dynamic_cast<PlayerDataComponent*>(pEntity->GetComponent(COMPONENT_TYPE::PLAYER_DATA));
		if(!pPlayerData)
		{
			break;;
		}

		RakNet::RakNetGUID guid = pPlayerData->GetPlayerID();
		
		//We know that we have a player check if the ID is the one we are looking for
		if(a_playerGUID == guid)
		{
			//We have found the player, delete them
			DestroyPlayer(pEntity);
			return;
		}
	}
}

/// <summary>
/// Load the level
/// </summary>
void GameManager::LoadLevel()
{

	
	
}

/// <summary>
/// Gets the collision world that the game is occoruing in
/// </summary>
rp3d::CollisionWorld* GameManager::GetCollisionWorld() const
{
	return m_pCollisionWorld;
}
