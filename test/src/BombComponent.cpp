#include "stdafx.h"
#include "BombComponent.h"

//Project Includes
#include "Entity.h"
#include "GameManager.h"
#include "ServerClientBase.h"
#include "TestProject.h"

//Typedefs
typedef Component PARENT;

BombComponent::BombComponent(Entity* a_pOwner) :
	PARENT(a_pOwner),
	m_fTimeSinceCreation(0.f)
{
}

/// <summary>
/// Update the bomb. Counting down until an explosion
/// </summary>
/// <param name="a_fDeltaTime"></param>
void BombComponent::Update(float a_fDeltaTime)
{
	//Only run update on server
	if(!TestProject::isServer)
	{
		return;
	}
	
	//Count down
	m_fTimeSinceCreation += a_fDeltaTime;

	//If we are over our explosion time then explode the bomb
	if(m_fTimeSinceCreation >= mc_fBombExplosionTime)
	{
		ExplodeBomb();
	}
}

//Empty Draw
void BombComponent::Draw(Shader* a_pShader)
{
}

void BombComponent::ExplodeBomb()
{
	//todo explosion

	//Destroy this bomb's owner entity
	GameManager::GetInstance()->DeleteEntityAfterUpdate(m_pOwnerEntity);
}
