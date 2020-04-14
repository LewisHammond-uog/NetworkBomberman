#include "stdafx.h"
#include "PlayerControlComponent.h"

#include "Application.h"

//Project Includes
#include "NetworkDataBlackboard.h"

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
	m_v3CurrentVelocity(glm::vec3(0,0,0)),
	m_v2LastSentMovementInputs(0,0)
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

	//Get our owners transform
	//todo store this?
	if (!m_pOwnerEntity) { return; }
	TransformComponent* pTransform = dynamic_cast<TransformComponent*>(m_pOwnerEntity->GetComponent(COMPONENT_TYPE::TRANSFORM));
	if (!pTransform) { return; }

	//Update based on velocity
	glm::vec3 v3CurrentPos = pTransform->GetEntityMatrixRow(POSTION_VECTOR);
	glm::vec3 v3NewPos = v3CurrentPos + m_v3CurrentVelocity;
	pTransform->SetEntityMatrixRow(POSTION_VECTOR, v3NewPos);
	
}

void PlayerControlComponent::Draw(Shader* a_pShader)
{
}


void PlayerControlComponent::ServerUpdatePlayer(float a_fDeltaTime)
{
	//Get the inputs off the blackboard
	const std::vector<PlayerInputNetworkData*> latestNetworkInputs = NetworkDataBlackboard::GetInstance()->GetPlayerInputNetworkData(m_iPlayerID);
	if (!latestNetworkInputs.empty()) {
		
		const glm::vec2 v2ClientInput = latestNetworkInputs[0]->v2MovementInputs;

		//Take our inputs and mutiply then by the movement speed and delta time
		//to get our velocity
		m_v3CurrentVelocity = glm::vec3(v2ClientInput.y, 0.f, v2ClientInput.x) * mc_fMovementSpeed * a_fDeltaTime;
	}
}

void PlayerControlComponent::ClientUpdatePlayer(float a_fDeltaTime)
{
	//Get player input
	glm::vec2 v2PlayerInput = GetPlayerKeyboardMovementInput();

	//Send to server - if our inputs have changed
	if(v2PlayerInput != m_v2LastSentMovementInputs)
	{
		PlayerInputNetworkData* pData = new PlayerInputNetworkData();
		pData->iPlayerID = m_iPlayerID;
		pData->v2MovementInputs = v2PlayerInput;
		
		NetworkDataBlackboard::GetInstance()->SendPlayerInputNetworkData(pData);

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

	//Return that we should always serialize
	return RakNet::RM3SR_SERIALIZED_ALWAYS;
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

