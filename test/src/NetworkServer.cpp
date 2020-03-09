#include "stdafx.h"
#include "NetworkServer.h"

//Raknet Includes
#include "BitStream.h"

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
	//Destory rakpeer
	RakNet::RakPeerInterface::DestroyInstance(m_pRakPeer);

	//Destroy Authenticator
	if (m_oServerAuthenticator != nullptr) {
		delete m_oServerAuthenticator;
	}
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
	RakNet::SocketDescriptor sd(SERVER_PORT, 0);
	m_pRakPeer->Startup(MAX_CLIENTS, &sd, 1);
	m_pRakPeer->SetMaximumIncomingConnections(MAX_CLIENTS);
	m_eServerState = NetworkServer::ServerGameStates::SERVER_PROCESSING_EVENTS;

	LogConsoleMessage("SERVER :: Server Initalised");
}

void NetworkServer::Update()
{
	DoPreGameServerEvents();
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
				LogConsoleMessage("SERVER :: A CLIENT HAS DISCONNECTED");
				break;
			}
			case(ID_REMOTE_CONNECTION_LOST):
			{
				LogConsoleMessage("SERVER :: A CLIENT HAS LOST CONNECTION");
				break;
			}
			case(ID_REMOTE_NEW_INCOMING_CONNECTION):
			{
				LogConsoleMessage("SERVER :: NEW INCOMING CONNECTION REQUEST");
				break;
			}
			case(ID_NEW_INCOMING_CONNECTION):
			{
				LogConsoleMessage("SERVER :: NEW INCOMING CONNECTION REQUEST");
				break;
			}
			case(ID_NO_FREE_INCOMING_CONNECTIONS):
			{
				LogConsoleMessage("SERVER :: A CLIENT ATTEMPTED TO CONNECT TO A FULL SERVER");
				break;
			}
			case(CSNetMessages::CLIENT_REGISTER_DATA): {
				//Fall through to login data as it uses (mostly) the same code
				RakNet::BitStream incomingLoginData(packet->data, packet->length, false);

				//Try and authenticate exsiting user, return message to client
				//if this is successfull or not
				if (m_oServerAuthenticator->LoginFromBitstream(incomingLoginData, true))
				{
					LogConsoleMessage("SERVER :: SENDING CLIENT REGISTER SUCCESS INFO");
					//Send success message to client
					SendMessageToClient(packet->systemAddress, CSNetMessages::SERVER_AUTHENTICATE_SUCCESS, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);

					//Add to list of connected clients
					ConnectedClientInfo newClientInfo{
						packet->systemAddress, //Store Sys address
						++connectedClients //Store Client ID
					};
					m_vConnectedClients.push_back(newClientInfo);
				}
				else {

					//Send fail message to client
					SendMessageToClient(packet->systemAddress, CSNetMessages::SERVER_AUTHENTICATE_FAIL, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);
					LogConsoleMessage("SERVER :: SENDING CLIENT REGISTER FAIL INFO");
				}

				break;
			}
			case(CSNetMessages::CLIENT_LOGIN_DATA): {
				
				RakNet::BitStream incomingLoginData(packet->data, packet->length, false);

				//Try and authenticate exsiting user, return message to client
				//if this is successfull or not
				if (m_oServerAuthenticator->LoginFromBitstream(incomingLoginData, false))
				{
					LogConsoleMessage("SERVER :: SENDING CLIENT LOGIN SUCCESS INFO");
					//Send success message to client
					SendMessageToClient(packet->systemAddress, CSNetMessages::SERVER_AUTHENTICATE_SUCCESS, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);

					//Add to list of connected clients
					ConnectedClientInfo newClientInfo{
						packet->systemAddress, //Store Sys address
						++connectedClients //Store Client ID
					};
					m_vConnectedClients.push_back(newClientInfo);
				}
				else {

					//Send fail message to client
					SendMessageToClient(packet->systemAddress, CSNetMessages::SERVER_AUTHENTICATE_FAIL, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);
					LogConsoleMessage("SERVER :: SENDING CLIENT LOGIN FAIL INFO");
				}

				break;
			}
			case(CSNetMessages::CLIENT_READY_TO_PLAY): {
				LogConsoleMessage("SERVER :: A CLIENT IS READY TO PLAY");

				//Add to our ready clients count
				++readyClients;

				//Check if game is ready to start
				if (readyClients >= requiredPlayerCount) {
					
					//Need to make this work so it sends to all clients
					//Create and send packet that game is ready to play
					RakNet::BitStream readyMessage;
					readyMessage.Write((RakNet::MessageID)CSGameMessages::SERVER_GAME_STARTING);
					SendMessageToAllClients(readyMessage, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);

					LogConsoleMessage("SERVER :: GAME STARTING");
				}

				break;
			}
			default:
			{
				LogConsoleMessage("SERVER :: UNKNOWN DATA RECEIVED");
				break;
			}
		}

		//Deallocate Packet and get the next packet
		m_pRakPeer->DeallocatePacket(packet);
		packet = m_pRakPeer->Receive();
	}
}

