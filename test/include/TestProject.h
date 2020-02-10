#ifndef __MY_APPLICATION_H__
#define __MY_APPLICATION_H__

#include <stdafx.h>
#include "Application.h"

//Rak Net Includes
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>

//Forward declare
namespace RakNet {
	class RakPeerInterface;
}

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

	CSNET_MESSAGE_END
}CSNetMessages;

// Derived application class that wraps up all globals neatly
class TestProject : public Application
{
public:

	TestProject();
	virtual ~TestProject();

protected:

	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	//Raknet Variables and connection status
	void ProcessServerEvents(); //[To do client processing events]
	RakNet::RakPeerInterface* m_pRakPeer;
	RakNet::SystemAddress m_serverAddress;

	ConnectionState m_currentState;

	void LogConsoleMessage(const char* a_sMessage);
};

#endif // __MY_APPLICATION_H__