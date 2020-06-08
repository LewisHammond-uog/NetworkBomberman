#ifndef __MY_APPLICATION_H__
#define __MY_APPLICATION_H__

#include <stdafx.h>
#include "Application.h"

//Rak Net Includes

//Forward Delcleration
class NetworkServer;
class NetworkClient;

// Derived application class that wraps up all globals neatly
class TestProject : public Application
{
public:

	TestProject();
	virtual ~TestProject();

	//todo change to be better
	//Dirty debug static just to test replication
	static bool isServer;

protected:

	bool onCreate() override;
	void Update(float a_deltaTime) override;
	void Draw() override;
	void Destroy() override;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

private:
	NetworkServer* gameServer; //Game Server (if we are the host)
	NetworkClient* gameClient; //Game Client (always used), connects to the server and shares info
	bool showConnectionWindow = true;

	//Camera Settings
	const glm::vec3 m_v3CameraPos = glm::vec3(10.f, 25.f, 15.f);
	const glm::vec3 m_v3CameraLookAt = glm::vec3(10.f, 0.f, 10.f);
	const glm::vec3 m_v3CameraUp = glm::vec3(0,1,0);
	
};

#endif // __MY_APPLICATION_H__