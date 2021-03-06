#include "stdafx.h"
#include "GameManager.h"

//C++ Includes
#include <map>

//Project Includes
#include "Entity.h"
#include "Level.h"
#include "LevelManager.h"
#include "PlayerManager.h"
#include "ConsoleLog.h"

//Init Statics
GameManager* GameManager::s_pInstance = nullptr;

/// <summary>
/// Create the Game Manager
/// </summary>
GameManager::GameManager() :
	m_eCurrentGameState(GAME_STATE::GAME_STATE_IDLE)
{
	//Create Collision World
	m_pCollisionWorld = new rp3d::CollisionWorld();

	//Create the Level Loader
	m_pLevelManager = new LevelManager();

	//Create the Player Manager
	m_pPlayerManager = new PlayerManager();
	
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

	//Delete the Player Manager
	delete m_pPlayerManager;

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
		m_pLevelManager->LoadNextLevel();
	}

	//Create Players and disable players
	if (m_pvConnectedClients != nullptr && m_pPlayerManager != nullptr) {
		
		m_pPlayerManager->CreatePlayersForAllClients(*m_pvConnectedClients);
		m_pPlayerManager->SetAllPlayersEnabled(false);
	}
	else
	{
		ConsoleLog::LogError("Cannot Create Players, no connected clients assigned!");
	}

	//Set the Game State
	m_eCurrentGameState = GAME_STATE::GAME_STATE_WARMUP;
}

/// <summary>
/// Start the Game
/// </summary>
void GameManager::StartGame()
{
	//Enable all of the players
	if (m_pPlayerManager) {
		m_pPlayerManager->SetAllPlayersEnabled(true);
	}

	//Set the Game State
	m_eCurrentGameState = GAME_STATE::GAME_STATE_PLAYING;
}

/// <summary>
/// Ends the game is progress
/// </summary>
void GameManager::EndGame()
{
	//Set the Game State
	m_eCurrentGameState = GAME_STATE::GAME_STATE_ENDED;

	//Unload the level
	if(m_pLevelManager)
	{
		m_pLevelManager->UnloadLevel();
	}
	
	//Unload the players
	if(m_pPlayerManager)
	{
		m_pPlayerManager->DestroyAllPlayers();
	}
}

/// <summary>
/// Set the map rotation on the level manager
/// </summary>
/// <param name="a_vsMapRotation"></param>
void GameManager::SetLevelRotation(const std::vector<std::string> a_vsMapRotation) const
{
	if(m_pLevelManager)
	{
		m_pLevelManager->SetLevelRotation(a_vsMapRotation);
	}
}


/// <summary>
/// Update all of the entities in the game world
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

	//Null Check player manager
	if(m_pPlayerManager)
	{
		//If there is only 1 player left then end the game,
		//catch just incase 2 players die in the same frame
		if(m_eCurrentGameState == GAME_STATE::GAME_STATE_PLAYING &&
			m_pPlayerManager->GetPlayerCount() <= 1)
		{
			//End the game
			EndGame();
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
	for (std::vector<Entity*>::const_iterator xDelIter = m_vDeleteEntityQueue.begin(); xDelIter != m_vDeleteEntityQueue.end();)
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
/// Processes the client disconnecting from the server and deletes their player
/// </summary>
/// <param name="a_disconnectionGUID">Client that has disconnected</param>
void GameManager::ProcessDisconnection(const RakNet::RakNetGUID a_disconnectionGUID) const
{
	//Delete Player from the Player Manager
	if(m_pPlayerManager)
	{
		m_pPlayerManager->DestroyPlayer(a_disconnectionGUID);
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


/// <summary>
/// Gets the current state of the game
/// </summary>
/// <returns></returns>
GAME_STATE GameManager::GetGameState() const
{
	return m_eCurrentGameState;
}
