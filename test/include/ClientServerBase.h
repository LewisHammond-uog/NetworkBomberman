#ifndef __NETWORK_H__
#define __NETWORK_H__

//Rak Net Includes
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>
#include <string.h>

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

	static bool isServer;

	//Function for Debug Log console message
	//TODO - Move this somewhere else
	static void LogConsoleMessage(const char* m_Message) {
		Application_Log* log = Application_Log::Get();

		if (log != nullptr) {
			log->addLog(LOG_LEVEL::LOG_INFO, m_Message);
		}
	}

protected:

	//Raknet peer for packet send/receive
	RakNet::RakPeerInterface* m_pRakPeer;
	RakNet::SystemAddress m_serverAddress;




};

#endif // !__NETWORK_H__s

