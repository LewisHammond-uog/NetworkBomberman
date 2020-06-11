#include "stdafx.h"
#include "NetworkServer.h"

#include <utility>

//Raknet Includes
#include  "RakPeerInterface.h"
#include "BitStream.h"

//Project Includes
#include "ConnectionUI.h"
#include "ConsoleLog.h"
#include "GameManager.h"
#include "NetworkBlackboard.h"
#include "Entity.h"
#include "NetworkOrderingChannels.h"
#include "Timer.h"

constexpr int ERROR_BUFFER_SIZE = 128;

NetworkServer::NetworkServer()
{
	//Null out ptrs and network values
	s_pRakPeer = nullptr;
	m_serverAddress = RakNet::SystemAddress();

	//Create Authenticator
	m_pServerAuthenticator = new Authenticator();
}

NetworkServer::~NetworkServer()
{
	//Destroy Authenticator
	delete m_pServerAuthenticator;
}

/// <summary>
/// Initalise he server by setting up it's 
/// rak peer and setting it's starting state
/// </summary>
void NetworkServer::Init(int a_iMaxPlayerCount, int a_iMinReadyPlayers, float a_fWarmupTime, std::vector<std::string> a_vsSelectedLevels)
{
	//Get instance of rakPeerInterface and set state
	s_pRakPeer = RakNet::RakPeerInterface::GetInstance();

	//Start up Server
	RakNet::SocketDescriptor sd(SERVER_PORT, nullptr);
	s_pRakPeer->Startup(a_iMaxPlayerCount, &sd, 1);
	s_pRakPeer->SetMaximumIncomingConnections(a_iMaxPlayerCount);
	m_eServerState = ServerGameStates::SERVER_CLIENTS_CONNECTING;

	//Set Server Settings
	m_iMinReadyPlayers = a_iMinReadyPlayers;
	m_fWarmupDuration = a_fWarmupTime;
	GameManager::GetInstance()->SetLevelRotation(std::move(a_vsSelectedLevels));

	//Attach the network replicator to our Rak Peer
	//so that it runs automatically
	//We get the network replicator from the base class
	//ServerClientBase
	s_pRakPeer->AttachPlugin(GetNetworkReplicator());

	ConsoleLog::LogMessage("SERVER :: Server initialized");
}

void NetworkServer::Update()
{
	//If the server is not initalised then wait until it is
	if (m_eServerState == ServerGameStates::SERVER_NOT_INIT){
		
		//Statics to pass to server settings UI so we can receive the user
		//inputted values
		static int iMaxPlayers = mc_iDefaultMaxPlayers;
		static int iMinReadyPlayers = mc_iDefaultMinReadyPlayers;
		static float fWarmupTime = mc_fDefaultWarmupTime;
		static std::vector<std::string> vsLevelRotation;

		//Wait for the user to press the server start button
		if (ConnectionUI::DrawServerSettingsUI(iMaxPlayers, iMinReadyPlayers, fWarmupTime, vsLevelRotation))
		{
			Init(iMaxPlayers, iMinReadyPlayers, fWarmupTime, vsLevelRotation);
		}
	}

	//Check that RakPeer is valid 
	if(s_pRakPeer == nullptr)
	{
		return;
	}
	
	//Get a received packets this frame
	RakNet::Packet* pPacket = s_pRakPeer->Receive();

	//Proccess the packets using different methods
	//(i.e disconnection, game playing packets)
	while(pPacket != nullptr)
	{
		//Process Game Packets
		switch(m_eServerState) {
		case ServerGameStates::SERVER_CLIENTS_CONNECTING: {
			HandlePreGamePackets(pPacket);
			break;
		}
		case ServerGameStates::SERVER_GAME_PLAYING: {
			HandelPlayingGamePackets(pPacket);
			break;
		}
		default:
			break;
		}

		//Process Disconnections
		HandleDisconnectPackets(pPacket);

		//Deallocate Packet and get the next a_pPacket
		s_pRakPeer->DeallocatePacket(pPacket);
		pPacket = s_pRakPeer->Receive();		
	}

	//Do Server events that are not dependent on packets
	if(m_eServerState == ServerGameStates::SERVER_GAME_WARMUP)
	{
		DoGameWarmupServerEvents();
	}else if(m_eServerState == ServerGameStates::SERVER_GAME_PLAYING)
	{
		DoGamePlayingEvents();
	}
}

