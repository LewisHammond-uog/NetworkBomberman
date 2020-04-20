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

NetworkServer::NetworkServer()
{
	//Null out ptrs and network values
	m_pRakPeer = nullptr;
	m_serverAddress = RakNet::SystemAddress();

	//Create Authenticator
	m_oServerAuthenticator = new Authenticator();

}

NetworkServer::~NetworkServer()
{
	//Destroy Authenticator
	delete m_oServerAuthenticator;
}

/// <summary>
/// Initalise he server by setting up it's 
/// rak peer and setting it's starting state
/// </summary>
void NetworkServer::Init()
{
	//Get instance of rakPeerInterface and set state
	m_pRakPeer = RakNet::RakPeerInterface::GetInstance();

	//Start up Server
	RakNet::SocketDescriptor sd(SERVER_PORT, nullptr);
	m_pRakPeer->Startup(MAX_CLIENTS, &sd, 1);
	m_pRakPeer->SetMaximumIncomingConnections(MAX_CLIENTS);
	m_eServerState = ServerGameStates::SERVER_CLIENTS_CONNECTING;

	//Attach the network replicator to our Rak Peer
	//so that it runs automatically
	//We get the network replicator from the base class
	//ServerClientBase
	m_pRakPeer->AttachPlugin(GetNetworkReplicator());

	ConsoleLog::LogConsoleMessage("SERVER :: Server initialized");
}

void NetworkServer::Update()
{
	if (m_eServerState == ServerGameStates::SERVER_CLIENTS_CONNECTING) {
		DoPreGameServerEvents();
	}else if(m_eServerState == ServerGameStates::SERVER_PROCESSING_EVENTS)
	{
		DoGamePlayingServerEvents();
	}
}

/// <summary>
/// Does server events until enough clients are connected to the 
/// server and a ready
/// </summary>
void NetworkServer::DoPreGameServerEvents()
{

	RakNet::Packet* packet = m_pRakPeer->Receive();

	while (packet != nullptr) {

		switch (packet->data[0])
		{
			case(ID_REMOTE_DISCONNECTION_NOTIFICATION):
			{
				ConsoleLog::LogConsoleMessage("SERVER :: A CLIENT HAS DISCONNECTED");
				break;
			}
			case(ID_REMOTE_CONNECTION_LOST):
			{
				ConsoleLog::LogConsoleMessage("SERVER :: A CLIENT HAS LOST CONNECTION");
				break;
			}
			case(ID_NEW_INCOMING_CONNECTION):
			{
				ConsoleLog::LogConsoleMessage("SERVER :: NEW INCOMING CONNECTION REQUEST");
				break;
			}
			case(ID_NO_FREE_INCOMING_CONNECTIONS):
			{
				ConsoleLog::LogConsoleMessage("SERVER :: A CLIENT ATTEMPTED TO CONNECT TO A FULL SERVER");
				break;
			}
			case(ID_REPLICA_MANAGER_SCOPE_CHANGE):
			{
				ConsoleLog::LogConsoleMessage("SERVER :: A REPLICA OBJECT HAS CHANGED SCOPE");
				break;
			}
			case(ID_REPLICA_MANAGER_SERIALIZE):
			{
				ConsoleLog::LogConsoleMessage("SERVER :: A REPLICA OBJECT IS BEING SERIALIZED");
				break;
			}
			case(ID_REPLICA_MANAGER_CONSTRUCTION):
			{
				ConsoleLog::LogConsoleMessage("SERVER :: A REPLICA OBJECT IS BEING CONSTRUCTED");
				break;
			}
			case(ID_REPLICA_MANAGER_DOWNLOAD_STARTED):
			{
				ConsoleLog::LogConsoleMessage("SERVER :: A REPLICA OBJECT DOWNLOAD IS STARTING");
				break;
			}
			case(ID_REPLICA_MANAGER_DOWNLOAD_COMPLETE):
			{
				ConsoleLog::LogConsoleMessage("SERVER :: A REPLICA OBJECT DOWNLOAD IS COMPLETE");
				break;
			}
			case(CSNetMessages::CLIENT_REGISTER_DATA): {

				RakNet::BitStream incomingLoginData(packet->data, packet->length, false);

				//Try and authenticate a new user user, return message to client
				//if this is successful or not
				if (m_oServerAuthenticator->LoginFromBitstream(incomingLoginData, true))
				{
					ConsoleLog::LogConsoleMessage("SERVER :: SENDING CLIENT REGISTER SUCCESS INFO");
					//Send success message to client
					SendMessageToClient(packet->systemAddress, CSNetMessages::SERVER_AUTHENTICATE_SUCCESS, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);

					//Add to list of connected clients
					ConnectedClientInfo newClientInfo{
						packet->systemAddress, //Store Sys address
						++m_iConnectedClients //Store Client ID
					};
					m_vConnectedClients.push_back(newClientInfo);
					
				} else {

					//Send fail message to client
					SendMessageToClient(packet->systemAddress, CSNetMessages::SERVER_AUTHENTICATE_FAIL, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);
					ConsoleLog::LogConsoleMessage("SERVER :: SENDING CLIENT REGISTER FAIL INFO");
				}

				break;
			}
			case(CSNetMessages::CLIENT_LOGIN_DATA): {

				RakNet::BitStream incomingLoginData(packet->data, packet->length, false);

				//Try and authenticate exsiting user, return message to client
				//if this is successfull or not
				if (m_oServerAuthenticator->LoginFromBitstream(incomingLoginData, false))
				{
					ConsoleLog::LogConsoleMessage("SERVER :: SENDING CLIENT LOGIN SUCCESS INFO");
					//Send success message to client
					SendMessageToClient(packet->systemAddress, CSNetMessages::SERVER_AUTHENTICATE_SUCCESS, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);

					//Add to list of connected clients
					ConnectedClientInfo newClientInfo{
						packet->systemAddress, //Store Sys address
						++m_iConnectedClients //Store Client ID
					};
					m_vConnectedClients.push_back(newClientInfo);


				}
				else {

					//Send fail message to client
					SendMessageToClient(packet->systemAddress, CSNetMessages::SERVER_AUTHENTICATE_FAIL, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);
					ConsoleLog::LogConsoleMessage("SERVER :: SENDING CLIENT LOGIN FAIL INFO");
				}

				break;
			}
			case(CSNetMessages::CLIENT_READY_TO_PLAY): {
				ConsoleLog::LogConsoleMessage("SERVER :: A CLIENT IS READY TO PLAY");

				//Add to our ready clients count
				++m_iReadyClients;

				//Check if game is ready to start
				if (m_iReadyClients >= requiredPlayerCount) {

					//Create and send packet that game is ready to play
					RakNet::BitStream readyMessage;
					readyMessage.Write((RakNet::MessageID)CSGameMessages::SERVER_GAME_STARTING);
					SendMessageToAllClients(readyMessage, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);

					//Change Server State
					m_eServerState = ServerGameStates::SERVER_PROCESSING_EVENTS;

					//todo move?
					//Create Players
					GameManager::CreatePlayers(1);
					
					ConsoleLog::LogConsoleMessage("SERVER :: GAME STARTING");
				}

				break;
			}
			default:
			{
				//Log out unknown data and it's message ID
				char errorBuffer[128];
				sprintf(errorBuffer, "SERVER :: Unknown Data Received in Get Connections Stage ID: %i", packet->data[0]);
				ConsoleLog::LogConsoleMessage(errorBuffer);
				break;
			}
		}

		//Deallocate Packet and get the next packet
		m_pRakPeer->DeallocatePacket(packet);
		packet = m_pRakPeer->Receive();
	}
}

