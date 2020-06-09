#include "stdafx.h"
#include "NetworkClient.h"

//Raknet Includes
#include <BitStream.h>
#include <RakPeerInterface.h>

//Project Incldues
#include "Authenticator.h"
#include "ConnectionUI.h"
#include "NetworkBlackboard.h"
#include "ServerCreatedObject.h"
#include "ConsoleLog.h"
#include "NetworkOrderingChannels.h"

//Default Client Constructor
NetworkClient::NetworkClient() : ServerClientBase()
{
	
}

NetworkClient::~NetworkClient()
{
}

/// <summary>
/// Initalise the network client
/// </summary>
void NetworkClient::Init() {

	//Get instance of rakPeerInterface and set state
	s_pRakPeer = RakNet::RakPeerInterface::GetInstance();
	m_eConnectionState = NetworkClient::ClientConnectionState::CLIENT_START_CONNECTION;
	m_eClientGameState = NetworkClient::ClientLocalState::NOT_CONNECTED;

	//Startup Client
	RakNet::SocketDescriptor sd;
	s_pRakPeer->Startup(1, &sd, 1);

	//Set Server Address to undefined
	m_serverAddress = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
	
	//Attach the network replicator to our Rak Peer
	//so that it runs automatically
	//We get the network replicator from the base class
	//ServerClientBase
	s_pRakPeer->AttachPlugin(GetNetworkReplicator());

	//Set our network client for the blackboard
	NetworkBlackboard::GetInstance()->SetNetworkClient(this);
}

/// <summary>
/// Update the network client
/// </summary>
void NetworkClient::Update()
{
	//Choose what functions to run based on the current client state
	switch (m_eClientGameState) {
		case(ClientLocalState::NOT_CONNECTED): {
			DoClientConnectionEvents();
			break;
		}
		case(ClientLocalState::PRE_GAME): {
			DoClientPreGameEvents();
			break;
		}
		case(ClientLocalState::GAME_PLAYING): {
			DoClientGameEvents();
			break;
		}
		default: {
			m_eClientGameState = ClientLocalState::NOT_CONNECTED;
		}
	}
}



/// <summary>
/// Deinititilises the Client, disconnecting it from the server if we are connected to one
/// </summary>
void NetworkClient::DeInit()
{
	//Disconnect from the server
	DisconnectFromServer();
}

/// <summary>
/// Intalises the ImGUI window for client connection
/// </summary>
void NetworkClient::InitImguiWindow()
{
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 windowSize = ImVec2(400.f, 250.f);
	ImVec2 windowPos = ImVec2(io.DisplaySize.x * 0.5f - windowSize.x * 0.5f, io.DisplaySize.y * 0.5f - windowSize.y * 0.5f);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(windowSize, ImGuiCond_FirstUseEver);
}


