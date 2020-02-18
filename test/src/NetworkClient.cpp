#include "stdafx.h"
#include "NetworkClient.h"

//Raknet Includes
#include <RakPeerInterface.h>
#include <BitStream.h>

//Default Client Contstructor
NetworkClient::NetworkClient()
{
	//TODO JUST CALL PARENT EVENT
	//Null out rakpeer
	m_pRakPeer = nullptr;
	m_serverAddress = RakNet::SystemAddress();
}

NetworkClient::~NetworkClient()
{
	//TODO JUST CALL PARENT EVENT
	//Destory rakpeer
	RakNet::RakPeerInterface::DestroyInstance(m_pRakPeer);
}

void NetworkClient::Init() {

	//Get instance of rakPeerInterface and set state
	m_pRakPeer = RakNet::RakPeerInterface::GetInstance();
	m_eConnectionState = ClientConnectionState::CLIENT_START_CONNECTION;

	//Startup Client
	RakNet::SocketDescriptor sd;
	m_pRakPeer->Startup(1, &sd, 1);
}

void NetworkClient::Update()
{
	//If we haven't yet made it through to being authenticated with the server
	//then keep trying to connect
	if (m_eConnectionState <= CLIENT_WAITING_FOR_AUTHORISATION) {
		DoServerConnectionEvents();
	}
}

/// <summary>
/// An update loop to get the client connected to the server
/// </summary>
void NetworkClient::DoServerConnectionEvents()
{

	//Setup window to display login details
	static bool showConnectionWindow = true;
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowSize = ImVec2(400.f, 250.f);
	ImVec2 windowPos = ImVec2(io.DisplaySize.x * 0.5f - windowSize.x * 0.5f, io.DisplaySize.y * 0.5f - windowSize.y * 0.5f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);

	switch (m_eConnectionState) {
		case(ClientConnectionState::CLIENT_START_CONNECTION):
		{
			/*
			State where the UI is waiting for the user to input the server
			ip address
			*/

			//Window Render
			ImGui::Begin("Connect To Server", &showConnectionWindow);
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Enter Server IP");

			//Fill in with Defualt 
			static int ipAddress[4] = { 127,0,0,1 };
			ImGui::InputInt4("Server Address", ipAddress);

			if (ImGui::Button("Connect")) {

				LogConsoleMessage("CLIENT :: CLIENT CONNECTION STARTED");

				//Get user inputed IP address and try and connect to the server
				std::stringstream ss;
				ss << ipAddress[0] << "." << ipAddress[1] << "." << ipAddress[2] << "." << ipAddress[3];
				m_pRakPeer->Connect(ss.str().c_str(), SERVER_PORT, 0, 0);
				m_eConnectionState = ClientConnectionState::CLIENT_WAITING_FOR_CONNECTION;
			}


			ImGui::End();

			break;
		}
		case(ClientConnectionState::CLIENT_WAITING_FOR_CONNECTION):
		{
			/*
			Wait here until the server has confirmed that we have connected or
			that our connection has been rejected
			*/

			//Draw Imgui Window
			ImGui::Begin("Waiting for Connection", &showConnectionWindow);
			ImGui::Text("Waiting for Connection... %c", "|/-\\"[(int)(Utility::getTotalTime() / 0.05f) & 3]);

			//Wait for a packet to be recieved
			RakNet::Packet* packet = m_pRakPeer->Receive();

			//While we still have packets to proccess keep processing them
			while (packet != nullptr) {

				switch (packet->data[0])
				{
					case(ID_CONNECTION_REQUEST_ACCEPTED):
					{
						//Server has accepted our requrest
						LogConsoleMessage("CLIENT :: CLIENT SUCCESSFULLY CONNECTED TO THE SERVER");
						m_eConnectionState = ClientConnectionState::CLIENT_ENTER_AUTH_DETAILS;

						//We have successfully connected to the server store it's ip to send all further
						//packet to
						m_serverAddress = packet->systemAddress;

						break;
					}
					case(ID_NO_FREE_INCOMING_CONNECTIONS):
					{
						//Server is full - reset to new connection window
						ImGui::Text("Server Full");
						m_pRakPeer->CloseConnection(packet->systemAddress, true);
						LogConsoleMessage("CLIENT :: CLIENT CANNOT CONNECT TO A FULL SERVER");
						m_eConnectionState = ClientConnectionState::CLIENT_START_CONNECTION;
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
		case(ClientConnectionState::CLIENT_ENTER_AUTH_DETAILS):
		{
			/*
			Wait here for the user to input login/registration details and then
			send them to the server
			*/

			//Vars to store details
			static char username[256];
			static char password[256];

			ImGui::Begin("Enter Login Details", &showConnectionWindow);
			ImGui::InputText("Enter Username: ", username, 256);
			ImGui::InputText("Enter Password: ", password, 256);

			//Send Login Details and move to waiting for authorization
			if (ImGui::Button("Login")) {
				RakNet::BitStream loginCreds;
				loginCreds.Write((RakNet::MessageID)CSNetMessages::CLIENT_LOGIN_DATA);
				loginCreds.Write(username, 256 * 8);
				loginCreds.Write(password, 256 * 8);
				m_pRakPeer->Send(&loginCreds, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
				m_eConnectionState = ClientConnectionState::CLIENT_WAITING_FOR_AUTHORISATION;

				LogConsoleMessage("CLIENT :: SENDING LOGIN DETAILS TO THE SERVER");
			}

			//Send Registation Details and move to waiting for authorization
			if (ImGui::Button("Register")) {
				//Send through new username and password
				RakNet::BitStream regCreds;
				regCreds.Write((RakNet::MessageID)CSNetMessages::CLIENT_REGISTER_DATA);
				regCreds.Write(username, 256 * 8);
				regCreds.Write(password, 256 * 8);
				m_pRakPeer->Send(&regCreds, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
				m_eConnectionState = ClientConnectionState::CLIENT_WAITING_FOR_AUTHORISATION;

				LogConsoleMessage("CLIENT :: SENDING REGISTARTION DETAILS TO THE SERVER");
			}

			ImGui::End();
			break;

		}
		case(ClientConnectionState::CLIENT_WAITING_FOR_AUTHORISATION):
		{

			/*
			Wait here for the server to send back whether the details we sent were correct,
			if they were then we are connected to the server and can move on to actually getting game data
			*/

			ImGui::Begin("Waiting for server to authenticate", &showConnectionWindow);
			ImGui::Text("Waiting for Authentication... %c", "|/-\\"[(int)(Utility::getTotalTime() / 0.05f) & 3]);

			//Wait for message of login.fail success
			RakNet::Packet* packet = m_pRakPeer->Receive();
			while (packet != nullptr) {

				switch (packet->data[0])
				{
					//TO DO - AUTHENTICATION
					case(CSNetMessages::SERVER_AUTHENTICATE_SUCCESS):
					{
						LogConsoleMessage("CLIENT :: LOGIN SUCCESS");

						//We have successfully connected and logged in to the server,
						//move on to actually doing game stuff

						break;
					}
					case(CSNetMessages::SERVER_AUTHENTICATE_FAIL):
					{
						LogConsoleMessage("CLIENT :: LOGIN FAILED");
						//Reset to enter new login details
						m_eConnectionState = ClientConnectionState::CLIENT_ENTER_AUTH_DETAILS;
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
}