/// <summary>
/// Deinitalise the Server, disconnecting all clients
/// </summary>
void NetworkServer::DeInit()
{
	//Disconnect all clients
	if(!m_vConnectedClients.empty())
	{
		for(int i = 0; i < m_vConnectedClients.size(); ++i)
		{
			DisconnectClient(m_vConnectedClients[i].m_clientGUID);
		}
	}

	if (s_pRakPeer) {
		//Shut down the rakpeer
		s_pRakPeer->Shutdown(0);
	}
	ConsoleLog::LogMessage("SERVER :: SHUTTING DOWN");
}

/// <summary>
/// Does server events until enough clients are connected to the 
/// server and a ready
/// </summary>
void NetworkServer::HandlePreGamePackets(RakNet::Packet* a_pPacket)
{
	//Do events based on the data we have received
	switch (a_pPacket->data[0])
	{
		case(CSNetMessages::CLIENT_REGISTER_DATA): {

			RakNet::BitStream incomingLoginData(a_pPacket->data, a_pPacket->length, false);

			//Try and authenticate a new user user, return message to client
			//if this is successful or not
			if (m_pServerAuthenticator->LoginFromBitstream(incomingLoginData, true))
			{
				//Send Success
				ConsoleLog::LogMessage("SERVER :: SENDING CLIENT REGISTER SUCCESS INFO");
				SendMessageToClient(a_pPacket->systemAddress, CSNetMessages::SERVER_AUTHENTICATE_SUCCESS_REG, PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE);

				//Add to list of connected clients
				const ConnectedClientInfo newClientInfo{
					a_pPacket->guid, //Store Sys identifier
				};
				m_vConnectedClients.push_back(newClientInfo);
				
			} else {

				//Send fail message to client
				SendMessageToClient(a_pPacket->systemAddress, CSNetMessages::SERVER_AUTHENTICATE_FAIL, PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE);
				ConsoleLog::LogMessage("SERVER :: SENDING CLIENT REGISTER FAIL INFO");
			}

			break;
		}
		case(CSNetMessages::CLIENT_LOGIN_DATA): {

			RakNet::BitStream incomingLoginData(a_pPacket->data, a_pPacket->length, false);

			//Try and authenticate exsiting user, return message to client
			//if this is successfull or not
			if (m_pServerAuthenticator->LoginFromBitstream(incomingLoginData, false))
			{
				ConsoleLog::LogMessage("SERVER :: SENDING CLIENT LOGIN SUCCESS INFO");
				//Send success message to client
				SendMessageToClient(a_pPacket->systemAddress, CSNetMessages::SERVER_AUTHENTICATE_SUCCESS_LOGIN, PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE);

				//Add to list of connected clients
				const ConnectedClientInfo newClientInfo{
					a_pPacket->guid, //Store Sys addres
				};
				m_vConnectedClients.push_back(newClientInfo);
			}
			else {

				//Send fail message to client
				SendMessageToClient(a_pPacket->systemAddress, CSNetMessages::SERVER_AUTHENTICATE_FAIL, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);
				ConsoleLog::LogMessage("SERVER :: SENDING CLIENT LOGIN FAIL INFO");
			}

			break;
		}
		case(CSNetMessages::CLIENT_READY_TO_PLAY): {
			ConsoleLog::LogMessage("SERVER :: A CLIENT IS READY TO PLAY");

			//Add to our ready clients count
			++m_iReadyClients;

			//Check if game is ready to start, do warmup actions
			if (m_iReadyClients >= m_iMinReadyPlayers) {

				//Create and send packet that game is warming up
				RakNet::BitStream readyMessage;
				readyMessage.Write((RakNet::MessageID)CSGameMessages::SERVER_GAME_WARMUP);
				SendMessageToAllClients(readyMessage, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);

				//Start Game Warmup
				//Create Players and level
				GameManager::GetInstance()->AssignConnectedPlayers(&m_vConnectedClients);
				GameManager::GetInstance()->WarmupGame();

				//Reset Ready clients
				m_iReadyClients = 0;
				
				//Change Server State
				m_eServerState = ServerGameStates::SERVER_GAME_WARMUP;					
				ConsoleLog::LogMessage("SERVER :: WARMING UP GAME");
			}
			break;
		}
		default:
		{
			//Do Nothing
			break;
		}
	}
	
}