/// <summary>
/// An update loop to get the client connected to the server
/// </summary>
void NetworkClient::DoClientConnectionEvents()
{	
	//Setup window to display login details
	static bool showConnectionWindow = true;
	InitImguiWindow();

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

				ConsoleLog::LogMessage("CLIENT :: CLIENT CONNECTION STARTED");

				//Get user inputed IP address and try and connect to the server
				std::stringstream ss;
				ss << ipAddress[0] << "." << ipAddress[1] << "." << ipAddress[2] << "." << ipAddress[3];
				ConnectToServer(ss.str().c_str());
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
			//Wait for a packet to be recieved
			RakNet::Packet* packet = s_pRakPeer->Receive();

			ConnectionUI::DrawWaitingUI("Waiting For Connection");

			//While we still have packets to proccess keep processing them
			while (packet != nullptr) {
				switch (packet->data[0])
				{
					case(ID_CONNECTION_REQUEST_ACCEPTED):
					{
						//Server has accepted our requrest
						ConsoleLog::LogMessage("CLIENT :: CLIENT SUCCESSFULLY CONNECTED TO THE SERVER");
						m_eConnectionState = ClientConnectionState::CLIENT_ENTER_AUTH_DETAILS;

						//We have successfully connected to the server store it's ip to send all further
						//packet to
						m_serverAddress = packet->systemAddress;
						break;
					}
					case(ID_CONNECTION_ATTEMPT_FAILED):
						//Set state to connection failed
						m_eConnectionState = ClientConnectionState::CLIENT_FAILED_CONNECTION;
						break;;
					case(ID_NO_FREE_INCOMING_CONNECTIONS):
					{
						//Server is full - reset to new connection window
						s_pRakPeer->CloseConnection(packet->systemAddress, true);
						ConsoleLog::LogMessage("CLIENT :: CLIENT CANNOT CONNECT TO A FULL SERVER");
						m_eConnectionState = ClientConnectionState::CLIENT_START_CONNECTION;
						break;
					}
					default:
						break;
					}

				//Deallocate Packet and get the next packet
				s_pRakPeer->DeallocatePacket(packet);
				packet = s_pRakPeer->Receive();

			}
			break;
		}
		case(ClientConnectionState::CLIENT_FAILED_CONNECTION):
			//We have failed connection show a window
			if(ConnectionUI::DrawAcknowledgeUI("Connection Failed", "Connection has timed out"))
			{
				//If the acknolage UI button has been pressed then return to the connection screen
				m_eConnectionState = ClientConnectionState::CLIENT_START_CONNECTION;
			}
			break;
		case(ClientConnectionState::CLIENT_ENTER_AUTH_DETAILS):
		{
			/*
			Wait here for the user to input login/registration details and then
			send them to the server
			*/

			//Vars to store details
			static char cUsername[Authenticator::mc_iMaxUsernameLen];
			static char cPassword[Authenticator::mc_iMaxPasswordLen];

			ImGui::Begin("Enter Login Details", &showConnectionWindow);
			ImGui::InputText("Enter Username: ", cUsername, Authenticator::mc_iMaxUsernameLen);
			ImGui::InputText("Enter Password: ", cPassword, Authenticator::mc_iMaxPasswordLen);

			//Send Login Details and move to waiting for authorization
			if (ImGui::Button("Login")) {
				RakNet::BitStream loginCreds;
				loginCreds.Write((RakNet::MessageID)CSNetMessages::CLIENT_LOGIN_DATA);
				loginCreds.Write(cUsername, Authenticator::mc_iMaxUsernameLen * sizeof(char));
				loginCreds.Write(cPassword, Authenticator::mc_iMaxPasswordLen * sizeof(char));
				SendMessageToServer(loginCreds, PacketPriority::LOW_PRIORITY, PacketReliability::RELIABLE_ORDERED, ORDERING_CHANNEL_LOGIN);
				
				m_eConnectionState = ClientConnectionState::CLIENT_WAITING_FOR_AUTHORISATION;

				ConsoleLog::LogMessage("CLIENT :: SENDING LOGIN DETAILS TO THE SERVER");
			}

			//Send Registation Details and move to waiting for authorization
			if (ImGui::Button("Register")) {
				//Send through new username and password
				RakNet::BitStream regCreds;
				regCreds.Write((RakNet::MessageID)CSNetMessages::CLIENT_REGISTER_DATA);
				regCreds.Write(cUsername, Authenticator::mc_iMaxUsernameLen * sizeof(char));
				regCreds.Write(cPassword, Authenticator::mc_iMaxPasswordLen * sizeof(char));
				SendMessageToServer(regCreds, PacketPriority::LOW_PRIORITY, PacketReliability::RELIABLE_ORDERED, ORDERING_CHANNEL_LOGIN);

				m_eConnectionState = ClientConnectionState::CLIENT_WAITING_FOR_AUTHORISATION;

				ConsoleLog::LogMessage("CLIENT :: SENDING REGISTRATION DETAILS TO THE SERVER");
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
			ConnectionUI::DrawWaitingUI("Waiting for Authentication from the Server");
				
			//Wait for message of login.fail success
			RakNet::Packet* packet = s_pRakPeer->Receive();
			while (packet != nullptr) {

				switch (packet->data[0])
				{
					case(CSNetMessages::SERVER_AUTHENTICATE_SUCCESS):
					{
						ConsoleLog::LogMessage("CLIENT :: LOGIN SUCCESS");

						//We have successfully connected and logged in to the server,
						//move on to actually doing game stuff
						m_eClientGameState = ClientLocalState::PRE_GAME;
						m_eConnectionState = ClientConnectionState::CLIENT_INIT_PREGAME;

						break;
					}
					case(CSNetMessages::SERVER_AUTHENTICATE_FAIL):
					{
						ConsoleLog::LogMessage("CLIENT :: LOGIN FAILED");
						//Change state to auth failed
						m_eConnectionState = ClientConnectionState::CLIENT_FAILED_AUTHORISATION;
						break;
					}
					default:
						break;
					}

				//Deallocate Packet and get the next packet
				s_pRakPeer->DeallocatePacket(packet);
				packet = s_pRakPeer->Receive();
			}

			break;
		}
		case ClientConnectionState::CLIENT_FAILED_AUTHORISATION:
			{
				//Show Failed Authorisiation, if we have pressed the button then return to the login
				if(ConnectionUI::DrawAcknowledgeUI("Connection Failed", "Login or Registration Details are incorrect", "Retry"))
				{
					m_eConnectionState = ClientConnectionState::CLIENT_ENTER_AUTH_DETAILS;
				}
				break;
			}
		default:
			break;
	}
}

