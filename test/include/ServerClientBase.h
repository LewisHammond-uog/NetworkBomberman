#ifndef __SERVER_CLIENT_BASE_H__
#define __SERVER_CLIENT_BASE_H__

//Rak Net Includes
#include <MessageIdentifiers.h>
#include <NetworkIDManager.h>
#include <RakNetTypes.h>

//Project Includes
#include "NetworkReplicator.h"

//Defines for max clients and server port
#define SERVER_PORT 6000
#define MAX_CLIENTS 8

//Custom Message Types for custom data that we are sending
//over hte network, extend from RakNets Messages (i.e ID_CONNECTION_REQUEST_ACCEPTED)
typedef enum CSNetMessages {
	SERVER_AUTHENTICATE_SUCCESS = ID_USER_PACKET_ENUM + 1,
	SERVER_AUTHENTICATE_FAIL,

	CLIENT_LOGIN_DATA,
	CLIENT_REGISTER_DATA,
	CLIENT_READY_TO_PLAY,

	CSNET_MESSAGE_END
}CSNetMessages;

//Custom Message Types for game messages
//extends from RakNets Messages (i.e ID_CONNECTION_REQUEST_ACCEPTED)
typedef enum CSGameMessages {
	CLIENT_PLAYER_INPUT_DATA = CSNET_MESSAGE_END + 1,
	CLIENT_PLAYER_CREATE_BOMB,

	SERVER_GAME_STARTING,
	SERVER_VERIFIY_PLAYER_POS,
	SERVER_OTHER_PLAYER_POS


} CSGameMessages;

/*
Abstract class that is used for functionality shared between clients and severs
*/
class ServerClientBase
{
public:

	//Pure Virtual Function to make sure that network clients/servers have
	//an init and update funciton
	virtual void Init() = 0;
	virtual void Update() = 0;

	static RakNet::NetworkIDManager* GetNetworkIDManager();
	static NetworkReplicator* GetNetworkReplicator();


protected:
	
	//Protected Constructor/Destructor so that this class cannot be created
	//without it being a server or a client
	ServerClientBase();
	~ServerClientBase();
	

	//Raknet peer for packet send/receive
	RakNet::RakPeerInterface* m_pRakPeer;
	RakNet::SystemAddress m_serverAddress;

private:
	//Members for the replica and network ID Management
	// ReplicaManager3 requires NetworkIDManager to lookup pointers from numbers.
	static RakNet::NetworkIDManager* s_pNetworkIdManager;
	//Network Replicator - manages replication of objects
	static NetworkReplicator* s_pReplicaManager;

};


#endif //!__SERVER_CLIENT_BASE_H__

