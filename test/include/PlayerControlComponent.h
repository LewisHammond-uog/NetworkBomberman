#ifndef __PLAYER_CONTROL_COMPONENT_H__
#define __PLAYER_CONTROL_COMPONENT_H__


//Project Includes
#include "Component.h"

class PlayerControlComponent : public Component
{
public:
	PlayerControlComponent(Entity* a_pOwner);
	~PlayerControlComponent();

	//=Update/Draw Functions
	virtual void Update(float a_fDeltaTime);
	virtual void Draw(Shader* a_pShader);

private:

	//Seperate Functions for Server and Client Update
	//Server updates the velocity of the player
	//Client Sends key presses and sets position based on server velocity
	void ServerUpdatePlayer(float a_fDeltaTime);
	void ClientUpdatePlayer(float a_fDeltaTime);

	//Function to get the input from the player
	glm::vec2 GetPlayerKeyboardMovementInput() const;

	//Player ID
	int m_iPlayerID;

	//Last sent data 
	glm::vec2 m_v2LastSentMovementInputs;
	
	//Store our current velocity - we transmit this over
	//the network rather than our position every frame
	glm::vec3 m_v3CurrentVelocity;

	//Speed for the player to move at
	const float mc_fMovementSpeed = 5.0f;
	
	//Keys for movement
	const int mc_iMoveUpKey =		GLFW_KEY_UP;
	const int mc_iMoveDownKey =		GLFW_KEY_DOWN;
	const int mc_iMoveLeftKey =		GLFW_KEY_LEFT;
	const int mc_iMoveRightKey =	GLFW_KEY_RIGHT;
	
};


#endif //!__PLAYER_CONTROL_COMPONENT_H__