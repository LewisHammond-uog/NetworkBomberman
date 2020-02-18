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
	m_eConnectionState = ConnectionState::CLIENT_CONNECTION;

	//Startup Client
	RakNet::SocketDescriptor sd;
	m_pRakPeer->Startup(1, &sd, 1);
}

void NetworkClient::Update()
{
	DoPreGameConnectionEvents();
}

void NetworkClient::DoPreGameConnectionEvents()
{

	//WINDOW RENDERING
	static bool showConnectionWindow = true;
	//Setup Imgui window size and position
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowSize = ImVec2(400.f, 250.f);
	ImVec2 windowPos = ImVec2(io.DisplaySize.x * 0.5f - windowSize.x * 0.5f, io.DisplaySize.y * 0.5f - windowSize.y * 0.5f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);

	switch (m_eConnectionState) {
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
				ss << ipAddress[0] << "." << ipAddress[1] << "." << ipAddress[2] << "." << ipAddress[3];
				m_pRakPeer->Connect(ss.str().c_str(), SERVER_PORT, 0, 0);
				m_eConnectionState = ConnectionState::CLIENT_WAITING_FOR_CONNECTION;
			}


			ImGui::End();

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

					m_eConnectionState = ConnectionState::CLIENT_AUTHORISATION;
					break;
				}
				case(ID_NO_FREE_INCOMING_CONNECTIONS):
				{
					ImGui::Text("Server Full");
					m_pRakPeer->CloseConnection(packet->systemAddress, true);
					LogConsoleMessage("Connection Closed :: Server Full");
					m_eConnectionState = ConnectionState::CLIENT_SERVER_DECISION;
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
				m_eConnectionState = ConnectionState::CLIENT_WAITING_FOR_AUTHORISATION;
			}

			if (ImGui::Button("Register")) {
				//Send through new username and password
				RakNet::BitStream regCreds;
				regCreds.Write((RakNet::MessageID)CSNetMessages::CLIENT_REGISTER_DATA);
				regCreds.Write(username, 256 * 8);
				regCreds.Write(password, 256 * 8);
				m_pRakPeer->Send(&regCreds, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
				LogConsoleMessage("Sending Register Data");
				m_eConnectionState = ConnectionState::CLIENT_WAITING_FOR_AUTHORISATION;
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
					m_eConnectionState = ConnectionState::START_GAME;
					break;
				}
				case(CSNetMessages::SERVER_AUTHENTICATE_FAIL):
				{
					LogConsoleMessage("Authentication Failed");
					m_eConnectionState = ConnectionState::START_GAME;
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
		case(ConnectionState::START_GAME): {

			//Create a packet and send it
			RakNet::BitStream myStream;
			myStream.Write((RakNet::MessageID)CSNetMessages::CLIENT_TEST_DATA);

			m_pRakPeer->Send(&myStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);

		}
		default:
			break;

	}
}