/// <summary>
/// An update loop to get the client ready for the game (i.e downloading level,
/// telling the server we are ready)
/// </summary>
void NetworkClient::DoClientPreGameEvents()
{
	//Window 
	static bool showConnectionWindow = true;
	InitImguiWindow();

	switch (m_eConnectionState) {
		case(ClientConnectionState::CLIENT_INIT_PREGAME): {
				
			//Show a Ready UI and if we have pressed then send the ready message
			if (ConnectionUI::DrawAcknowledgeUI("Waiting for Ready", "Press when Ready to Play", "READY")){
				/*
				Tell the server that this client is ready to start the game
				*/
				SendMessageToServer(CSNetMessages::CLIENT_READY_TO_PLAY, PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE);
				//Set state to wait for the game to start
				m_eConnectionState = ClientConnectionState::CLIENT_WAITING_FOR_READY_PLAYERS;
				ConsoleLog::LogMessage("CLIENT :: SENT SERVER READY MESSAGE");
			}

			//Wait for a packet to be recieved
			RakNet::Packet* packet = s_pRakPeer->Receive();
			while (packet != nullptr) {
				//If the packet type is that we are warming the game up then force the player to be 'ready' and skip to game start
				if (packet->data[0] == SERVER_GAME_WARMUP) {
					m_eConnectionState = ClientConnectionState::CLIENT_WARMUP;
		
				}
				//Deallocate Packet and get the next packet
				s_pRakPeer->DeallocatePacket(packet);
				packet = s_pRakPeer->Receive();
			}
			break;
				
		}
		case(ClientConnectionState::CLIENT_WAITING_FOR_READY_PLAYERS): {

			/*
			Wait for the server to tell us the game is starting
			*/
			//Show waiting for players
			ConnectionUI::DrawWaitingUI("Waiting for enough players to be ready...");

			//Wait for a packet to be recieved
			RakNet::Packet* packet = s_pRakPeer->Receive();
				
			//Wait for us to receive the game warmup message
			while (packet != nullptr) {
				
				//Check if we have the game start message
				if (packet->data[0] == CSGameMessages::SERVER_GAME_WARMUP) {
					ConsoleLog::LogMessage("CLIENT :: RECEIVED GAME WARMUP MESSAGES");

					//Change state to game warmup
					m_eConnectionState = ClientConnectionState::CLIENT_WARMUP;
				}
				//Deallocate Packet and get the next packet
				s_pRakPeer->DeallocatePacket(packet);
				packet = s_pRakPeer->Receive();
			}
			
			break;
		}
		case(ClientConnectionState::CLIENT_WARMUP):
			{
				//Show waiting for players
				ConnectionUI::DrawWaitingUI("Game is Starting, Standby...");
				
				//Wait for a packet to be recieved
				RakNet::Packet* packet = s_pRakPeer->Receive();

				//Wait for us to receive the game start message
				while (packet != nullptr) {

					//Check if we have the game start message
					if (packet->data[0] == CSGameMessages::SERVER_GAME_START) {
						ConsoleLog::LogMessage("CLIENT :: RECEIVED GAME START MESSAGES");

						//Change state to game warmup
						m_eClientGameState = ClientLocalState::GAME_PLAYING;
					}
					//Deallocate Packet and get the next packet
					s_pRakPeer->DeallocatePacket(packet);
					packet = s_pRakPeer->Receive();
				}
			}
		default:
			break;
	}

}

