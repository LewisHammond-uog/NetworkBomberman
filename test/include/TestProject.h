#ifndef __MY_APPLICATION_H__
#define __MY_APPLICATION_H__

#include <stdafx.h>
#include "Application.h"

//Forward declare
namespace RakNet {
	class RakPeerInterface;
}

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
	RakNet::SystemAddress m_serverAddress;

	ConnectionState m_currentState;

	void LogConsoleMessage(const char* a_sMessage);
};

#endif // __MY_APPLICATION_H__