#include <stdafx.h>
#include "TestProject.h"
#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"

#define MAX_CLIENTS 10
#define SERVER_PORT 60000

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
	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse(glm::lookAt(glm::vec3(10, 10, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, m_windowWidth / (float)m_windowHeight, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f, 0.25f, 0.25f, 1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

void TestProject::Update(float a_deltaTime)
{
	Application_Log* log = Application_Log::Get();

	char str[512];
	bool isServer = false;
	RakNet::Packet* packet;

	//Singleton for the Network manager
	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();

	//Buttons
	bool clientSelected = ImGui::Button("Client");
	bool serverSelected = ImGui::Button("Server");

	/* SETUP SERVER/CLIENT */
	if (clientSelected) {
		RakNet::SocketDescriptor sd;
		peer->Startup(1, &sd, 1);
		isServer = false;

		log->addLog(LOG_INFO, "Starting the Client");
		peer->Connect("127.0.0.1", SERVER_PORT, 0, 0);

	}
	else if (serverSelected) {
		RakNet::SocketDescriptor sd(SERVER_PORT, 0);
		peer->Startup(MAX_CLIENTS, &sd, 1);
		isServer = true;

		log->addLog(LOG_INFO, "Starting the Server");
		peer->SetMaximumIncomingConnections(MAX_CLIENTS);
	}

	while (1) {
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive()) {

			//Get the message type and deal with it
			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				log->addLog(LOG_INFO, "Another client has disconnected.\n");
				break;
			case ID_REMOTE_CONNECTION_LOST:
				log->addLog(LOG_INFO, "Another client has lost the connection.\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				log->addLog(LOG_INFO, "Another client has connected.\n");
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				log->addLog(LOG_INFO, "Our connection request has been accepted.\n");
				break;
			case ID_NEW_INCOMING_CONNECTION:
				log->addLog(LOG_INFO, "A connection is incoming.\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				log->addLog(LOG_INFO, "The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				if (isServer) {
					log->addLog(LOG_INFO, "A client has disconnected.\n");
				}
				else {
					log->addLog(LOG_INFO, "We have been disconnected.\n");
				}
				break;
			case ID_CONNECTION_LOST:
				if (isServer) {
					log->addLog(LOG_INFO, "A client lost the connection.\n");
				}
				else {
					log->addLog(LOG_INFO, "Connection lost.\n");
				}
				break;
			default:
				log->addLog(LOG_INFO, "Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}

		}
	}


	//RakNet::RakPeerInterface::DestroyInstance(peer);

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

	#pragma endregion

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
	Gizmos::destroy();
}


