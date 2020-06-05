#include "stdafx.h"
#include "NetworkServer.h"

//Raknet Includes
#include  "RakPeerInterface.h"
#include "BitStream.h"

//Project Includes
#include "ConsoleLog.h"

//TestIncludes
#include "GameManager.h"
#include <NetworkBlackboard.h>

#include "LevelManager.h"

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
	//Get Packets and then proccess them by the current server state
	if (m_eServerState == ServerGameStates::SERVER_CLIENTS_CONNECTING) {
		DoPreGameServerEvents();
	}else if(m_eServerState == ServerGameStates::SERVER_PROCESSING_EVENTS)
	{
		DoGamePlayingServerEvents();
	}
}

void NetworkServer::DeInit()
{
}

/// <summary>
/// Does server events until enough clients are connected to the 
/// server and a ready
/// </summary>
void NetworkServer::DoPreGameServerEvents()
{
	RakNet::Packet* packet = s_pRakPeer->Receive();

	while (packet != nullptr) {

		switch (packet->data[0])
		{
			case(CSNetMessages::CLIENT_REGISTER_DATA): {

				RakNet::BitStream incomingLoginData(packet->data, packet->length, false);

				//Try and authenticate a new user user, return message to client
				//if this is successful or not
				if (m_pServerAuthenticator->LoginFromBitstream(incomingLoginData, true))
				{
					//Send Success
					ConsoleLog::LogMessage("SERVER :: SENDING CLIENT REGISTER SUCCESS INFO");
					SendMessageToClient(packet->systemAddress, CSNetMessages::SERVER_AUTHENTICATE_SUCCESS, PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE);

					//Add to list of connected clients
					ConnectedClientInfo newClientInfo{
						packet->guid, //Store Sys identifier
					};
					m_vConnectedClients.push_back(newClientInfo);
					
				} else {

					//Send fail message to client
					SendMessageToClient(packet->systemAddress, CSNetMessages::SERVER_AUTHENTICATE_FAIL, PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE);
					ConsoleLog::LogMessage("SERVER :: SENDING CLIENT REGISTER FAIL INFO");
				}

				break;
			}
			case(CSNetMessages::CLIENT_LOGIN_DATA): {

				RakNet::BitStream incomingLoginData(packet->data, packet->length, false);

				//Try and authenticate exsiting user, return message to client
				//if this is successfull or not
				if (m_pServerAuthenticator->LoginFromBitstream(incomingLoginData, false))
				{
					ConsoleLog::LogMessage("SERVER :: SENDING CLIENT LOGIN SUCCESS INFO");
					//Send success message to client
					SendMessageToClient(packet->systemAddress, CSNetMessages::SERVER_AUTHENTICATE_SUCCESS, PacketPriority::MEDIUM_PRIORITY, PacketReliability::RELIABLE);

					//Add to list of connected clients
					ConnectedClientInfo newClientInfo{
						packet->guid, //Store Sys addres
					};
					m_vConnectedClients.push_back(newClientInfo);
				}
				else {

					//Send fail message to client
					SendMessageToClient(packet->systemAddress, CSNetMessages::SERVER_AUTHENTICATE_FAIL, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);
					ConsoleLog::LogMessage("SERVER :: SENDING CLIENT LOGIN FAIL INFO");
				}

				break;
			}
			case(CSNetMessages::CLIENT_READY_TO_PLAY): {
				ConsoleLog::LogMessage("SERVER :: A CLIENT IS READY TO PLAY");

				//Add to our ready clients count
				++m_iReadyClients;

				//Check if game is ready to start
				if (m_iReadyClients >= requiredPlayerCount) {

					//Create and send packet that game is ready to play
					RakNet::BitStream readyMessage;
					readyMessage.Write((RakNet::MessageID)CSGameMessages::SERVER_GAME_STARTING);
					SendMessageToAllClients(readyMessage, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);

					//Create Player
					GameManager::GetInstance()->AssignConnectedPlayers(&m_vConnectedClients);
					GameManager::GetInstance()->WarmupGame();
					GameManager::GetInstance()->StartGame();
					
					//Change Server State
					m_eServerState = ServerGameStates::SERVER_PROCESSING_EVENTS;					
					ConsoleLog::LogMessage("SERVER :: GAME STARTING");
				}
				break;
			}
			default:
			{
				//Log out unknown data and it's message ID
				char errorBuffer[ERROR_BUFFER_SIZE];
				sprintf(errorBuffer, "SERVER :: Unknown Data Received in Get Connections Stage. ID: %i", packet->data[0]);
				ConsoleLog::LogMessage(errorBuffer);
				break;
			}
		}

		//Deallocate Packet and get the next packet
		s_pRakPeer->DeallocatePacket(packet);
		packet = s_pRakPeer->Receive();
	}
}

