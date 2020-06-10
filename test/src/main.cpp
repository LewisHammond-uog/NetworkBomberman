#include <stdafx.h>
#include "TestProject.h"

//C++ Include
#include <fstream>

#include "LevelManager.h"

#define DEFAULT_SCREENWIDTH 1920
#define DEFAULT_SCREENHEIGHT 1080
// main that controls the creation/destruction of an application
int WINAPI WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	// explicitly control the creation of our application
	TestProject* app = new TestProject();
	app->run("Test Project", DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT, false);

	// explicitly control the destruction of our application
	delete app;

	return 0;
}