#ifndef __GAME_MANAGER_H__
#define __GAME_MANAGER_H__

//C++ Includes
#include <queue>

//RakNet Includes
#include "NetworkServer.h"
#include "RakNetTypes.h"

//React Phsyics Includes
#include "reactphysics3d.h"

//Forward Declares
class NetworkReplicator;
class Entity;
class LevelManager;
class PlayerManager;
namespace RakNet
{
	class NetworkIDManager;
}

/// <summary>
/// Game Manager that is used to perform game functions such as creating players
/// and updating all entities
/// </summary>
class GameManager
{
public:

	//Destructor
	~GameManager();
	
	static GameManager* GetInstance();
	
	//Update/Draw Functions
	void Update(float a_fDeltaTime);
	void Draw();
	
	//Game Running Functions
	void WarmupGame();
	void StartGame();
	void EndGame();

	//Function to process when a player disconnects
	void ProcessDisconnection(RakNet::RakNetGUID a_disconnectionGUID) const;

	//Collision World
	rp3d::CollisionWorld* GetCollisionWorld() const;

	//Functions to assign the players that need to be created
	void AssignConnectedPlayers(std::vector<ConnectedClientInfo>* a_pvConnectedPlayers);
	
	//Function to an entity for safe deletion
	void DeleteEntityAfterUpdate(Entity* a_pEntity);

private:

	//Constructor
	GameManager();

	//Proccess Deleted Objects
	void ProcessDeletions();

	//Level Loader - to spawn level
	LevelManager* m_pLevelManager;
	
	//Player Manager - to spawn player
	PlayerManager* m_pPlayerManager;
	
	//Queue of entities to delete after this frame ends
	std::vector<Entity*> m_vDeleteEntityQueue;

	//Pointer to the clients that are connected to the server,
	//so we can assign players to a GUID
	std::vector<ConnectedClientInfo>* m_pvConnectedClients{};
	
	//Collision World that the game happens in
	rp3d::CollisionWorld* m_pCollisionWorld;
	
	//Instance of this sigleton
	static GameManager* s_pInstance;
};

#endif //__GAME_MANAGER_H__