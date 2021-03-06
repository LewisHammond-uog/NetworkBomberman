#ifndef __PLAYER_MANAGER_H__
#define __PLAYER_MANAGER_H__

//C++ Includes
#include <vector>
#include <map>

//Project Includes
#include "NetworkServer.h"
#include "Colour.h"


//Forward Declerations
class Entity;
class PlayerDataComponent;
namespace RakNet {
	struct RakNetGUID;
}

/// <summary>
/// Class for Creating, Destroying and Updating the Properties of players
/// </summary>
class PlayerManager
{
public:

	//Function to Create Player
	void CreatePlayersForAllClients(const std::vector<ConnectedClientInfo>& a_vConnectedClients);
	Entity* CreatePlayer(RakNet::RakNetGUID a_ownerGUID, glm::vec3 a_v3Position);

	//Function to Set Player enabled state
	void SetAllPlayersEnabled(bool a_bEnabled);
	//Function to assign player a colour
	void SetPlayerColour(RakNet::RakNetGUID a_ownerGUID, glm::vec4 a_v4Colour);
	
	//Functions to Destory Player
	void DestroyPlayer(RakNet::RakNetGUID a_playerGUID);
	void DestroyAllPlayers();

	//Getter function for the number of active players
	unsigned GetPlayerCount() const;
	
private:

	//Map of players
	//<Owner GUID, Player Entity*>
	std::map<RakNet::RakNetGUID, Entity*> m_xPlayers;
};

#endif //!__PLAYER_MANAGER_H__