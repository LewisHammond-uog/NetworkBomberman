#ifndef __NETWORK_SERVER_H__
#define __NETWORK_SERVER_H__

//C++ Includes
#include <vector>

//Raknet Includes
#include <NetworkIDManager.h>

//Project Includes
#include <PacketPriority.h>
#include "ServerClientBase.h"
#include "Authenticator.h"
#include "NetworkOrderingChannels.h"
#include "NetworkReplicator.h"



//Struct to store info about a connected client
struct ConnectedClientInfo {
	RakNet::RakNetGUID m_clientGUID; //GUID is used as it is unique to a system (not a IP address as it may not be unique (i.e LAN connections))
};

class NetworkServer final : public ServerClientBase
{
public:
	//Server Connection States
	typedef enum ServerGameStates {
		SERVER_CLIENTS_CONNECTING,
		SERVER_PROCESSING_EVENTS,

		SERVER_MAX_CONNECTION_STATES
	} ServerConnectionState;


	//Constructor/Destructor
	NetworkServer();
	virtual ~NetworkServer();

	//Update and Init
	void Init() override;
	void Update() override;
	void DeInit() override;

private:
	//Functions for pre game connection of clients 
	//to the server
	void DoPreGameServerEvents();
	void DoGamePlayingServerEvents();

	//Sending Messages Events
	void SendMessageToClient(RakNet::SystemAddress a_clientAddress, RakNet::BitStream& a_data, PacketPriority a_priority, PacketReliability a_reliability, 
							ORDERING_CHANNELS a_orderingChannel = ORDERING_CHANNEL_GENERAL) const;
	void SendMessageToClient(RakNet::SystemAddress a_clientAddress, RakNet::MessageID a_eMessage, PacketPriority a_priority, PacketReliability a_reliability, 
							ORDERING_CHANNELS a_orderingChannel = ORDERING_CHANNEL_GENERAL) const;
	void SendMessageToAllClients(RakNet::BitStream& a_data, PacketPriority a_priority, PacketReliability a_reliability, ORDERING_CHANNELS a_orderingChannel = ORDERING_CHANNEL_GENERAL);

	//Client Disconnections
	void NetworkServer::DisconnectClient(RakNet::RakNetGUID a_clientGUID);
	

	//Server Authenticator - used to verify usernames/passwords
	Authenticator* m_pServerAuthenticator;

	//Current State of the server - Waiting, Playing etc.
	ServerGameStates m_eServerState;

	//Current number of connected clients
	int m_iConnectedClients = 0;
	int m_iReadyClients = 0;

	//List of clients connected to the server
	std::vector<ConnectedClientInfo> m_vConnectedClients;

	//Number of players required to start the game
	const int requiredPlayerCount = 1;
};

#endif // !__NETWORK_SERVER_H__