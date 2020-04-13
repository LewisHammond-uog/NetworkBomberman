#include "stdafx.h"
#include "PlayerControlComponent.h"

#include "Application.h"

//todo remove
#include "ServerClientBase.h"
#include "TransformComponent.h"
#include "Entity.h"
#include "TestProject.h"

//Typedefs
typedef Component PARENT;


PlayerControlComponent::PlayerControlComponent(Entity* a_pOwner) :
	PARENT(a_pOwner),
	m_iPlayerID(-1),//Init to -1 so we know that this is not initalised
	m_v3CurrentVelocity(glm::vec3(0,0,0))
{
	//Set Component Type
	m_eComponentType = COMPONENT_TYPE::PLAYER_CONTROL;
	
}

PlayerControlComponent::~PlayerControlComponent()
{
}

/// <summary>
/// Updates the player movement.
/// If we are on the server we will update the velocity based on player key presses
/// If we are on the client we will update the position based on the velocity from the server
/// </summary>
/// <param name="a_fDeltaTime">Delta Time</param>
void PlayerControlComponent::Update(float a_fDeltaTime)
{
	//Update based on whether we are server or not
	//Server actually updates the velocity, based on client key press
	//Client sends key presses for the server to process
	if(TestProject::isServer)
	{
		ServerUpdatePlayer(a_fDeltaTime);
	}else
	{
		ClientUpdatePlayer(a_fDeltaTime);
	}	
}

void PlayerControlComponent::Draw(Shader* a_pShader)
{
}

void PlayerControlComponent::ServerUpdatePlayer(float a_fDeltaTime)
{
	//Get our owners transform
	if (!m_pOwnerEntity) { return; }
	TransformComponent* pTransform = dynamic_cast<TransformComponent*>(m_pOwnerEntity->GetComponent(COMPONENT_TYPE::TRANSFORM));
	if (!pTransform) { return; }

	//todo this should get inputs from the clients
	glm::vec2 v2ClientInput = GetPlayerKeyboardMovementInput();

	//Take our inputs and mutiply then by the movement speed and delta time
	//to get our velocity
	m_v3CurrentVelocity = glm::vec3(v2ClientInput.y, 0.f, v2ClientInput.x) * mc_fMovementSpeed * a_fDeltaTime;

	//Move the entity in the directions that the user has specified
	glm::vec3 v3CurrentPos = pTransform->GetEntityMatrixRow(POSTION_VECTOR);
	glm::vec3 v3NewPos = v3CurrentPos + m_v3CurrentVelocity;
	pTransform->SetEntityMatrixRow(POSTION_VECTOR, v3NewPos);
}

void PlayerControlComponent::ClientUpdatePlayer(float a_fDeltaTime)
{
	//Get player input
	glm::vec2 v2PlayerInput = GetPlayerKeyboardMovementInput();

	//Send to server
}

/// <summary>
/// Gets the player inputs for movement, direct from a connected keyboard
/// This is direction based so will return (1, 0 or -1) based
/// on the inputs that have been pressed
/// </summary>
/// <returns></returns>
glm::vec2 PlayerControlComponent::GetPlayerKeyboardMovementInput() const
{
	//Get the Horizontal and Vertical Movement from the up/down keys
	//Use -ive for the up/left key so we mutiply by this value for our movement calculations
	GLFWwindow* pActiveWindow = glfwGetCurrentContext();

	//Null Check the current input
	if(!pActiveWindow)
	{
		return glm::vec2(0, 0);
	}
	
	const int iVertInput = glfwGetKey(pActiveWindow, mc_iMoveDownKey) | -glfwGetKey(pActiveWindow, mc_iMoveUpKey);
	const int iHorizInput = glfwGetKey(pActiveWindow, mc_iMoveRightKey) | -glfwGetKey(pActiveWindow, mc_iMoveLeftKey);

	return glm::vec2(iVertInput, iHorizInput);
}


