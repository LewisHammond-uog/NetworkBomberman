#include "stdafx.h"
#include "GameManager.h"

//C++ Includes
#include <map>
#include <algorithm>

//Project Includes
#include "ServerClientBase.h"
#include "NetworkReplicator.h"
#include "Entity.h"
//Components
#include "BombSpawnerComponent.h"
#include "ColliderComponent.h"
#include "ConsoleLog.h"
#include "PlayerControlComponent.h"
#include "PlayerDataComponent.h"
#include "TransformComponent.h"
#include "SpherePrimitiveComponent.h"
#include "LevelManager.h"

//Init Statics
GameManager* GameManager::s_pInstance = nullptr;

/// <summary>
/// Create the Game Manager
/// </summary>
GameManager::GameManager() 
{
	//Create Collision World
	m_pCollisionWorld = new rp3d::CollisionWorld();

	//Create the Level Loader
	m_pLevelManager = new LevelManager();
	
}

/// <summary>
/// Destory the Game Manager
/// </summary>
GameManager::~GameManager()
{
	//Destroy the collision world
	delete m_pCollisionWorld;

	//Delete the level loader
	delete m_pLevelManager;

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
/// Does Game Warm up, creating players and spawning the level
/// </summary>
void GameManager::WarmupGame()
{
	//Create the level
	if (m_pLevelManager) {
		m_pLevelManager->LoadLevel("level");
	}

	//Create Players
	if (m_pvConnectedClients != nullptr) {
		CreatePlayersForAllClients(*m_pvConnectedClients);

		//Disable all of the players
		for (int i = 0; i < m_vpPlayers.size(); ++i)
		{
			m_vpPlayers[i]->SetEnabled(false);
		}
	}
	else
	{
		ConsoleLog::LogError("Cannot Create Players, no connected clients assigned!");
	}
}

/// <summary>
/// Start the Game
/// </summary>
void GameManager::StartGame()
{
	//Enable all of the players
	if(!m_vpPlayers.empty())
	{
		for(int i = 0; i < m_vpPlayers.size(); ++i)
		{
			m_vpPlayers[i]->SetEnabled(true);
		}
	}
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
		return;
	}
	pNetworkReplicator->Reference(pPlayerEntity);
	pNetworkReplicator->Reference(pPlayerTransform);
	pNetworkReplicator->Reference(pSphere);
	pNetworkReplicator->Reference(pPlayerControl);
	pNetworkReplicator->Reference(pBombSpawner);
	pNetworkReplicator->Reference(pPlayerData);
	
	//Add Player to list of players
	m_vpPlayers.push_back(pPlayerEntity);
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
	
	//Remove from the players list
	m_vpPlayers.erase(std::remove(m_vpPlayers.begin(), m_vpPlayers.end(), a_pPlayer), m_vpPlayers.end());
	
	//Destory the player entity
	delete a_pPlayer;
}

/// <summary>
/// Processes the client disconnecting from the server and deletes their player
/// </summary>
/// <param name="a_disconnectionGUID">Client that has disconnected</param>
void GameManager::ProcessDisconnection(const RakNet::RakNetGUID a_disconnectionGUID)
{
	for (std::vector<Entity*>::const_iterator xPlayerIter = m_vpPlayers.begin(); xPlayerIter != m_vpPlayers.end(); ++xPlayerIter)
	{
		Entity* pEntity = *xPlayerIter;
		if (!pEntity)
		{
			break;
		}

		//Check if entity is a player and has data component
		PlayerDataComponent* pPlayerData = dynamic_cast<PlayerDataComponent*>(pEntity->GetComponent(COMPONENT_TYPE::PLAYER_DATA));
		if (!pPlayerData)
		{
			break;;
		}

		RakNet::RakNetGUID guid = pPlayerData->GetPlayerID();

		//We know that we have a player check if the ID is the one we are looking for
		if (a_disconnectionGUID == guid)
		{
			//We have found the player, delete them
			DestroyPlayer(pEntity);
			return;
		}
	}
}

/// <summary>
/// Gets the collision world that the game is occoruing in
/// </summary>
rp3d::CollisionWorld* GameManager::GetCollisionWorld() const
{
	return m_pCollisionWorld;
}

/// <summary>
/// Assigns the players that are connected so that we are able
/// to create players for them
/// </summary>
/// <param name="a_pvConnectedPlayers"></param>
void GameManager::AssignConnectedPlayers(std::vector<ConnectedClientInfo>* a_pvConnectedPlayers)
{
	m_pvConnectedClients = a_pvConnectedPlayers;
}