void NetworkServer::SendMessageToClient(int a_iPlayerID, RakNet::BitStream& a_data, PacketPriority a_priotity, PacketReliability a_reliability)
{
	//Loop through the vector and see if we have a client with the given player
	//id, then get it's address and call the send message function
	for (int i = 0; i < m_vConnectedClients.size(); ++i) {
		if (m_vConnectedClients[i].playerID == a_iPlayerID) {
			RakNet::SystemAddress clientAddress = m_vConnectedClients[i].m_clientAddress;
			SendMessageToClient(clientAddress, a_data, a_priotity, a_reliability);
		}

	}
}

/// <summary>
/// Send a BitStream Message to a client with a given IP address
/// </summary>
/// <param name="a_sClientAddress">Address to send message to</param>
/// <param name="a_data">Bitstream Data to send to</param>
/// <param name="a_priotity">Priotity to Send this message as</param>
/// <param name="a_reliability">Reliability to send this message as</param>
void NetworkServer::SendMessageToClient(RakNet::SystemAddress a_clientAddress, RakNet::BitStream& a_data, PacketPriority a_priotity, PacketReliability a_reliability)
{
	//Send Message over rak peer
	m_pRakPeer->Send(&a_data, a_priotity, a_reliability, 0, a_clientAddress, false);
}

/// <summary>
/// Send just a Net Message to a client and no other data
/// (e.g sending that a client has been successfully authenticated, SERVER_AUTHENTICATE_SUCCESS)
/// </summary>
/// <param name="a_clientAddress"></param>
/// <param name="a_data"></param>
/// <param name="a_priotity"></param>
/// <param name="a_reliability"></param>
void NetworkServer::SendMessageToClient(RakNet::SystemAddress a_clientAddress, RakNet::MessageID a_eMessage, PacketPriority a_priotity, PacketReliability a_reliability)
{
	//Create a bit stream and write
	//the message ID
	RakNet::BitStream messageBitStream;
	messageBitStream.Write((RakNet::MessageID)a_eMessage);

	//Call Send Message Function
	SendMessageToClient(a_clientAddress, messageBitStream, a_priotity, a_reliability);
}

/// <summary>
/// Send a Message to all Clients connected to this server
/// </summary>
/// <param name="a_data">Message to Send</param>
/// <param name="a_priotity">Priotity to Send this message as</param>
/// <param name="a_reliability">Reliability to send this message as</param>
void NetworkServer::SendMessageToAllClients(RakNet::BitStream& a_data, PacketPriority a_priotity, PacketReliability a_reliability)
{
	//Loop though all of the clients and call the send message function
	for (int i = 0; i < m_vConnectedClients.size(); ++i) {
		
		//Get the server address of the client
		RakNet::SystemAddress sClientAddress = m_vConnectedClients[i].m_clientAddress;

		//Call Send Message Function on the selected client
		SendMessageToClient(sClientAddress, a_data, a_priotity, a_reliability);
	}
}