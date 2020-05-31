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
	
	//Function to Create Player
	static void CreatePlayersForAllClients(const std::vector<ConnectedClientInfo>& a_vConnectedClients);
	static void CreatePlayer(RakNet::RakNetGUID a_ownerGUID);

	//Functions to Destory Player
	static void DestroyPlayer(Entity* a_pPlayer);
	static void DestroyPlayer(RakNet::RakNetGUID a_playerGUID);

	//Functions to generate the level
	static void LoadLevel();

	//Collision World
	rp3d::CollisionWorld* GetCollisionWorld() const;
	
	//Function to an entity for safe deletion
	void DeleteEntityAfterUpdate(Entity* a_pEntity);
	void ProcessDeletions();
	
private:

	//Constructor
	GameManager();

	//Queue of entities to delete after this frame ends
	std::vector<Entity*> m_vDeleteEntityQueue;

	//Collision World that the game happens in
	rp3d::CollisionWorld* m_pCollisionWorld;
	
	//Instance of this sigleton
	static GameManager* s_pInstance;
};

#endif //__GAME_MANAGER_H__