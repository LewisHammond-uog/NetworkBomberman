#include "stdafx.h"
#include "BombSpawnerComponent.h"

//Project Includes
#include "Entity.h"
#include "NetworkBlackboard.h"
#include "NetworkClient.h"
#include "TestProject.h"
#include "TransformComponent.h"

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
void BombSpawnerComponent::Update(float a_fDeltaTime)
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

void BombSpawnerComponent::ServerUpdateSpawner(float a_fDeltaTime)
{
	//Check for messages that we should spawn a bomb
	//todo - remove -1 get the player id properly
	std::vector<NetworkData*> vBombCreationRequests = NetworkBlackboard::GetInstance()->GetNetworkData(CSGameMessages::CLIENT_PLAYER_CREATE_BOMB, -1);

	//If we have any messages to create bombs then we should just create them
	//because this message has no other data attached
	if(!vBombCreationRequests.empty())
	{
		//Get our transform because this is where we are going to
		//create our bombs at our current position
		if (m_pOwnerEntity == nullptr) { return; }
		TransformComponent* pTransform = static_cast<TransformComponent*>(m_pOwnerEntity->GetComponent(COMPONENT_TYPE::TRANSFORM));
		if (pTransform == nullptr) { return; }
		const glm::vec3 bombCreatePos = pTransform->GetCurrentPosition();
		
		for(int i = 0; i < vBombCreationRequests.size(); ++i)
		{
			SpawnBomb(bombCreatePos);
		}
	}
	
}

void BombSpawnerComponent::ClientUpdateSpawner(float a_fDeltaTime)
{
	GLFWwindow* pActiveWindow = glfwGetCurrentContext();

	bool bBombSpawnKeyPressed = glfwGetKey(pActiveWindow, mc_iBombSpawnKey) == GLFW_PRESS;
	
	//Check for key press to spawn a bomb
	if (bBombSpawnKeyPressed && !m_bSpawnKeyPressedLastFrame)
	{
		//Send the server that we want to create a bomb
		//todo - remove -1 get the player id properly
		//todo - change passing RakNet::BitStream()
		NetworkBlackboard::GetInstance()->SendBlackboardDataToServer(CSGameMessages::CLIENT_PLAYER_CREATE_BOMB, -1, RakNet::BitStream());
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

	pBombEntity->AddComponent(pTransform);

	//todo call network replicator so that we can reference stuff
	
}
