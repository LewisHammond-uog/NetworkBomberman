#include "stdafx.h"
#include "PlayerDataComponent.h"

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
}
/// <summary>
/// Get the ID of the player that this component is attached to
/// </summary>
RakNet::RakNetGUID PlayerDataComponent::GetPlayerID() const
{
	return m_playerID;
}