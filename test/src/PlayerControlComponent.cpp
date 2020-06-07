#include "stdafx.h"
#include "PlayerControlComponent.h"

#include "Application.h"

//Project Includes
#include "NetworkBlackboard.h"

//Project includes
#include "ServerClientBase.h"
#include "TransformComponent.h"
#include "Entity.h"
#include "PlayerDataComponent.h"
#include "TestProject.h"
#include "LevelManager.h"
#include "Level.h"

//Typedefs
typedef Component PARENT;


PlayerControlComponent::PlayerControlComponent(Entity* a_pOwner) :
	PARENT(a_pOwner),
	m_v2LastSentMovementInputs(0,0),
	m_v3CurrentVelocity(glm::vec3(0, 0, 0))
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
		//Only allow player updates if the player data (i.e the player that owns
		//this component) is us so we can't control other players		
		if (ServerClientBase::GetSystemGUID() == GetPlayerID()) {
			ClientUpdatePlayer(a_fDeltaTime);
		}
	}
}

void PlayerControlComponent::Draw(Shader* a_pShader)
{
}

/// <summary>
/// Do Server Updates on the server,
/// moving the player according to inputs
/// </summary>
/// <param name="a_fDeltaTime"></param>
void PlayerControlComponent::ServerUpdatePlayer(float a_fDeltaTime)
{	
	//Get the inputs off the blackboard
	const std::vector<NetworkData*> latestNetworkInputs = NetworkBlackboard::GetInstance()->GetNetworkData(
		CSGameMessages::CLIENT_PLAYER_INPUT_DATA, GetPlayerID());
	
	//Get our owners transform
	TransformComponent* pTransform = dynamic_cast<TransformComponent*>(m_pOwnerEntity->GetComponent(COMPONENT_TYPE::TRANSFORM));
	if (!pTransform) { return; }

	//Get the players current position, so we can see if we can move
	glm::vec3 v3PlayerPos = pTransform->GetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR);

	//See if there is an input from the network
	if (!latestNetworkInputs.empty()) {
		
		//Get the movement amount
		glm::vec2 v2ClientInput = glm::vec2(0, 0);
		latestNetworkInputs[0]->m_data.Read(v2ClientInput);

		//Translate the client movement in a potential velocity, used if the next tile
		//is free, ignore the y component as we never jump
		m_v3PlayerInput = glm::vec3(v2ClientInput.y, Level::sc_fLevelY, v2ClientInput.x) * Level::sc_fLevelSpacing;
	}

	//If we have input then update our target position to be the next cell in the
	//direction that the player is going in
	if (m_v3PlayerInput != glm::vec3(0, 0, 0)) {
		if (LevelManager::GetCurrentLevel() != nullptr)
		{
			if (LevelManager::GetCurrentLevel()->IsCellFree(v3PlayerPos + m_v3PlayerInput))
			{
				//We have a valid grid cell, set it to be our target position
				m_v3TargetPos = LevelManager::GetCurrentLevel()->GetNearestCell(v3PlayerPos + m_v3PlayerInput) * Level::sc_fLevelSpacing;
			}
		}
	}

	//--Move towards the target position--//
	
	//Get the direction
	glm::vec3 v3CellDir = glm::length(m_v3TargetPos - v3PlayerPos) != 0 ? glm::normalize(m_v3TargetPos - v3PlayerPos) : m_v3TargetPos - v3PlayerPos;
	//Calculate the amount we will move towards the target this frame
	m_v3CurrentVelocity = v3CellDir * mc_fMovementSpeed * a_fDeltaTime;
	glm::vec3 v3NewPos;
	
	//If our movement this frame is less than the distance to the cell then set out new position to the cell
	if (glm::length(m_v3CurrentVelocity) > glm::length(m_v3TargetPos - v3PlayerPos))
	{
		v3NewPos = m_v3TargetPos;
	}else
	{
		//Otherwise keep moving towards it
		v3NewPos = v3PlayerPos + m_v3CurrentVelocity;
	}

	//Move the player object
	pTransform->SetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR, v3NewPos);
	
}

/// <summary>
/// Client Updates for the player, takes in inputs from
/// the keyboards and send them to the server
/// </summary>
/// <param name="a_fDeltaTime"></param>
void PlayerControlComponent::ClientUpdatePlayer(float a_fDeltaTime)
{
	//Get player input
	glm::vec2 v2PlayerInput = GetPlayerKeyboardMovementInput();

	//Send to server - if our inputs have changed
	if(v2PlayerInput != m_v2LastSentMovementInputs)
	{
		//Write bitstream of player input data
		RakNet::BitStream moveData;
		moveData.Write(v2PlayerInput);
		
		NetworkBlackboard::GetInstance()->SendBlackboardDataToServer(CSGameMessages::CLIENT_PLAYER_INPUT_DATA, moveData);

		m_v2LastSentMovementInputs = v2PlayerInput;
	}
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

/// <summary>
/// Gets the Player ID from the player data component,
/// simplifies calls and prevents null ptrs
/// </summary>
/// <returns></returns>
RakNet::RakNetGUID PlayerControlComponent::GetPlayerID() const
{
	//Get player Data Component, if it null return a 'null' GUID
	PlayerDataComponent* playerData = static_cast<PlayerDataComponent*>(m_pOwnerEntity->GetComponent(COMPONENT_TYPE::PLAYER_DATA));
	if (!playerData)
	{
		return RakNet::RakNetGUID();
	}

	return playerData->GetPlayerID();
}

/// <summary>
/// Called by the Replica Manager
/// Seralises Parameters for the Player Control Component to send to the server
/// In this case it sends our velocity once it has updated
/// </summary>
/// <param name="serializeParameters"></param>
/// <returns></returns>
RakNet::RM3SerializationResult PlayerControlComponent::Serialize(RakNet::SerializeParameters* serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;

	//Write reliability type
	serializeParameters->pro[0].reliability = PacketReliability::RELIABLE;

	//Begin Serialization of the data that we are going to send
	m_variableDeltaSerializer.BeginIdenticalSerialize(
		&serializationContext,
		serializeParameters->whenLastSerialized == 0,
		&serializeParameters->outputBitstream[0]
	);

	//Serialize Variables
	m_variableDeltaSerializer.SerializeVariable(&serializationContext, m_v3CurrentVelocity);

	//Send only if the data has changed
	return RakNet::RM3SR_BROADCAST_IDENTICALLY;
}

/// <summary>
/// Called by the Replica Manager
/// Deserializes data that has been sent from the server
/// In this case it will give us our updated velocity
/// </summary>
/// <param name="deserializeParameters"></param>
void PlayerControlComponent::Deserialize(RakNet::DeserializeParameters* deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;
	
	//Deserialise the data
	m_variableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	
	m_variableDeltaSerializer.DeserializeVariable(&deserializationContext, m_v3CurrentVelocity);
	
	m_variableDeltaSerializer.EndDeserialize(&deserializationContext);
}

