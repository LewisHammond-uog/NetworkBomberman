#ifndef __NETWORK_DATA_BLACKBOARD_H__
#define __NETWORK_DATA_BLACKBOARD_H__
#include <vector>

//RakNet Includes
#include "BitStream.h"

//todo - move to a different file?
struct PlayerInputNetworkData
{
	int iPlayerID;
	glm::vec2 v2MovementInputs;
};

//todo - More Generic Functions?
/// <summary>
/// Class for network data that is not dealt with by the ReplicaManager to be
/// black-boarded to so that server/client code can interact with other
/// game systems
/// </summary>
class NetworkDataBlackboard
{
	
public:
	
	//Destructor
	~NetworkDataBlackboard() = default;
	
	//Singleton Function
	static NetworkDataBlackboard* GetInstance();

	//Server Functions
	std::vector<PlayerInputNetworkData*> GetPlayerInputNetworkData(int a_iPlayerID);
	void AddReceivedNetworkData(RakNet::BitStream& a_data);
	
	//Client Functions
	void SendPlayerInputNetworkData(PlayerInputNetworkData* a_data);


	
private:
	//Private Constructor
	NetworkDataBlackboard() = default;

	//List of unread input messages
	std::vector<PlayerInputNetworkData*> m_vUnreadInputMessages;

	//Store the instance of the blackboard
	static NetworkDataBlackboard* m_pInstance;
	
};

#endif //!__NETWORK_DATA_BLACKBOARD_H__
