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
		GAME_PLAYING
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
		CLIENT_WAITING_FOR_GAME_START,

		//Start Game

		//Game Shutdown

		CLIENT_MAX_CONNECTION_STATES
	} ClientConnectionState;

	//Constructor/Destructor
	NetworkClient();
	virtual ~NetworkClient();

	void Init() override;
	void Update() override;
	void DeInit() override;

	//Init Imgui Function
	void InitImguiWindow();
	
	//Functions to send a message to the server
	void SendMessageToServer(RakNet::BitStream& a_data, PacketPriority a_priority, PacketReliability a_reliability, ORDERING_CHANNELS a_orderingChannel = ORDERING_CHANNEL_GENERAL) const;
	void SendMessageToServer(RakNet::MessageID a_eMessage, PacketPriority a_priority, PacketReliability a_reliability, ORDERING_CHANNELS a_orderingChannel = ORDERING_CHANNEL_GENERAL) const;

private:

	//Functions to run different network events based on 
	//the current game state
	void DoClientConnectionEvents();
	void DoClientPreGameEvents();
	void DoClientGameEvents();

	//Function to connect to the server
	void ConnectToServer(const char* a_zcIPAddress);
	
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

