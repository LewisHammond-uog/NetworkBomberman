#ifndef __PLAYER_CONTROL_COMPONENT_H__
#define __PLAYER_CONTROL_COMPONENT_H__


//Project Includes
#include "Component.h"

/// <summary>
/// Class to deal with Player Control, moving the player
/// around the world
/// </summary>
class PlayerControlComponent : public Component
{
public:
	PlayerControlComponent(Entity* a_pOwner);
	~PlayerControlComponent();

	//=Update/Draw Functions
	virtual void Update(float a_fDeltaTime);
	virtual void Draw(Shader* a_pShader);

	
#pragma region Replica Manager Functions
	
	//Entity Name
	virtual RakNet::RakString GetName(void) const { return RakNet::RakString("PlayerControlComponent"); }
	
	//Functions for Replica Manager - so that it automatically
	//sends updated velocity
	RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters* serializeParameters) override;
	void Deserialize(RakNet::DeserializeParameters* deserializeParameters) override;
#pragma endregion
	
private:

	//Seperate Functions for Server and Client Update
	//Server updates the velocity of the player
	//Client Sends key presses and sets position based on server velocity
	void ServerUpdatePlayer(float a_fDeltaTime);
	void ClientUpdatePlayer(float a_fDeltaTime);

	//Function to get the input from the player
	glm::vec2 GetPlayerKeyboardMovementInput() const;

	//Function to get our player id
	RakNet::RakNetGUID GetPlayerID() const;

	//Last sent data 
	glm::vec2 m_v2LastSentMovementInputs;
	
	//Store our current velocity - we transmit this over
	//the network rather than our position every frame
	glm::vec3 m_v3CurrentVelocity;
	glm::vec3 m_v3PlayerInput = glm::vec3(0.f);
	glm::vec3 m_v3TargetPos = glm::vec3(0.0f);

	//Speed for the player to move at
	const float mc_fMovementSpeed = 5.0f;
	
	//Keys for movement
	const int mc_iMoveUpKey =		GLFW_KEY_UP;
	const int mc_iMoveDownKey =		GLFW_KEY_DOWN;
	const int mc_iMoveLeftKey =		GLFW_KEY_LEFT;
	const int mc_iMoveRightKey =	GLFW_KEY_RIGHT;
	
};


#endif //!__PLAYER_CONTROL_COMPONENT_H__