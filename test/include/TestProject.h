#ifndef __MY_APPLICATION_H__
#define __MY_APPLICATION_H__

#include <stdafx.h>
#include "Application.h"

//Rak Net Includes
#include <MessageIdentifiers.h>
#include <RakNetTypes.h>

//Forward Delcleration
class ServerClientBase;

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

private:
	ServerClientBase* networkUpdater; //Network Updater (i.e client or server)
	bool showConnectionWindow = true;
};

#endif // __MY_APPLICATION_H__