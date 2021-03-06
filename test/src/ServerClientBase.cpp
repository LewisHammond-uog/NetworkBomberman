#include "stdafx.h"
#include "ServerClientBase.h"

//RakNet Includes
#include <RakPeerInterface.h>

//Declare Static Variables
NetworkReplicator* ServerClientBase::s_pReplicaManager = nullptr;
RakNet::NetworkIDManager* ServerClientBase::s_pNetworkIdManager = nullptr;
RakNet::RakPeerInterface* ServerClientBase::s_pRakPeer = nullptr;

//Constructor for Client/Server
ServerClientBase::ServerClientBase()
{
	s_pRakPeer = nullptr; //Set RakPeer to null
	m_serverAddress = RakNet::UNASSIGNED_SYSTEM_ADDRESS; //We are not connected to the server, set no address
}

//Destructor for deleting objects we have created
ServerClientBase::~ServerClientBase()
{
	//Destroy RakPeer
	if (s_pRakPeer != nullptr) {
		RakNet::RakPeerInterface::DestroyInstance(s_pRakPeer);
	}
	
	//Delete NetworkID and Replica Manager
	delete s_pReplicaManager;
	delete s_pNetworkIdManager;
}

/// <summary>
/// Gets the Network ID Manager that the game is using.
///  Creates on if one has not been created yet
/// </summary>
/// <returns>Pointer to the Network ID Manager</returns>
RakNet::NetworkIDManager* ServerClientBase::GetNetworkIDManager()
{
	//If a network manager does not exist then create one
	if(s_pNetworkIdManager == nullptr)
	{
		s_pNetworkIdManager = new RakNet::NetworkIDManager();
	}

	//Return the Network ID Manager, either the one that we have just created
	//or the pre-exsiting one
	return s_pNetworkIdManager;
}

/// <summary>
/// Gets the Network Replicator that the game is using.
/// Creates on if one has not been created yet
/// </summary>
/// <returns>Pointer to the Network Replicator</returns>
NetworkReplicator* ServerClientBase::GetNetworkReplicator()
{
	//Create a network replicator if one does not already exist
	if(s_pReplicaManager == nullptr)
	{
		s_pReplicaManager = new NetworkReplicator();

		//Set the network ID Manager of the Network Replicator
		s_pReplicaManager->SetNetworkIDManager(GetNetworkIDManager());

		//Set the default update time
		s_pReplicaManager->SetAutoSerializeInterval(DEFAULT_SERVER_MILLISECONDS_BETWEEN_UPDATES);
	}

	//Return the Network Replicator, either the one that we have just created
	//or the pre-exsiting one
	return s_pReplicaManager;
}

/// <summary>
/// Gets the GUID, a RakNet unqiue identifier, of this system
/// </summary>
/// <returns>GUID of this system</returns>
RakNet::RakNetGUID ServerClientBase::GetSystemGUID()
{
	//Null Check rakpeer
	if(s_pRakPeer == nullptr)
	{
		return RakNet::RakNetGUID();
	}

	//Return this systems GUID
	return s_pRakPeer->GetMyGUID();
}
