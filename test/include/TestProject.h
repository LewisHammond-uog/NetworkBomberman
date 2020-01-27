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

//Connection State enum
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

	MAX_CONNECTIONS_STATES

}ConnectionState;

//Message types
typedef enum CSNetMessages {
	AUTHENTICATE = ID_USER_PACKET_ENUM + 1,

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
	RakNet::RakPeerInterface* m_pRakPeer;
	RakNet::SystemAddress m_serverAddress;
	void ProcessServerEvents();

	ConnectionState m_currentState;
};

#endif // __MY_APPLICATION_H__