/// <summary>
/// Do events relating to game playing (i.e processing movements, updating game objects)
/// </summary>
void NetworkServer::DoGamePlayingServerEvents()
{
	RakNet::Packet* packet = s_pRakPeer->Receive();

	while (packet != nullptr) {
		switch(packet->data[0])
		{
		//Creation Requests
		case(CSGameMessages::CLIENT_PLAYER_CREATE_BOMB):
		case(CSGameMessages::CLIENT_PLAYER_INPUT_DATA):
			{
				//Send Player Input Data to the Blackboard so that it can be processed by
				//individal players
				RakNet::BitStream incomingInputData(packet->data, packet->length, true);

				NetworkBlackboard::GetInstance()->AddReceivedNetworkData(incomingInputData);
				break;
				
			}
		//Disconnections
		case(ID_CONNECTION_LOST):
		case(ID_DISCONNECTION_NOTIFICATION):
			{
				//Process the fact that this client has disconnected by destroying their player
				//and removing them from our connected client list
				DisconnectClient(packet->guid);
				break;
			}
		default:
			{
				//Log out unknown data and it's message ID
				char errorBuffer[ERROR_BUFFER_SIZE];
				sprintf(errorBuffer, "SERVER :: Unknown Data Received in Play Stage. ID: %i", packet->data[0]);
				ConsoleLog::LogMessage(errorBuffer);
			}
		}

		//Deallocate Packet and get the next packet
		s_pRakPeer->DeallocatePacket(packet);
		packet = s_pRakPeer->Receive();
	}
}

/// <summary>
/// Send a BitStream Message to a client with a given IP address
/// </summary>
/// <param name="a_clientAddress">Address to send message to</param>
/// <param name="a_data">Bitstream Data to send to</param>
/// <param name="a_priority">Priotity to Send this message as</param>
/// <param name="a_reliability">Reliability to send this message as</param>
void NetworkServer::SendMessageToClient(const RakNet::SystemAddress a_clientAddress, RakNet::BitStream& a_data, const PacketPriority a_priority, const PacketReliability a_reliability) const
{
	//Send Message over rak peer
	s_pRakPeer->Send(&a_data, a_priority, a_reliability, 0, a_clientAddress, false);
}


/// <summary>
/// Send just a Net Message to a client and no other data
/// (e.g sending that a client has been successfully authenticated, SERVER_AUTHENTICATE_SUCCESS)
/// </summary>
/// <param name="a_clientAddress">System address to send the data to</param>
/// <param name="a_eMessage">Message to send</param>
/// <param name="a_priority">Message priority</param>
/// <param name="a_reliability">Message reliability</param>
void NetworkServer::SendMessageToClient(const RakNet::SystemAddress a_clientAddress, const RakNet::MessageID a_eMessage, const PacketPriority a_priority, const PacketReliability a_reliability) const
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
void NetworkServer::SendMessageToAllClients(RakNet::BitStream& a_data, const PacketPriority a_priority, const PacketReliability a_reliability)
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
/// Disconnect a client from the server
/// </summary>
/// <param name="a_clientGUID">Client to disconnectg</param>
void NetworkServer::DisconnectClient(const RakNet::RakNetGUID a_clientGUID)
{
	//Destroy the Player Object
	GameManager::GetInstance()->ProcessDisconnection(a_clientGUID);

	//Remove Client from the connected clients list
	for(std::vector<ConnectedClientInfo>::const_iterator clientIt = m_vConnectedClients.begin(); clientIt != m_vConnectedClients.end(); ++clientIt)
	{
		ConnectedClientInfo currentClient = *clientIt;
		if(currentClient.m_clientGUID == a_clientGUID)
		{
			m_vConnectedClients.erase(clientIt);
		}
	}
}