/// <summary>
/// Does the events of warming up the game
/// </summary>
void NetworkServer::DoGameWarmupServerEvents()
{
	//Create a static timer to use for tracking how long the warmup is
	static Timer timer;

	//Check if the timer is enabled if it is not then warmup has not started
	//and we should enable it
	if(!timer.IsActive())
	{
		timer.Start();
	}

	//Check if the timer elapsed time is more than the warmup time
	//then end the warmup phase
	if(timer.GetElapsedS() >= m_fWarmupDuration)
	{
		//Set state to game start and tell the players that the game is starting
		m_eServerState = ServerGameStates::SERVER_GAME_PLAYING;
		RakNet::BitStream readyMessage;
		readyMessage.Write((RakNet::MessageID)CSGameMessages::SERVER_GAME_START);
		SendMessageToAllClients(readyMessage, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);

		//Start the game in the game manager
		GameManager::GetInstance()->StartGame();
		
		//Stop the timer
		timer.Stop();
	}
}

/// <summary>
/// Does events for when the game is playing
/// </summary>
void NetworkServer::DoGamePlayingEvents()
{
	//Keep checking for if the game is over if it is then send this to clients
	if(GameManager::GetInstance()->GetGameState() == GAME_STATE::GAME_STATE_ENDED)
	{
		//Send Message of the game finishing
		RakNet::BitStream finishedMessage;
		finishedMessage.Write((RakNet::MessageID)CSGameMessages::SERVER_GAME_OVER);
		SendMessageToAllClients(finishedMessage, PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE);

		//Change the server state to be back at connecting events
		m_eServerState = ServerGameStates::SERVER_CLIENTS_CONNECTING;
	}
}

/// <summary>
/// Do events relating to game playing (i.e processing movements, updating game objects)
/// </summary>
void NetworkServer::HandelPlayingGamePackets(RakNet::Packet* a_pPacket)
{
	//Do events based on the data we have received
	switch(a_pPacket->data[0])
	{
	//Creation Requests
	case(CSGameMessages::CLIENT_PLAYER_CREATE_BOMB):
	case(CSGameMessages::CLIENT_PLAYER_INPUT_DATA):
		{
			//Send Player Input Data to the Blackboard so that it can be processed by
			//individal players
			RakNet::BitStream incomingInputData(a_pPacket->data, a_pPacket->length, true);
			NetworkBlackboard::GetInstance()->AddReceivedNetworkData(incomingInputData);
			break;
			
		}
	default:
		{
			//Do Nothing
			break;
		}
	}
}

/// <summary>
/// Send a BitStream Message to a client with a given IP address
/// </summary>
/// <param name="a_clientAddress">Address to send message to</param>
/// <param name="a_data">Bitstream Data to send to</param>
/// <param name="a_priority">Priotity to Send this message as</param>
/// <param name="a_reliability">Reliability to send this message as</param>
/// <param name="a_orderingChannel">Ordering Channel to Use, defaults to general</param>
void NetworkServer::SendMessageToClient(const RakNet::SystemAddress a_clientAddress, RakNet::BitStream& a_data, const PacketPriority a_priority, const PacketReliability a_reliability, const ORDERING_CHANNELS a_orderingChannel) const
{
	//Send Message over rak peer
	s_pRakPeer->Send(&a_data, a_priority, a_reliability, ORDERING_CHANNEL_GENERAL, a_clientAddress, false);
}


