#include "stdafx.h"
#include "NetworkServer.h"

//Raknet Includes
#include  "RakPeerInterface.h"
#include "BitStream.h"

//Project Includes
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
void NetworkServer::Init()
{
	//Get instance of rakPeerInterface and set state
	s_pRakPeer = RakNet::RakPeerInterface::GetInstance();

	//Start up Server
	RakNet::SocketDescriptor sd(SERVER_PORT, nullptr);
	s_pRakPeer->Startup(MAX_CLIENTS, &sd, 1);
	s_pRakPeer->SetMaximumIncomingConnections(MAX_CLIENTS);
	m_eServerState = ServerGameStates::SERVER_CLIENTS_CONNECTING;

	//Attach the network replicator to our Rak Peer
	//so that it runs automatically
	//We get the network replicator from the base class
	//ServerClientBase
	s_pRakPeer->AttachPlugin(GetNetworkReplicator());

	ConsoleLog::LogMessage("SERVER :: Server initialized");
}

void NetworkServer::Update()
{
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
				SendMessageToClient(a_pPacket->systemAddress, CSNetMessages::SERVER_AUTHENTICATE_SUCCESS, PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE);

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
				SendMessageToClient(a_pPacket->systemAddress, CSNetMessages::SERVER_AUTHENTICATE_SUCCESS, PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE);

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
			if (m_iReadyClients >= m_iRequiredPlayerCount) {

				//Create and send packet that game is warming up
				RakNet::BitStream readyMessage;
				readyMessage.Write((RakNet::MessageID)CSGameMessages::SERVER_GAME_WARMUP);
				SendMessageToAllClients(readyMessage, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);

				//Start Game Warmup
				//Create Players and level
				GameManager::GetInstance()->AssignConnectedPlayers(&m_vConnectedClients);
				GameManager::GetInstance()->WarmupGame();
				
				//Change Server State
				m_eServerState = ServerGameStates::SERVER_GAME_WARMUP;					
				ConsoleLog::LogMessage("SERVER :: WARMING UP GAME");
			}
			break;
		}
		default:
		{
			//Log out unknown data and it's message ID
			char errorBuffer[ERROR_BUFFER_SIZE];
			sprintf(errorBuffer, "SERVER :: Unknown Data Received in Get Connections Stage. ID: %i", a_pPacket->data[0]);
			ConsoleLog::LogMessage(errorBuffer);
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
			//Log out unknown data and it's message ID
			char errorBuffer[ERROR_BUFFER_SIZE];
			sprintf(errorBuffer, "SERVER :: Unknown Data Received in Play Stage. ID: %i", a_pPacket->data[0]);
			ConsoleLog::LogMessage(errorBuffer);
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
	s_pRakPeer->CloseConnection(a_clientGUID, true, ORDERING_CHANNEL_CONNECTIONS);
	
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
