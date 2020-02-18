#ifndef __NETWORK_SERVER_H__
#define __NETWORK_SERVER_H__

//Project Includes
#include "ClientServerBase.h"

class NetworkServer : public ServerClientBase
{
public:
	//Server Connection States
	typedef enum ServerConnectionState {
		SERVER_CLIENTS_CONNECTED,
		SERVER_PROCESSING_EVENTS,
		SERVER_HANDLE_CLIENT_DISCONNECT,

		SERVER_MAX_CONNECTION_STATES
	} ServerConnectionState;


	//Constructor/Destructor
	NetworkServer();
	~NetworkServer();

	//Update and Init
	void Init();
	void Update();

private:
	//Functions for pre game connection of clients 
	//to the server
	void DoPreGameServerEvents();

	//Connection State
	ServerConnectionState m_eConnectionState;
};

#endif // !__NETWORK_SERVER_H__