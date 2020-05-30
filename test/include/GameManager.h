#ifndef __GAME_MANAGER_H__
#define __GAME_MANAGER_H__

//C++ Includes
#include <queue>

//RakNet Includes
#include "NetworkServer.h"
#include "RakNetTypes.h"

//Forward Declares
class NetworkReplicator;
class Entity;
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
	static GameManager* GetInstance();
	
	//Update/Draw Functions
	void Update(float a_fDeltaTime);
	void Draw();
	
	//Function to Create Player
	static void CreatePlayersForAllClients(const std::vector<ConnectedClientInfo>& a_vConnectedClients);
	static void CreatePlayer(RakNet::RakNetGUID a_ownerGUID);

	//Functions to Destory Player
	static void DestroyPlayer(Entity* a_pPlayer);
	static void DestroyPlayer(RakNet::RakNetGUID a_playerGUID);

	//Function to an entity for safe deletion
	void DeleteEntityAfterUpdate(Entity* a_pEntity);
	void ProcessDeletions();
	
private:

	//Constructor/Destructor
	GameManager() = default;
	~GameManager();

	//Queue of entities to delete after this frame ends
	std::vector<Entity*> m_vDeleteEntityQueue;
	
	//Instance of this sigleton
	static GameManager* s_pInstance;
};

#endif //__GAME_MANAGER_H__