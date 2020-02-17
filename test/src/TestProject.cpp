#include <stdafx.h>
#include "TestProject.h"

//RakNet Includes
#include <RakPeerInterface.h>
#include <BitStream.h>

#include <string.h>

#include "NetworkClient.h"


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

	gameClient = new NetworkClient();
	gameClient->Init();

	#pragma endregion

	return true;
}

void TestProject::Update(float a_deltaTime)
{

	Application_Log* log = Application_Log::Get();

	static bool showConnectionWindow = true;

	//Setup Imgui window size and position
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowSize = ImVec2(400.f, 250.f);
	ImVec2 windowPos = ImVec2(io.DisplaySize.x * 0.5f - windowSize.x * 0.5f, io.DisplaySize.y * 0.5f - windowSize.y * 0.5f);

	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);

	gameClient->Update();

	/*
	switch (m_currentState) {
	case(ConnectionState::CLIENT_SERVER_DECISION):
	{
		ImGui::Begin("Establish Connection", &showConnectionWindow);

		if (ImGui::Button("Client")) {
			m_currentState = ConnectionState::CLIENT_CONNECTION;
			RakNet::SocketDescriptor sd;
			m_pRakPeer->Startup(1, &sd, 1);

			LogConsoleMessage("Client Startup");
		}

		if (ImGui::Button("Server")) {
			m_currentState = ConnectionState::SERVER_PROCESSING_EVENTS;
			RakNet::SocketDescriptor sd(SERVER_PORT, 0);
			m_pRakPeer->Startup(MAX_CLIENTS, &sd, 1);

			LogConsoleMessage("Server Startup");

			m_pRakPeer->SetMaximumIncomingConnections(MAX_CLIENTS);
		}

		ImGui::End();

		break;
	}
	case(ConnectionState::CLIENT_CONNECTION):
	{
		
		ImGui::Begin("Connect To Server", &showConnectionWindow);
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Enter Server IP");

		static int ipAddress[4] = { 127,0,0,1 };
		ImGui::InputInt4("Server Address", ipAddress);

		if (ImGui::Button("Connect")) {

			LogConsoleMessage("Client Connect Started");

			std::stringstream ss;
			ss << ipAddress[0] << "." << ipAddress[1] << "." << ipAddress[2] <<  "." << ipAddress[3];
			m_pRakPeer->Connect(ss.str().c_str(), SERVER_PORT, 0, 0);
			m_currentState = ConnectionState::CLIENT_WAITING_FOR_CONNECTION;
		}


		ImGui::End();

		break;
	}
	case(ConnectionState::SERVER_PROCESSING_EVENTS): 
	{
		ProcessServerEvents();
		break;
	}
	case(ConnectionState::CLIENT_WAITING_FOR_CONNECTION): 
	{
		ImGui::Begin("Waiting for Connection", &showConnectionWindow);
		//Loading Animation
		ImGui::Text("Waiting for Connection... %c", "|/-\\"[(int)(Utility::getTotalTime() / 0.05f) & 3]);
		
		RakNet::Packet* packet = m_pRakPeer->Receive();
		while (packet != nullptr) {

			switch (packet->data[0])
			{
				case(ID_CONNECTION_REQUEST_ACCEPTED): 
				{
					LogConsoleMessage("Connection Request Accepted");
					break;
				}
				case(CSNetMessages::SERVER_AUTHENTICATE_SUCCESS):
				{
					//Read in byte from packet, ignore the first byte (which is the message id)
					m_serverAddress = packet->systemAddress;
					RakNet::RakString rs;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

					bsIn.Read(rs);
					LogConsoleMessage(rs.C_String());

					m_currentState = ConnectionState::CLIENT_AUTHORISATION;
					break;
				}
				case(ID_NO_FREE_INCOMING_CONNECTIONS):
				{
					ImGui::Text("Server Full");
					m_pRakPeer->CloseConnection(packet->systemAddress, true);
					LogConsoleMessage("Connection Closed :: Server Full");
					m_currentState = ConnectionState::CLIENT_SERVER_DECISION;
					break;
				}
				default:
					break;	
			}

			//Deallocate Packet and get the next packet
			m_pRakPeer->DeallocatePacket(packet);
			packet = m_pRakPeer->Receive();

		}

		ImGui::End();

		break;
	}
	case(ConnectionState::CLIENT_AUTHORISATION):
	{
		static char username[256];
		static char password[256];

		ImGui::Begin("Enter Login Details", &showConnectionWindow);
		ImGui::InputText("Enter Username: ", username, 256);
		ImGui::InputText("Enter Password: ", password, 256);

		if (ImGui::Button("Login")) {
			RakNet::BitStream loginCreds;
			loginCreds.Write((RakNet::MessageID)CSNetMessages::CLIENT_LOGIN_DATA);
			loginCreds.Write(username, 256 * 8);
			loginCreds.Write(password, 256 * 8);
			m_pRakPeer->Send(&loginCreds, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
			LogConsoleMessage("Sending Login Data");
			m_currentState = ConnectionState::CLIENT_WAITING_FOR_AUTHORISATION;
		}

		if (ImGui::Button("Register")) {
			//Send through new username and password
			RakNet::BitStream regCreds;
			regCreds.Write((RakNet::MessageID)CSNetMessages::CLIENT_REGISTER_DATA);
			regCreds.Write(username, 256 * 8);
			regCreds.Write(password, 256 * 8);
			m_pRakPeer->Send(&regCreds, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
			LogConsoleMessage("Sending Register Data");
			m_currentState = ConnectionState::CLIENT_WAITING_FOR_AUTHORISATION;
		}

		ImGui::End();
		break;

	}
	case(ConnectionState::CLIENT_WAITING_FOR_AUTHORISATION):
	{
		ImGui::Begin("Waiting for server to authenticate", &showConnectionWindow);

		RakNet::Packet* packet = m_pRakPeer->Receive();
		ImGui::Text("Waiting for Authentication... %c", "|/-\\"[(int)(Utility::getTotalTime() / 0.05f) & 3]);

		while (packet != nullptr) {

			switch (packet->data[0])
			{
			//TO DO - AUTHENTICATION
			case(CSNetMessages::SERVER_AUTHENTICATE_SUCCESS):
			{
				LogConsoleMessage("Authentication Successfull");
				break;
			}
			case(CSNetMessages::SERVER_AUTHENTICATE_FAIL):
			{
				LogConsoleMessage("Authentication Failed");
				break;
			}
			default:
				break;
			}

			//Deallocate Packet and get the next packet
			m_pRakPeer->DeallocatePacket(packet);
			packet = m_pRakPeer->Receive();
		}

		ImGui::End();
		break;
	}

	default:
		break;

	}
	*/

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

	static bool show_demo_window = true;
	//ImGui::ShowDemoWindow(&show_demo_window);
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


