#ifndef __NETWORK_SERVER_H__
#define __NETWORK_SERVER_H__

//C++ Includes
#include <vector>

//Raknet Includes

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
	typedef enum class ServerGameStates {
		SERVER_NOT_INIT, //Server is not initalised
		SERVER_CLIENTS_CONNECTING, //We are connecting clients to the server
		SERVER_GAME_WARMUP, //The server is warming up
		SERVER_GAME_PLAYING, //The server is playing the game
		SERVER_GAME_OVER, //The game is over we are resetting the game
		
		SERVER_MAX_CONNECTION_STATES
	} ServerConnectionState;


	//Constructor/Destructor
	NetworkServer();
	virtual ~NetworkServer();

	//Update and Init
	void Init(int a_iMaxPlayerCount, int a_iMinReadyPlayers, float a_iWarmupTime, std::vector<
	          std::string> a_vsSelectedLevels);
	void Update() override;
	void DeInit() override;

private:
	
	//Functions for pre game connection of clients 
	//to the server
	void HandlePreGamePackets(RakNet::Packet* a_pPacket);
	void HandelPlayingGamePackets(RakNet::Packet* a_pPacket);

	//Packet Independent Events
	void DoGameWarmupServerEvents();
	void DoGamePlayingEvents();

	//Sending Messages Events
	void SendMessageToClient(RakNet::SystemAddress a_clientAddress, RakNet::BitStream& a_data, PacketPriority a_priority, PacketReliability a_reliability, 
							ORDERING_CHANNELS a_orderingChannel = ORDERING_CHANNEL_GENERAL) const;
	void SendMessageToClient(RakNet::SystemAddress a_clientAddress, RakNet::MessageID a_eMessage, PacketPriority a_priority, PacketReliability a_reliability, 
							ORDERING_CHANNELS a_orderingChannel = ORDERING_CHANNEL_GENERAL) const;
	void SendMessageToAllClients(RakNet::BitStream& a_data, PacketPriority a_priority, PacketReliability a_reliability, 
							ORDERING_CHANNELS a_orderingChannel = ORDERING_CHANNEL_GENERAL);

	//Client Disconnections
	bool HandleDisconnectPackets(RakNet::Packet* a_pPacket);
	void DisconnectClient(RakNet::RakNetGUID a_clientGUID);
	

	//Server Authenticator - used to verify usernames/passwords
	Authenticator* m_pServerAuthenticator;   

	//Current State of the server - Waiting, Playing etc.
	ServerGameStates m_eServerState;

	//Current number of connected clients
	int m_iReadyClients = 0;

	//List of clients connected to the server
	std::vector<ConnectedClientInfo> m_vConnectedClients;

	//Server Settings
	int m_iMinReadyPlayers = 1;
	float m_fWarmupDuration = 1.0f;
};

#endif // !__NETWORK_SERVER_H__