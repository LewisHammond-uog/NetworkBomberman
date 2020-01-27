#include <stdafx.h>
#include "TestProject.h"

#include <RakPeerInterface.h>
#include<BitStream.h>

#define MAX_CLIENTS 10
#define SERVER_PORT 60000

enum GameMessages {
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1
};

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

	//Get instance of rakPeerInterface and set state
	m_pRakPeer = RakNet::RakPeerInterface::GetInstance();
	m_currentState = ConnectionState::CLIENT_SERVER_DECISION;


	#pragma region Render


	// initialise the Gizmos helper class
	Gizmos::create();
	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse(glm::lookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, m_windowWidth / (float)m_windowHeight, 0.1f, 1000.0f);

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

	

	#pragma region Rendering

	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement(m_cameraMatrix, a_deltaTime, 10);

	// clear all gizmos from last frame
	Gizmos::clear();

	// add an identity matrix gizmo
	Gizmos::addTransform(glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));
	// add a 20x20 grid on the XZ-plane
	for (int i = 0; i < 21; ++i)
	{
		Gizmos::addLine(glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),
			i == 10 ? glm::vec4(1, 1, 1, 1) : glm::vec4(0, 0, 0, 1));

		Gizmos::addLine(glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),
			i == 10 ? glm::vec4(1, 1, 1, 1) : glm::vec4(0, 0, 0, 1));
	}
	#pragma endregion


	static bool showConnectionWindow = true;

	switch (m_currentState) {
		case(ConnectionState::CLIENT_SERVER_DECISION): 
		{
			ImGuiIO& io = ImGui::GetIO();
			ImVec2 windowSize = ImVec2(400.f, 250.f);
			ImVec2 windowPos = ImVec2(io.DisplaySize.x * 0.5f - windowSize.x * 0.5f, io.DisplaySize.y * 0.5f - windowSize.y * 0.5f);

			ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
			ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
			ImGui::Begin("Establish Connection", &showConnectionWindow);

			if (ImGui::Button("Client")) {
				m_currentState = ConnectionState::CLIENT_CONNECTION;
				RakNet::SocketDescriptor sd;
				m_pRakPeer->Startup(1, &sd, 1);

				if (log != nullptr) {
					log->addLog(LOG_LEVEL::LOG_INFO, "Client Startup");
				}
			}

			if (ImGui::Button("Server")) {
				m_currentState = ConnectionState::SERVER_PROCESSING_EVENTS;
				RakNet::SocketDescriptor sd(SERVER_PORT, 0);
				m_pRakPeer->Startup(MAX_CLIENTS, &sd, 1);

				if (log != nullptr) {
					log->addLog(LOG_LEVEL::LOG_INFO, "Server Startup");
				}

				m_pRakPeer->SetMaximumIncomingConnections(MAX_CLIENTS);
			}

			ImGui::End();

			break;
		}
		default:
			break;
	}


	static bool show_demo_window = true;
	ImGui::ShowDemoWindow(&show_demo_window);
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

	// draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);


}

void TestProject::Destroy()
{
	RakNet::RakPeerInterface::DestroyInstance(m_pRakPeer);
	Gizmos::destroy();
}