/*
void TestProject::ProcessServerEvents() {

	RakNet::Packet* packet = m_pRakPeer->Receive();

	while (packet != nullptr) {

		switch (packet->data[0])
		{
			case(ID_REMOTE_DISCONNECTION_NOTIFICATION):
			{
				LogConsoleMessage("Another Client has Disconnected");
				break;
			}
			case(ID_REMOTE_CONNECTION_LOST):
			{
				LogConsoleMessage("Another Client has Lost Connection");
				break;
			}
			case(ID_REMOTE_NEW_INCOMING_CONNECTION):
			{
				LogConsoleMessage("A new client is trying to connect");
				break;
			}
			case(ID_CONNECTION_REQUEST_ACCEPTED):
			{
				LogConsoleMessage("Our Connection Request has been accepted");
				break;
			}
			case(ID_NEW_INCOMING_CONNECTION):
			{
				LogConsoleMessage("A Client is attempting to connect");

				RakNet::BitStream connectionAuthorise;
				connectionAuthorise.Write((RakNet::MessageID)CSNetMessages::SERVER_AUTHENTICATE_SUCCESS);
				connectionAuthorise.Write("Identify yourself!");
				m_pRakPeer->Send(&connectionAuthorise, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
				break;
			}
			case(ID_NO_FREE_INCOMING_CONNECTIONS):
			{
				LogConsoleMessage("A Client attempted to connect a full server");
				break;
			}
			case(CSNetMessages::CLIENT_LOGIN_DATA): {

				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				//Ignore Message ID
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				char username[256];
				char password[256];
				bsIn.Read(username, 256);
				bsIn.Read(password, 256);

				LogConsoleMessage("Received Login Info");

				std::string usernameString = std::string(username);
				std::string passwordString = std::string(password);

				RakNet::BitStream authCreds;

				if (usernameString == "a" && passwordString == "a") 
				{
					
					authCreds.Write((RakNet::MessageID)CSNetMessages::SERVER_AUTHENTICATE_SUCCESS);
					m_pRakPeer->Send(&authCreds, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
					LogConsoleMessage("Sending Auth Success");
				}
				else {
					authCreds.Write((RakNet::MessageID)CSNetMessages::SERVER_AUTHENTICATE_FAIL);
					m_pRakPeer->Send(&authCreds, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
					LogConsoleMessage("Sending Auth Fail");
				}

				break;
			}
		default:
			{
				LogConsoleMessage("Unknown Data Received");
				break;
			}
		}

		//Deallocate Packet and get the next packet
		m_pRakPeer->DeallocatePacket(packet);
		packet = m_pRakPeer->Receive();
	}


}*/