/// <summary>
/// Procceses packes received during gameplay
/// </summary>
void NetworkClient::DoClientGameEvents()
{
	//Wait for a packet to be recieved
	RakNet::Packet* packet = s_pRakPeer->Receive();
	//Wait for us to receive the game start message
	while (packet != nullptr) {
		//Do Nothing Yet

		//Deallocate Packet and get the next packet
		s_pRakPeer->DeallocatePacket(packet);
		packet = s_pRakPeer->Receive();
	}
}

/// <summary>
/// Connect to a server
/// </summary>
/// <param name="a_zcIPAddress"></param>
void NetworkClient::ConnectToServer(const char* a_zcIPAddress) const
{
	//Check we are not already connected to a server
	if(m_serverAddress != RakNet::UNASSIGNED_SYSTEM_ADDRESS)
	{
		return;
	}

	//Send connection request,
	//we are sending no password data or a public key nor a connectionSocket Index this the (nullptr, 0u, nullptr, 0)
	s_pRakPeer->Connect(a_zcIPAddress, SERVER_PORT, nullptr, 0u, nullptr, 0,
		mc_iConnectionAttemptLimit, mc_iTimeBetweenConnectionAttempts, mc_fConnectionTimeout);
}

/// <summary>
/// Disconnect from the server that we are connected to
/// </summary>
void NetworkClient::DisconnectFromServer()
{
	//Send message to server that we are disconnecting
	s_pRakPeer->CloseConnection(m_serverAddress, true, ORDERING_CHANNEL_CONNECTIONS, LOW_PRIORITY);
	m_serverAddress = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
}

/// <summary>
/// Send a bit stream message to the server with a given priority and reliability
/// </summary>
/// <param name="a_data">Bitstream of data to send, must include message id</param>
/// <param name="a_priority">Priority of the message to send</param>
/// <param name="a_reliability">Reliability of the message to send</param>
/// <param name="a_orderingChannel">Ordering Channel to Use, defaults to general</param>
void NetworkClient::SendMessageToServer(RakNet::BitStream& a_data, const PacketPriority a_priority,
                                        const PacketReliability a_reliability, const ORDERING_CHANNELS a_orderingChannel) const
{
	s_pRakPeer->Send(&a_data, a_priority, a_reliability, a_orderingChannel, m_serverAddress, false);
}

/// <summary>
/// Send a notification message that it just the Message ID to the server with a given priority
/// and reliability
/// </summary>
/// <param name="a_eMessage">Message ID to send</param>
/// <param name="a_priority">Priority of the message to send</param>
/// <param name="a_reliability">Reliability of the message to send</param>
/// <param name="a_orderingChannel">Ordering Channel to Use, defaults to general</param>
void NetworkClient::SendMessageToServer(const RakNet::MessageID a_eMessage, const PacketPriority a_priority,
                                        const PacketReliability a_reliability, const ORDERING_CHANNELS a_orderingChannel) const
{
	//Create bitstream and write the message ID to it
	RakNet::BitStream messageStream;
	messageStream.Write((RakNet::MessageID)a_eMessage);

	//Send Message using other SendMessageToServer function
	SendMessageToServer(messageStream, a_priority, a_reliability, a_orderingChannel);
}
