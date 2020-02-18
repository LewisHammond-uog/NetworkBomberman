#ifndef __NETWORK_H__
#define __NETWORK_H__

//Rak Net Includes
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>

#include <string.h>

//Defines for max clients and server port
#define SERVER_PORT 6000
#define MAX_CLIENTS 2

//Connection State, not reltated to raknet
//just to move through our own switch statement
typedef enum ConnectionState {
	//Client
	CLIENT_SERVER_DECISION,
	CLIENT_CONNECTION,
	CLIENT_WAITING_FOR_CONNECTION,
	CLIENT_AUTHORISATION,
	CLIENT_WAITING_FOR_AUTHORISATION,

	//Server
	SERVER_CLIENTS_CONNECTED,
	SERVER_PROCESSING_EVENTS,
	SERVER_HANDLE_CLIENT_DISCONNECT,

	//Game
	START_GAME,
	GAME_RUNNING,
	END_GAME,

	MAX_CONNECTIONS_STATES

}ConnectionState;

//Custom Message Types for custom data that we are sending
//over hte network, extend from RakNets Messages (i.e ID_CONNECTION_REQUEST_ACCEPTED)
typedef enum CSNetMessages {
	SERVER_AUTHENTICATE_SUCCESS = ID_USER_PACKET_ENUM + 1,
	SERVER_AUTHENTICATE_FAIL,

	CLIENT_LOGIN_DATA,
	CLIENT_REGISTER_DATA,
	CLIENT_TEST_DATA,

	CSNET_MESSAGE_END
}CSNetMessages;

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

protected:

	//Raknet peer for packet send/receive
	RakNet::RakPeerInterface* m_pRakPeer;
	ConnectionState m_eConnectionState;
	RakNet::SystemAddress m_serverAddress;

	//Function for Debug Log console message
	//TODO - Move this somewhere else
	void LogConsoleMessage(const char* m_Message) {
		Application_Log* log = Application_Log::Get();

		if (log != nullptr) {
			log->addLog(LOG_LEVEL::LOG_INFO, m_Message);
		}
	}
};

#endif // !__NETWORK_H__s

