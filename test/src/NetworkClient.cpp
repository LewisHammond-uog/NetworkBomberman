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
	//Proccess Received Packets
	RakNet::Packet* pPacket = s_pRakPeer->Receive();
	while (pPacket != nullptr)
	{
		HandleClientConnectionPackets(pPacket);
		HandleClientPreGamePackets(pPacket);
		HandleClientGamePackets(pPacket);
		HandleDisconnectPackets(pPacket);

		s_pRakPeer->DeallocatePacket(pPacket);
		pPacket = s_pRakPeer->Receive();
	}
	
	//Run functions to draw connection UI etc. to the screen,
	//these functions can also send data to the server (i.e when
	//sending login data)
	switch (m_eClientGameState) {
		case(ClientLocalState::NOT_CONNECTED): {
			DoClientConnectionEvents();
			break;
		}
		case(ClientLocalState::PRE_GAME): {
			DoClientPreGameEvents();
			break;
		}
		case(ClientLocalState::GAME_OVER):{
			DoClientPostGameEvents();
			break;
		}
		default:
			//Do nothing
			break;
	}	
}



/// <summary>
/// Deinititilises the Client, disconnecting it from the server if we are connected to one
/// </summary>
void NetworkClient::DeInit()
{
	if (s_pRakPeer) {
		//Shut down the rakpeer
		s_pRakPeer->Shutdown(0);
	}
	
	//Disconnect from the server
	DisconnectFromServer();
}




/// <summary>
/// An update loop to get the client connected to the server
/// </summary>
void NetworkClient::DoClientConnectionEvents()
{	
	//Setup window to display login details
	static bool showConnectionWindow = true;

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
			ConnectionUI::DrawWaitingUI("Waiting For Connection");
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
			bool bLogin = false;
			bool bRegister = false;

			ConnectionUI::DrawLoginUI(cUsername, cPassword, bLogin, bRegister);

			//If a button has been pressed
			if(bRegister || bLogin)
			{
				//Set if we are going to write login or registration credits
				const RakNet::MessageID iMessageID = bLogin ? CLIENT_LOGIN_DATA : CLIENT_REGISTER_DATA;

				//Write a bit stream of our login data
				RakNet::BitStream loginCredentials;
				loginCredentials.Write(iMessageID);
				loginCredentials.Write(cUsername);
				loginCredentials.Write(cPassword);

				//Send to the server
				SendMessageToServer(loginCredentials, PacketPriority::LOW_PRIORITY, PacketReliability::RELIABLE_ORDERED, ORDERING_CHANNEL_LOGIN);

				//Change state to waiting for auth
				m_eConnectionState = ClientConnectionState::CLIENT_WAITING_FOR_AUTHORISATION;

				ConsoleLog::LogMessage("CLIENT :: SENDING LOGIN DETAILS TO THE SERVER");
			}
			break;

		}
		case(ClientConnectionState::CLIENT_WAITING_FOR_AUTHORISATION):
		{

			/*
			Wait here for the server to send back whether the details we sent were correct,
			if they were then we are connected to the server and can move on to actually getting game data
			*/
			ConnectionUI::DrawWaitingUI("Waiting for Authentication from the Server");
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

	switch (m_eConnectionState) {
		case(ClientConnectionState::CLIENT_FIRST_TIME_INFO):{
			//Show first time playing info for new players, progress to register after we have acknolaged
			if (ConnectionUI::DrawAcknowledgeUI("First Time Playing!", "Welcome to the game! This is your first time playing! To move the player use WASD to place a bomb press E"))
			{
				m_eConnectionState = ClientConnectionState::CLIENT_INIT_PREGAME;
			}
			break;
		}
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
			break;
		}
		case(ClientConnectionState::CLIENT_WAITING_FOR_READY_PLAYERS): {

			/*
			Wait for the server to tell us the game is starting
			*/
			//Show waiting for players
			ConnectionUI::DrawWaitingUI("Waiting for enough players to be ready...");	
			break;
		}
		case(ClientConnectionState::CLIENT_WARMUP):
			{
				//Show waiting for players
				ConnectionUI::DrawWaitingUI("Game is Starting, Standby...");
			}
		default:
			break;
	}


	//Show Disconnect UI so we can disconnect in this process
	if(ConnectionUI::DrawClientDisconnectUI())
	{
		DisconnectFromServer();
	}
}

/// <summary>
/// Do post game events
/// </summary>
void NetworkClient::DoClientPostGameEvents()
{

	bool bContinue = false;
	bool bDisconnect = false;

	//Draw Game UI and pass by vars by reference so we know when to disconnect
	ConnectionUI::DrawGameOverUI(bContinue, bDisconnect);

	//If we have chosen to disconnect then disconnect from the server
	if(bDisconnect)
	{
		DisconnectFromServer();

		//Set State to connect to a new server
		m_eConnectionState = ClientConnectionState::CLIENT_START_CONNECTION;
		m_eClientGameState = ClientLocalState::NOT_CONNECTED;
	}

	//If we have chosen to continue then set our state to waiting for the player to be ready
	if(bContinue)
	{
		m_eConnectionState = ClientConnectionState::CLIENT_INIT_PREGAME;
		m_eClientGameState = ClientLocalState::PRE_GAME;
	}
}


