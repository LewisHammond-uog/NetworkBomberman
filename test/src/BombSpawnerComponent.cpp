#include "stdafx.h"
#include "BombSpawnerComponent.h"

//Project Includes
#include "Entity.h"
#include "NetworkBlackboard.h"
#include "NetworkClient.h"
#include "TestProject.h"
//Components
#include "TransformComponent.h"
#include "SpherePrimitiveComponent.h"
#include "BombComponent.h"
#include "GameManager.h"
#include "PlayerDataComponent.h"
#include "RaycastComponent.h"

//Typedefs
typedef Component PARENT;

/// <summary>
/// Constructor for Bomb Spawner
/// </summary>
/// <param name="a_pOwner"></param>
BombSpawnerComponent::BombSpawnerComponent(Entity* a_pOwner) :
	PARENT(a_pOwner),
	m_bSpawnKeyPressedLastFrame(false)
{
}

/// <summary>
/// Update the Bomb Spawner Component
/// </summary>
/// <param name="a_fDeltaTime">Delta Time</param>
auto BombSpawnerComponent::Update(float a_fDeltaTime) -> void
{
	if(TestProject::isServer)
	{
		ServerUpdateSpawner(a_fDeltaTime);
	}else
	{
		ClientUpdateSpawner(a_fDeltaTime);
	}
}

/// <summary>
/// Draw the Bomb Spawner Component
/// </summary>
/// <param name="a_pShader">Shader to Draw with</param>
void BombSpawnerComponent::Draw(Shader* a_pShader)
{
}

/// <summary>
/// Update the spawner (on the server)
/// </summary>
/// <param name="a_fDeltaTime">Delta Time</param>
void BombSpawnerComponent::ServerUpdateSpawner(float a_fDeltaTime)
{
	//Check for messages that we should spawn a bomb
	PlayerDataComponent* pPlayerData = dynamic_cast<PlayerDataComponent*>(m_pOwnerEntity->GetComponent(COMPONENT_TYPE::PLAYER_DATA));
	if (!pPlayerData)
	{
		return;
	}
	
	std::vector<NetworkData*> vBombCreationRequests = NetworkBlackboard::GetInstance()->GetNetworkData(CSGameMessages::CLIENT_PLAYER_CREATE_BOMB, pPlayerData->GetPlayerID());

	//If we have any messages to create bombs then we should just create them
	//because this message has no other data attached
	if(!vBombCreationRequests.empty())
	{
		//Get our transform because this is where we are going to
		//create our bombs at our current position
		if (m_pOwnerEntity == nullptr) { return; }
		TransformComponent* pPlayerTransform = dynamic_cast<TransformComponent*>(m_pOwnerEntity->GetComponent(COMPONENT_TYPE::TRANSFORM));
		if (pPlayerTransform == nullptr) { return; }

		//Create the bomb at the players position
		glm::vec3 bombCreatePos = pPlayerTransform->GetCurrentPosition();

		
		for(unsigned int i = 0; i < vBombCreationRequests.size(); ++i)
		{
			SpawnBomb(bombCreatePos);
		}
	}
	
}

void BombSpawnerComponent::ClientUpdateSpawner(float a_fDeltaTime)
{
	//Get if the client has pressed the key to spawn the bomb
	GLFWwindow* pActiveWindow = glfwGetCurrentContext();
	const bool bBombSpawnKeyPressed = (glfwGetKey(pActiveWindow, mc_iBombSpawnKey) == GLFW_PRESS);
	
	//Check for key press to spawn a bomb
	if (bBombSpawnKeyPressed && !m_bSpawnKeyPressedLastFrame)
	{
		//Send the server that we want to create a bomb
		NetworkBlackboard::GetInstance()->SendBlackboardDataToServer(CSGameMessages::CLIENT_PLAYER_CREATE_BOMB, RakNet::BitStream());
	}

	m_bSpawnKeyPressedLastFrame = bBombSpawnKeyPressed;
}

void BombSpawnerComponent::SpawnBomb(glm::vec3 a_v3SpawnPosition)
{
	//Bombs can only be spawned by the server
	if(!TestProject::isServer)
	{
		return;
	}
	
	//Create Bomb Entity and add the approprite components
	Entity* pBombEntity = new Entity();
	TransformComponent* pTransform = new TransformComponent(pBombEntity);
	BombComponent* pBombComponent = new BombComponent(pBombEntity);
	SpherePrimitiveComponent* pSphere = new SpherePrimitiveComponent(pBombEntity);
	RaycastComponent* pRaycaster = new RaycastComponent(pBombEntity);
	
	//Set Transform position to be the inputted position
	pTransform->SetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR, a_v3SpawnPosition);
	
	pBombEntity->AddComponent(pTransform);
	pBombEntity->AddComponent(pBombComponent);
	pBombEntity->AddComponent(pSphere);
	pBombEntity->AddComponent(pRaycaster);

	//Add the new bomb to the network replicator so it is sent to all clients
	NetworkReplicator* pNetworkReplicator = ServerClientBase::GetNetworkReplicator();
	if (pNetworkReplicator == nullptr)
	{
		return;
	}
	pNetworkReplicator->Reference(pBombEntity);
	pNetworkReplicator->Reference(pTransform);
	pNetworkReplicator->Reference(pBombComponent);
	pNetworkReplicator->Reference(pSphere);
	pNetworkReplicator->Reference(pRaycaster);
	
}