/// <summary>
/// Send just a Net Message to a client and no other data
/// (e.g sending that a client has been successfully authenticated, SERVER_AUTHENTICATE_SUCCESS)
/// </summary>
/// <param name="a_clientAddress">System address to send the data to</param>
/// <param name="a_eMessage">Message to send</param>
/// <param name="a_priority">Message priority</param>
/// <param name="a_reliability">Message reliability</param>
/// <param name="a_orderingChannel">Ordering Channel to Use, defaults to general</param>
void NetworkServer::SendMessageToClient(const RakNet::SystemAddress a_clientAddress, const RakNet::MessageID a_eMessage, const PacketPriority a_priority, const PacketReliability a_reliability, const ORDERING_CHANNELS a_orderingChannel) const
{
	//Create a bit stream and write
	//the message ID
	RakNet::BitStream messageBitStream;
	messageBitStream.Write((RakNet::MessageID)a_eMessage);

	//Call Send Message Function
	SendMessageToClient(a_clientAddress, messageBitStream, a_priority, a_reliability);
}

/// <summary>
/// Send a Message to all Clients connected to this server
/// </summary>
/// <param name="a_data">Message to Send</param>
/// <param name="a_priority">Priotity to Send this message as</param>
/// <param name="a_reliability">Reliability to send this message as</param>
/// <param name="a_orderingChannel">Ordering Channel to Use, defaults to general</param>
void NetworkServer::SendMessageToAllClients(RakNet::BitStream& a_data, const PacketPriority a_priority, const PacketReliability a_reliability, const ORDERING_CHANNELS a_orderingChannel)
{
	//Loop though all of the clients and call the send message function
	for (unsigned int i = 0; i < m_vConnectedClients.size(); ++i) {
		
		//Get the server address of the client
		const RakNet::SystemAddress sClientAddress = s_pRakPeer->GetSystemAddressFromGuid(m_vConnectedClients[i].m_clientGUID);

		//Call Send Message Function on the selected client
		SendMessageToClient(sClientAddress, a_data, a_priority, a_reliability);
	}
}

/// <summary>
/// Handles Disconnection Packets regardless of play mode
/// </summary>
/// <param name="a_pPacket">Packet</param>
/// <returns>If a_pPacket was a disconnection a_pPacket</returns>
bool NetworkServer::HandleDisconnectPackets(RakNet::Packet* a_pPacket)
{
	//Get the a_pPacket type
	const RakNet::MessageID ePacketType = a_pPacket->data[0];

	//Check if a_pPacket is a disconnection a_pPacket
	if(ePacketType == ID_CONNECTION_LOST || ePacketType == ID_DISCONNECTION_NOTIFICATION)
	{
		//Deal with disconnection
		DisconnectClient(a_pPacket->guid);

		//Reduce the number of players that are ready incase we are in
		//waiting stage
		if(m_iReadyClients > 0)
		{
			--m_iReadyClients;
		}
		
		return true;
	}

	//Packet not dealt with by this function
	return false;
}

/// <summary>
/// Disconnect a client from the server
/// </summary>
/// <param name="a_clientGUID">Client to disconnectg</param>
void NetworkServer::DisconnectClient(const RakNet::RakNetGUID a_clientGUID)
{
	//Close the connection
	if (s_pRakPeer) {
		s_pRakPeer->CloseConnection(a_clientGUID, true, ORDERING_CHANNEL_CONNECTIONS);
	}
	//Destroy the Player Object
	GameManager::GetInstance()->ProcessDisconnection(a_clientGUID);

	//Remove Client from the connected clients list
	if (!m_vConnectedClients.empty()) {
		for (std::vector<ConnectedClientInfo>::const_iterator clientIt = m_vConnectedClients.begin(); clientIt != m_vConnectedClients.end();)
		{
			ConnectedClientInfo currentClient = *clientIt;
			if (currentClient.m_clientGUID == a_clientGUID)
			{
				clientIt = m_vConnectedClients.erase(clientIt);
			}else
			{
				++clientIt;
			}
		}
	}
}
