#ifndef __NETWORK_CLIENT_H__
#define __NETWORK_CLIENT_H__

//Project includes
#include "ServerClientBase.h"

//RaknetIncludes

//Project Includes
#include "NetworkOrderingChannels.h"
#include "NetworkReplicator.h"

class NetworkClient : public ServerClientBase
{
public:

	//Client Running State
	typedef enum class ClientLocalState {
		NOT_CONNECTED,
		PRE_GAME,
		GAME_PLAYING,
		GAME_OVER
	} ClientLocalState;

	//Client Connection States
	typedef enum class ClientConnectionState {
		//Setup Connection to the server
		CLIENT_START_CONNECTION,
		CLIENT_WAITING_FOR_CONNECTION,
		CLIENT_FAILED_CONNECTION,
		CLIENT_ENTER_AUTH_DETAILS,
		CLIENT_WAITING_FOR_AUTHORISATION,
		CLIENT_FAILED_AUTHORISATION,

		//Setup Game
		CLIENT_INIT_PREGAME,
		CLIENT_SEND_READY,
		CLIENT_WAITING_FOR_READY_PLAYERS,
		CLIENT_WARMUP,

		//Game Over
		

		CLIENT_MAX_CONNECTION_STATES
	} ClientConnectionState;

	//Constructor/Destructor
	NetworkClient();
	virtual ~NetworkClient();

	void Init();
	void Update() override;
	void DeInit() override;

	//Functions to send a message to the server
	void SendMessageToServer(RakNet::BitStream& a_data, PacketPriority a_priority, PacketReliability a_reliability, ORDERING_CHANNELS a_orderingChannel = ORDERING_CHANNEL_GENERAL) const;
	void SendMessageToServer(RakNet::MessageID a_eMessage, PacketPriority a_priority, PacketReliability a_reliability, ORDERING_CHANNELS a_orderingChannel = ORDERING_CHANNEL_GENERAL) const;

private:

	//Functions to deal with client connections, send out data to
	//the server
	void DoClientConnectionEvents();
	void DoClientPreGameEvents();
	void DoClientPostGameEvents();

	//Functions do deal with incoming packets from the server
	void HandleClientConnectionPackets(RakNet::Packet* a_pPacket);
	void HandleClientPreGamePackets(RakNet::Packet* a_pPacket);
	void HandleClientGamePackets(RakNet::Packet* a_pPacket);
	
	//Function to connect to the server
	void ConnectToServer(const char* a_zcIPAddress) const;
	
	//Funnction to disconnect to the server
	void DisconnectFromServer();

	//Local (i.e game running, loading) & Connection State
	ClientLocalState m_eClientGameState;
	ClientConnectionState m_eConnectionState;

	//Connection Settings
	const unsigned mc_iConnectionAttemptLimit = 10; //Maximum number of allowed connection attempts
	const unsigned mc_iTimeBetweenConnectionAttempts = 1000 /*once a second*/; //Time in MS between connection attempts
	const RakNet::TimeMS mc_fConnectionTimeout = 10000 /*10 Seconds*/; //Time in MS before we time out a connection 
};

#endif //!__NETWORK_CLIENT_H__

