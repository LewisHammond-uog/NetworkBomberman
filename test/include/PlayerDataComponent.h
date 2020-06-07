#ifndef __PLAYER_DATA_COMPONENT_H__
#define __PLAYER_DATA_COMPONENT_H__

//Project Includes
#include "Colour.h"
#include "Component.h"

/// <summary>
/// Class for data that is used by the player
/// (i.e playerID, health etc.)
/// </summary>
class PlayerDataComponent : public Component
{
public:
	explicit PlayerDataComponent(Entity* a_pOwner, RakNet::RakNetGUID a_pPlayerID);
	~PlayerDataComponent() = default;

	//Update/Draw
	void Update(float a_fDeltaTime) override {};
	void Draw(Shader* a_pShader) override {};

	//Death Function
	void KillPlayer();

	//Colour Set Function
	void SetPlayerColour(Colour a_v4Colour);
	
	RakNet::RakNetGUID GetPlayerID() const;

#pragma region RakNet Functions
	RakNet::RakString GetName(void) const override { return RakNet::RakString("PlayerDataComponent"); }
	void SerializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* destinationConnection) override;
	bool DeserializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* sourceConnection) override;
#pragma  endregion 

private:
	RakNet::RakNetGUID m_playerID; //Player ID of this component, uses RakNetGUID
	Colour m_v4PlayerColour; //Colour of this player
};

#endif //!__PLAYER_DATA_COMPONENT_H__