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
	static void Update(float a_fDeltaTime);

	//Function to Create Player
	static void CreatePlayers(int a_iPlayerCount);

private:

	//Instance of this sigleton
	static GameManager* s_pInstance;

};

#endif //__GAME_MANAGER_H__