#ifndef __MY_APPLICATION_H__
#define __MY_APPLICATION_H__

#include <stdafx.h>
#include "Application.h"

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

	bool initalized = false;

};

#endif // __MY_APPLICATION_H__