/// <summary>
/// Do events relating to game playing (i.e processing movements, updating game objects)
/// </summary>
void NetworkServer::DoGamePlayingServerEvents() const
{
	RakNet::Packet* packet = m_pRakPeer->Receive();

	while (packet != nullptr) {
		switch(packet->data[0])
		{
		case(CSGameMessages::CLIENT_PLAYER_CREATE_BOMB):
		case(CSGameMessages::CLIENT_PLAYER_INPUT_DATA):
			{
				//Send Player Input Data to the Blackboard so that it can be processed by
				//individal players
				RakNet::BitStream incomingInputData(packet->data, packet->length, true);

				NetworkBlackboard::GetInstance()->AddReceivedNetworkData(incomingInputData);
				break;
				
			}
		default:
			{
				//Log out unknown data and it's message ID
				char errorBuffer[128];
				sprintf(errorBuffer, "SERVER :: Unknown Data Received in Play Stage ID: %i", packet->data[0]);
				ConsoleLog::LogConsoleMessage(errorBuffer);
			}
		}

		//Deallocate Packet and get the next packet
		m_pRakPeer->DeallocatePacket(packet);
		packet = m_pRakPeer->Receive();
	}
}


void NetworkServer::SendMessageToClient(int a_iClientID, RakNet::BitStream& a_data, PacketPriority a_priority, PacketReliability a_reliability)
{
	//Loop through the vector and see if we have a client with the given player
	//id, then get it's address and call the send message function
	for (unsigned int i = 0; i < m_vConnectedClients.size(); ++i) {
		if (m_vConnectedClients[i].m_playerId == a_iClientID) {
			const RakNet::SystemAddress clientAddress = m_vConnectedClients[i].m_clientAddress;
			SendMessageToClient(clientAddress, a_data, a_priority, a_reliability);
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
void NetworkServer::SendMessageToClient(const RakNet::SystemAddress a_clientAddress, RakNet::BitStream& a_data, const PacketPriority a_priority, const PacketReliability a_reliability) const
{
	//Send Message over rak peer
	m_pRakPeer->Send(&a_data, a_priority, a_reliability, 0, a_clientAddress, false);
}


/// <summary>
/// Send just a Net Message to a client and no other data
/// (e.g sending that a client has been successfully authenticated, SERVER_AUTHENTICATE_SUCCESS)
/// </summary>
/// <param name="a_clientAddress">System address to send the data to</param>
/// <param name="a_eMessage">Message to send</param>
/// <param name="a_priority">Message priority</param>
/// <param name="a_reliability">Message reliability</param>
void NetworkServer::SendMessageToClient(const RakNet::SystemAddress a_clientAddress, const RakNet::MessageID a_eMessage, const PacketPriority a_priority, const PacketReliability a_reliability)
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
		const RakNet::SystemAddress sClientAddress = m_vConnectedClients[i].m_clientAddress;

		//Call Send Message Function on the selected client
		SendMessageToClient(sClientAddress, a_data, a_priority, a_reliability);
	}
}