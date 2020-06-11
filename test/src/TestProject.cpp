#include <stdafx.h>
#include "TestProject.h"

//RakNet Includes

//Project Includes
#include "NetworkClient.h"
#include "NetworkServer.h"
#include "GameManager.h"

bool TestProject::isServer = false;

TestProject::TestProject()
{
	
}

TestProject::~TestProject()
{

}

bool TestProject::onCreate()
{

	// initialise the Gizmos helper class
	Gizmos::create();
	
	#pragma region Render
	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse(glm::lookAt(m_v3CameraPos, m_v3CameraLookAt, m_v3CameraUp));

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, (float)m_windowWidth / (float)m_windowHeight, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f, 0.25f, 0.25f, 1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	#pragma endregion
	
	return true;
}

void TestProject::Update(float a_deltaTime)
{
	Application_Log* log = Application_Log::Get();

	//Setup Imgui window size and position
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowSize = ImVec2(400.f, 250.f);
	ImVec2 windowPos = ImVec2(io.DisplaySize.x * 0.5f - windowSize.x * 0.5f, io.DisplaySize.y * 0.5f - windowSize.y * 0.5f);

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);

	///////DECISION WINDOW///////////////
	
	if (m_bShowConnectionWindow) {
		ImGui::Begin("Establish Connection", &m_bShowConnectionWindow);

		if (ImGui::Button("Client")) {
			//Create Client
			gameClient = new NetworkClient();
			gameClient->Init();

			//Hide the connection window
			m_bShowConnectionWindow = false;

			//Set is Server
			TestProject::isServer = false;
		}

		if (ImGui::Button("Server")) {
			//Create Server
			gameServer = new NetworkServer();
			
			//Hide the connection window
			m_bShowConnectionWindow = false;

			//Set is Server
			TestProject::isServer = true;
		}

		ImGui::End();
	}
	//////////////////////////////////////
	
	if (gameClient != nullptr) {
		gameClient->Update();
	}

	if (gameServer != nullptr) {
		gameServer->Update();
	}

	// clear all gizmos from last frame
	Gizmos::clear();

	//Update the Game
	GameManager::GetInstance()->Update(a_deltaTime);
	
	static bool show_demo_window = true;

	if (log != nullptr && show_demo_window)
	{
		log->showLog(&show_demo_window);
	}
	//show application log window
	if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(m_window, GLFW_KEY_L) == GLFW_PRESS) {
		show_demo_window = !show_demo_window;
	}
	// quit our application when escape is pressed
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void TestProject::Draw()
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);

	//Draw the game
	GameManager::GetInstance()->Draw();

	// draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);


}

void TestProject::Destroy()
{
	//Deinitalise the client or sever
	if(gameClient)
	{
		gameClient->DeInit();
		delete gameClient;
	}
	if(gameServer)
	{
		gameServer->DeInit();
		delete gameServer;
	}
	

	//Delete the game manager
	delete GameManager::GetInstance();

	Gizmos::destroy();
}
