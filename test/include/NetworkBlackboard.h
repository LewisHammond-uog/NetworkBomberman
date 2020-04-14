#ifndef __NETWORK_DATA_BLACKBOARD_H__
#define __NETWORK_DATA_BLACKBOARD_H__

//C++ Includes
#include <vector>

//RakNet Includes
#include "BitStream.h"

//Struct for data that we get send to
//the blackboard
class NetworkData
{
public:
	NetworkData() = default;
	
	RakNet::MessageID m_dataType; //Type of Data
	int m_iPlayerID; //ID of the player this data is intended for
	RakNet::BitStream m_data; //Remaining Data after data type and player ID have been stripped
};

/// <summary>
/// Class for network data that is not dealt with by the ReplicaManager to be
/// black-boarded to so that server/client code can interact with other
/// game systems
/// </summary>
class NetworkBlackboard
{
	
public:
	
	//Destructor
	//todo destructor should remove all unread messages
	~NetworkBlackboard();
	
	//Singleton Function
	static NetworkBlackboard* GetInstance();

	//Server Functions
	std::vector<NetworkData*> GetNetworkData(RakNet::MessageID a_dataType, int a_iPlayerID);
	void AddReceivedNetworkData(RakNet::BitStream& a_data);
	void AddReceivedNetworkData(NetworkData* a_pBlackboardData);

	//Client Functions
	void SendBlackboardDataToServer(RakNet::MessageID a_dataType, int a_iPlayerID, RakNet::BitStream& a_data);

private:
	//Private Constructor
	NetworkBlackboard() = default;

	//List of unread input messages
	std::vector<NetworkData*> m_vUnreadMessages;

	//Store the instance of the blackboard
	static NetworkBlackboard* m_pInstance;
	
};

#endif //!__NETWORK_DATA_BLACKBOARD_H__
