#ifndef __NETWORK_SERVER_H__
#define __NETWORK_SERVER_H__

//C++ Includes
#include <vector>

//Raknet Includes
#include <NetworkIDManager.h>

//Project Includes
#include "ClientServerBase.h"
#include "Authenticator.h"
#include "NetworkReplicator.h"

//Struct to store info about a connected client
struct ConnectedClientInfo {
	RakNet::SystemAddress m_clientAddress;
	int m_playerId;
};

class NetworkServer : public ServerClientBase
{
public:
	//Server Connection States
	typedef enum ServerGameStates {
		SERVER_CLIENTS_CONNECTED,
		SERVER_PROCESSING_EVENTS,
		SERVER_HANDLE_CLIENT_DISCONNECT,

		SERVER_MAX_CONNECTION_STATES
	} ServerConnectionState;


	//Constructor/Destructor
	NetworkServer();
	virtual ~NetworkServer();

	//Update and Init
	void Init();
	void Update();

private:
	//Functions for pre game connection of clients 
	//to the server
	void DoPreGameServerEvents();
	void DoGamePlayingServerEvents();

	//Sending Messages Events
	void SendMessageToClient(int a_iClientID, RakNet::BitStream& a_data, PacketPriority a_priotity, PacketReliability a_reliability);
	void SendMessageToClient(RakNet::SystemAddress a_clientAddress, RakNet::BitStream& a_data, PacketPriority a_priotity, PacketReliability a_reliability);
	void SendMessageToClient(RakNet::SystemAddress a_clientAddress, RakNet::MessageID a_eMessage, PacketPriority a_priotity, PacketReliability a_reliability);
	void SendMessageToAllClients(RakNet::BitStream& a_data, PacketPriority a_priotity, PacketReliability a_reliability);

	//Server Authenticator - used to verify usernames/passwords
	Authenticator* m_oServerAuthenticator;

	//Current State of the server - Waiting, Playing etc.
	ServerGameStates m_eServerState;

	//Current number of connected clients
	int m_iConnectedClients = 0;
	int m_iReadyClients = 0;

	//List of clients connected to the server
	std::vector<ConnectedClientInfo> m_vConnectedClients;

	//Number of players required to start the game
	const int requiredPlayerCount = 2;

	//TODO Remove and put in client server base
	// ReplicaManager3 requires NetworkIDManager to lookup pointers from numbers.
	RakNet::NetworkIDManager* m_pNetworkIdManager;
	//Network Replicator - manages replication of objects
	NetworkReplicator* m_pReplicaManager;
};

#endif // !__NETWORK_SERVER_H__