/// <summary>
/// Handles the packets for getting our intial connection to the server
/// </summary>
/// <param name="a_pPacket">Packet to handle</param>
void NetworkClient::HandleClientConnectionPackets(RakNet::Packet* a_pPacket)
{
	if(a_pPacket != nullptr)
	{
		//If we are waiting for connection check if we have received a response
		if (m_eConnectionState == ClientConnectionState::CLIENT_WAITING_FOR_CONNECTION)
		{
			switch (a_pPacket->data[0])
			{
			case(ID_CONNECTION_REQUEST_ACCEPTED):
			{
				//Server has accepted our requrest
				ConsoleLog::LogMessage("CLIENT :: CLIENT SUCCESSFULLY CONNECTED TO THE SERVER");
				m_eConnectionState = ClientConnectionState::CLIENT_ENTER_AUTH_DETAILS;

				//We have successfully connected to the server store it's ip to send all further
				//packet to
				m_serverAddress = a_pPacket->systemAddress;
				break;
			}
			case(ID_CONNECTION_ATTEMPT_FAILED):
			{
				//Set state to connection failed
				m_eConnectionState = ClientConnectionState::CLIENT_FAILED_CONNECTION;
				break;
			}
			case(ID_NO_FREE_INCOMING_CONNECTIONS):
			{
				//Server is full - reset to new connection window
				s_pRakPeer->CloseConnection(a_pPacket->systemAddress, true);
				ConsoleLog::LogMessage("CLIENT :: CLIENT CANNOT CONNECT TO A FULL SERVER");
				m_eConnectionState = ClientConnectionState::CLIENT_START_CONNECTION;
				break;
			}
			default:
				break;
			}
		}

		//If we are waiting for auth results then proccess them
		if(m_eConnectionState == ClientConnectionState::CLIENT_WAITING_FOR_AUTHORISATION)
		{
			switch (a_pPacket->data[0])
			{
				case(CSNetMessages::SERVER_AUTHENTICATE_SUCCESS_REG):
				{
					ConsoleLog::LogMessage("CLIENT :: LOGIN SUCCESS");

					//We have successfully connected and registered for the server
					//show the player 1st time info
					m_eClientGameState = ClientLocalState::PRE_GAME;
					m_eConnectionState = ClientConnectionState::CLIENT_FIRST_TIME_INFO;

					break;
				}
				case(CSNetMessages::SERVER_AUTHENTICATE_SUCCESS_LOGIN):
					//We have successfully connected and logged in to the server,
					//move on to actually doing game stuff
					m_eClientGameState = ClientLocalState::PRE_GAME;
					m_eConnectionState = ClientConnectionState::CLIENT_INIT_PREGAME;
					break;
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
		}
	}
}

/// <summary>
/// Handles the packets for begining the game
/// </summary>
/// <param name="a_pPacket">Packet to handle</param>
void NetworkClient::HandleClientPreGamePackets(RakNet::Packet* a_pPacket)
{
	if (a_pPacket != nullptr)
	{
		//If we are INIT_PREGAME but have not pressed ready or we
		//have pressed ready and are WAITING_FOR_PLAYERS
		//and we receive the packet that the gam is starting then force us to the game warmup stage,
		//forcing us to be ready
		if(m_eConnectionState == ClientConnectionState::CLIENT_INIT_PREGAME || 
			m_eConnectionState == ClientConnectionState::CLIENT_WAITING_FOR_READY_PLAYERS)
		{
			if (a_pPacket->data[0] == SERVER_GAME_WARMUP) {
				m_eConnectionState = ClientConnectionState::CLIENT_WARMUP;
			}
		}

		//If we are warming up and receive game start messages then start the game
		if(m_eConnectionState == ClientConnectionState::CLIENT_WARMUP)
		{
			//Check if we have the game start message
			if (a_pPacket->data[0] == CSGameMessages::SERVER_GAME_START) {
				ConsoleLog::LogMessage("CLIENT :: RECEIVED GAME START MESSAGES");

				//Change state to game player
				m_eClientGameState = ClientLocalState::GAME_PLAYING;
			}
		}

		
	}
}


/// <summary>
/// Handle Packets while the game is playing, mostly to detect
/// when the game is over
/// </summary>
/// <param name="a_pPacket"></param>
void NetworkClient::HandleClientGamePackets(RakNet::Packet* a_pPacket)
{
	if(a_pPacket != nullptr)
	{
		//Check if the game is over
		if(a_pPacket->data[0] == SERVER_GAME_OVER)
		{
			//Change Client state
			m_eClientGameState = ClientLocalState::GAME_OVER;
		}
	}
}

/// <summary>
/// Handle disconnection packets sent from the server
/// </summary>
/// <param name="a_pPacket"></param>
void NetworkClient::HandleDisconnectPackets(RakNet::Packet* a_pPacket)
{
	//Get the a_pPacket type
	const RakNet::MessageID ePacketType = a_pPacket->data[0];

	//Check if a_pPacket is a disconnection a_pPacket
	if (ePacketType == ID_CONNECTION_LOST || ePacketType == ID_DISCONNECTION_NOTIFICATION)
	{
		//Deal with disconnection
		DisconnectFromServer();
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
	if (s_pRakPeer) {
		s_pRakPeer->CloseConnection(m_serverAddress, true, ORDERING_CHANNEL_CONNECTIONS, LOW_PRIORITY);
	}

	//Set state
	m_eConnectionState = ClientConnectionState::CLIENT_START_CONNECTION;
	m_eClientGameState = ClientLocalState::NOT_CONNECTED;
	
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
