#include "stdafx.h"
#include "NetworkServer.h"


//Raknet Includes
#include "RakPeerInterface.h"
#include "BitStream.h"

NetworkServer::NetworkServer()
{
	//Null out ptrs and network values
	m_pRakPeer = nullptr;
	m_serverAddress = RakNet::SystemAddress();
}

NetworkServer::~NetworkServer()
{
	//TO DO
	//Destory rakpeer
	RakNet::RakPeerInterface::DestroyInstance(m_pRakPeer);
}

void NetworkServer::Init()
{
	//Get instance of rakPeerInterface and set state
	m_pRakPeer = RakNet::RakPeerInterface::GetInstance();

	//Start up Server
	RakNet::SocketDescriptor sd(SERVER_PORT, 0);
	m_pRakPeer->Startup(MAX_CLIENTS, &sd, 1);
	m_pRakPeer->SetMaximumIncomingConnections(MAX_CLIENTS);
	m_eConnectionState = ServerConnectionState::SERVER_PROCESSING_EVENTS;

	LogConsoleMessage("Server Initalised");
}

void NetworkServer::Update()
{
	DoPreGameServerEvents();
}

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
}
