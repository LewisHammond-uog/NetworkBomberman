#include "stdafx.h"
#include "PlayerDataComponent.h"


//Project Includes
#include "GameManager.h"
#include "ConsoleLog.h"

//Define Component as our parent
typedef Component PARENT;

/// <summary>
/// Create player data
/// </summary>
/// <param name="a_pOwner">Owner Entity</param>
/// <param name="a_iPlayerID">ID of this Player</param>
PlayerDataComponent::PlayerDataComponent(Entity* a_pOwner, RakNet::RakNetGUID a_iPlayerID) :
	PARENT(a_pOwner),
	m_playerID(a_iPlayerID)
{
	m_eComponentType = COMPONENT_TYPE::PLAYER_DATA;
}

/// <summary>
/// Kills the Player
/// </summary>
void PlayerDataComponent::KillPlayer()
{
	//Destroy this player after this frame
	if (m_pOwnerEntity) {
		GameManager::GetInstance()->DeleteEntityAfterUpdate(m_pOwnerEntity);
	}
}

/// <summary>
/// Get the ID of the player that this component is attached to
/// </summary>
RakNet::RakNetGUID PlayerDataComponent::GetPlayerID() const
{
	return m_playerID;
}

/// <summary>
/// Serialize the constructon of the player data component. Transmitting the GUID
/// that this player is for
/// </summary>
/// <param name="constructionBitstream"></param>
/// <param name="destinationConnection"></param>
void PlayerDataComponent::SerializeConstruction(RakNet::BitStream* constructionBitstream,
	RakNet::Connection_RM3* destinationConnection)
{
	//Call base function
	Component::SerializeConstruction(constructionBitstream, destinationConnection);

	/*
	 * CONSTRUCTION DATA LAYOUT
	 * RakNet::RakNetGUID System that owns this player
	 */
	
	 //Send the RakNet GUID
	constructionBitstream->Write(m_playerID);
}

/// <summary>
/// Deserialise the constructon of the player data component. Setting the GUID that owns this player
/// </summary>
/// <param name="constructionBitstream"></param>
/// <param name="sourceConnection"></param>
/// <returns></returns>
bool PlayerDataComponent::DeserializeConstruction(RakNet::BitStream* constructionBitstream,
	RakNet::Connection_RM3* sourceConnection)
{

	//Call Base Function
	Component::DeserializeConstruction(constructionBitstream, sourceConnection);
	
	/*
	 * CONSTRUCTION DATA LAYOUT
	 * RakNet::RakNetGUID System that owns this player
	 */

	//Read in the player ID
	constructionBitstream->Read(m_playerID);
	
	return true;
}
