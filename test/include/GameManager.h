#ifndef __GAME_MANAGER_H__
#define __GAME_MANAGER_H__

//Forward Declares
class NetworkReplicator;

class GameManager
{
public:

	//Update/Draw Functions
	void Update(float a_fDeltaTime);

	//Function to Create Player
	static void CreatePlayers(int a_iPlayerCount, NetworkReplicator* a_pNetworkReplicator);
	
};

#endif //__GAME_MANAGER_H__