#include "stdafx.h"
#include "NetworkBlackboard.h"

//Project Includes
#include "ServerClientBase.h"
#include "NetworkClient.h"

//Initalise static instance
NetworkBlackboard* NetworkBlackboard::m_pInstance = nullptr;


/// <summary>
/// Destructor of blackboard
/// Removes all unread messages
/// </summary>
NetworkBlackboard::~NetworkBlackboard()
{
	//Delete all of the unread messages
	std::vector<NetworkData*>::const_iterator xIter;
	for (xIter = m_vUnreadMessages.begin(); xIter < m_vUnreadMessages.end(); ++xIter)
	{
		delete* xIter;
	}
}

/// <summary>
/// Get the instance of the NetworkBlackboard singleton
/// </summary>
/// <returns></returns>
NetworkBlackboard* NetworkBlackboard::GetInstance()
{
	if(m_pInstance == nullptr)
	{
		m_pInstance = new NetworkBlackboard();
	}
	return m_pInstance;
}


/// <summary>
/// Get the network data for a given message type, for a given player
/// </summary>
/// <param name="a_dataType">Type of Data to Get</param>
/// <param name="a_iPlayerID">ID of the Player to get the data for</param>
/// <returns>List of data for the given</returns>
std::vector<NetworkData*> NetworkBlackboard::GetNetworkData(RakNet::MessageID a_dataType, const RakNet::RakNetGUID a_iPlayerID)
{
	std::vector<NetworkData*> vReturnData;
	
	//Loop through all of the data we have and check for the data
	//type and player ID
	std::vector<NetworkData*>::const_iterator xIter;
	for(xIter = m_vUnreadMessages.begin(); xIter < m_vUnreadMessages.end();)
	{
		//Check that the current data is valid
		NetworkData* currentData = *xIter;
		if (currentData == nullptr) { return vReturnData; }

		//Check for data type, add it to the return type if it is correct
		if(currentData->m_dataType == a_dataType && currentData->m_iPlayerID == a_iPlayerID)
		{
			vReturnData.push_back(currentData);

			//Remove this from unread messages and set the Iterator to know about this
			xIter = m_vUnreadMessages.erase(xIter);
			continue;
		}

		//Data was not of correct format, just increment the iterator
		++xIter;
	}

	//Return the data we have collected, this maybe no data of the type
	//we were looking for
	return vReturnData;
}

/// <summary>
/// Add received network data to the blackboard
/// </summary>
/// <param name="a_data">Raw Data (with type and player ID)</param>
void NetworkBlackboard::AddReceivedNetworkData(RakNet::BitStream& a_data)
{
	//Strip off the data type and player ID then
	//create a struct of the infomation and the remaining data
	RakNet::MessageID dataType = 0;
	RakNet::RakNetGUID iPlayerID;
	RakNet::BitStream remainingData;

	a_data.Read(dataType);
	a_data.Read(iPlayerID);
	a_data.Read(remainingData);
	
	NetworkData* netData = new NetworkData();
	netData->m_dataType = dataType;
	netData->m_iPlayerID = iPlayerID;
	netData->m_data.Write(remainingData);
	  
	//Call other function to add it to the vector
	AddReceivedNetworkData(netData);
}

/// <summary>
/// Add received network data to the blackboard
/// </summary>
/// <param name="a_pBlackboardData">Data to add</param>
void NetworkBlackboard::AddReceivedNetworkData(NetworkData* a_pBlackboardData)
{
	m_vUnreadMessages.push_back(a_pBlackboardData);
}

/// <summary>
/// Set the Network Client for the blackboard
/// </summary>
/// <param name="a_networkClient"></param>
void NetworkBlackboard::SetNetworkClient(NetworkClient* a_networkClient)
{
	if(a_networkClient != nullptr)
	{
		m_netClient = a_networkClient;
	}
}

/// <summary>
/// Send network data to the sever
/// </summary>
/// <param name="a_dataType">Type of data to send</param>
/// <param name="a_data">Data (without type and player ID)</param>
void NetworkBlackboard::SendBlackboardDataToServer(const RakNet::MessageID a_dataType, RakNet::BitStream& a_data)
{
	//Get our system ID which will be this players ID so that the server knows who has sent that data
	RakNet::RakNetGUID ourPlayerID = ServerClientBase::GetSystemGUID();
	
	//Encode data in to a bitstream to send
	RakNet::BitStream bsToSend;
	bsToSend.Write((RakNet::MessageID)a_dataType);
	bsToSend.Write(ourPlayerID);
	bsToSend.Write(a_data);

	if (m_netClient) {
		m_netClient->SendMessageToServer(bsToSend, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE);
	}
}



