#ifndef __GAME_MANAGER_H__
#define __GAME_MANAGER_H__

//Forward Declares
class NetworkReplicator;
namespace RakNet
{
	class NetworkIDManager;
}

class GameManager
{
public:

	//Constructor/Destructor
	GameManager();
	~GameManager();

	static GameManager* GetInstance();
	
	//Update/Draw Functions
	void Update(float a_fDeltaTime);

	//Function to Create Player
	static void CreatePlayers(int a_iPlayerCount, NetworkReplicator* a_pNetworkReplicator);

	//Function to get the network ID manager that should be used by all game objects
	//to assign and check game objects
	RakNet::NetworkIDManager* GetNetworkIDManager();
	NetworkReplicator* GetNetworkReplicator();
private:
	//Pointer to the network ID manager used to assign networkID's
	//todo move to client/server
	//todo implement
	RakNet::NetworkIDManager* m_pNetworkIDManager;
	NetworkReplicator* m_pNetworkReplicator;

	//Instance of this sigleton
	static GameManager* s_pSceneInstance;

	
};

#endif //__GAME_MANAGER_H__