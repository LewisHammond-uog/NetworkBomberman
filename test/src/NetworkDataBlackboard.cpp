#include "stdafx.h"
#include "NetworkDataBlackboard.h"

//Project Includes
#include "ServerClientBase.h"
#include "NetworkClient.h"
#include "TestProject.h"

//Initalise static instance
NetworkDataBlackboard* NetworkDataBlackboard::m_pInstance = nullptr;

/// <summary>
/// Get the instance of the NetworkDataBlackboard singleton
/// </summary>
/// <returns></returns>
NetworkDataBlackboard* NetworkDataBlackboard::GetInstance()
{
	if(m_pInstance == nullptr)
	{
		m_pInstance = new NetworkDataBlackboard();
	}
	return m_pInstance;
}

/// <summary>
/// Get the input data we have received, and not yet processed, from a given player ID
/// </summary>
/// <param name="a_iPlayerID">Player ID to get input data for</param>
/// <returns>Vector of unprocessed inputs from given Player ID</returns>
std::vector<PlayerInputNetworkData*> NetworkDataBlackboard::GetPlayerInputNetworkData(const int a_iPlayerID)
{
	//Create a vector
	std::vector<PlayerInputNetworkData*> vInputDataVector;
	
	//Only check for inputs from clients if we are the server because we should not receive this data as
	//a client
	if (TestProject::isServer)
	{
		//Loop through all of the data and get a vector of all the data for the specified
		//player ID
		std::vector<PlayerInputNetworkData*>::const_iterator xIter;
		for (xIter = m_vUnreadInputMessages.begin(); xIter < m_vUnreadInputMessages.end();)
		{
			PlayerInputNetworkData* pCurrentMessage = *xIter;
			//If the player ID of the unread message matches the requested player id
			//add it to the vector we are going to return and remove it from the unread vector
			if (pCurrentMessage != nullptr) {
				if (pCurrentMessage->iPlayerID == a_iPlayerID)
				{
					//Add to vector to return
					vInputDataVector.push_back(pCurrentMessage);

					//Remove this from unread messages and set the Iterator to know about this
					xIter = m_vUnreadInputMessages.erase(xIter);
					continue;
				}
			}

			//We didn't remove anything so increment the Iterator
			++xIter;
		}
	}

	//return the vector of inputs - this maybe empty if there are no inputs for the player, or
	//we are the client
	return vInputDataVector;
}

/// <summary>
/// Add raw received input data to the blackboard for us to process within the players
/// </summary>
/// <param name="a_data">Raw (with message ID) data to procces</param>
void NetworkDataBlackboard::AddReceivedNetworkData(RakNet::BitStream& a_data)
{
	//Check that we are the server as the client should never proccess this
	if(!TestProject::isServer)
	{
		return;
	}

	//Check that we have received the correct message type
	RakNet::MessageID messageType;
	a_data.Read(messageType);
	if(messageType != CSGameMessages::CLIENT_PLAYER_INPUT_DATA)
	{
		return;
	}

	//Create input data struct and read data in
	//PlayerID
	//Movement Inputs
	PlayerInputNetworkData* pReceivedData = new PlayerInputNetworkData();
	a_data.Read(pReceivedData->iPlayerID);
	a_data.Read(pReceivedData->v2MovementInputs);
	m_vUnreadInputMessages.push_back(pReceivedData);
	
}

/// <summary>
/// Send Player Input Data to the server
/// </summary>
/// <param name="a_data">Data to send</param>
void NetworkDataBlackboard::SendPlayerInputNetworkData(PlayerInputNetworkData* a_data)
{
	//Make sure we are the client - server will never send this
	if(TestProject::isServer)
	{
		return;
	}

	//Create a bitstream with the correct message id, add the data and
	//send it to the server
	RakNet::BitStream bsToSend;
	bsToSend.Write((RakNet::MessageID)CSGameMessages::CLIENT_PLAYER_INPUT_DATA);
	bsToSend.Write(a_data->iPlayerID);
	bsToSend.Write(a_data->v2MovementInputs);

	NetworkClient::instance->SendMessageToServer(bsToSend,PacketPriority::IMMEDIATE_PRIORITY, PacketReliability::RELIABLE);
}
