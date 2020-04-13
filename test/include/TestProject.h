#ifndef __MY_APPLICATION_H__
#define __MY_APPLICATION_H__

#include <stdafx.h>
#include "Application.h"

//Rak Net Includes
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>

//Forward Delcleration
class NetworkServer;
class NetworkClient;

// Derived application class that wraps up all globals neatly
class TestProject : public Application
{
public:

	TestProject();
	virtual ~TestProject();

	//Dirty debug static just to test replication
	static bool isServer;

protected:

	virtual bool onCreate();
	virtual void Update(float a_deltaTime);
	virtual void Draw();
	virtual void Destroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

private:
	NetworkServer* gameServer; //Game Server (if we are the host)
	NetworkClient* gameClient; //Game Client (always used), connects to the server and shares info
	bool showConnectionWindow = true;
};

#endif // __MY_APPLICATION_H__