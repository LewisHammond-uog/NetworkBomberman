#include "stdafx.h"
#include "BombComponent.h"

//Project Includes
#include "Entity.h"
#include "GameManager.h"
#include "RaycastComponent.h"
#include "ServerClientBase.h"
#include "TestProject.h"
#include "DestructableWallComponent.h"

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
void BombComponent::Update(const float a_fDeltaTime)
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

/// <summary>
/// Explodes the bomb, doing a raycast and destorying any objects
/// that is comes in to contact with
/// </summary>
void BombComponent::ExplodeBomb() const
{
	/*
	Raycast in 4 directions up, down, left, right
	and check if we hit a player or a destructable wall
	*/

	//Check we have a raycaster component
	RaycastComponent* pRayCaster = dynamic_cast<RaycastComponent*>(m_pOwnerEntity->GetComponent(COMPONENT_TYPE::RAYCAST));
	if (pRayCaster == nullptr)
	{


		//Get the collision rays and do a raycast on all of them
		const std::vector<rp3d::Ray*> vV3CollisionRays = GetCollisionRays();
		std::vector<RayCastHitsInfo*> vRayResults = pRayCaster->MutiRayCast(vV3CollisionRays, true);

		//Check if any of our raycasts have hit anything
		for (int resultsIndex = 0; resultsIndex < vRayResults.size(); ++resultsIndex)
		{
			RayCastHitsInfo* currentResult = vRayResults[resultsIndex];
			for (int hitIndex = 0; hitIndex < currentResult->m_vRayCastHits.size(); ++hitIndex)
			{
				//We have hit something, apply what should happen if it is a player or a wall.
				//Kill Player | Destroy Wall
				RayCastHit* currentHit = currentResult->m_vRayCastHits[hitIndex];

				//Hit Player
				if (currentHit->m_pHitEntity->GetComponent(COMPONENT_TYPE::PLAYER_DATA))
				{
					//todo kill player
				}

				//Hit Wall
				DestructableWallComponent* pWall = dynamic_cast<DestructableWallComponent*>(currentHit->m_pHitEntity->GetComponent(COMPONENT_TYPE::DESTRUCTABLE_WALL));
				if (pWall)
				{
					pWall->DestroyWall();
				}
			}

			//Delete the ray we are done with it
			delete currentResult;
		}
	}
	
	
	//Destroy this bomb's owner entity
	GameManager::GetInstance()->DeleteEntityAfterUpdate(m_pOwnerEntity);
}

// <summary>
/// Generates a list of rays to use when checking for collisions
/// </summary>
/// <returns>List of a rays the use for collision</returns>
std::vector<rp3d::Ray*> BombComponent::GetCollisionRays() const
{
	//Create Vector
	std::vector<rp3d::Ray*> vRays;

	//Null Check and get transform
	if (!m_pOwnerEntity)
	{
		return vRays;
	}
	TransformComponent* pTransform = dynamic_cast<TransformComponent*>(m_pOwnerEntity->GetComponent(COMPONENT_TYPE::TRANSFORM));
	if (!pTransform)
	{
		return vRays;
	}

	//Get our current position, so we can make our rays relative
	const glm::vec3 v3CurrentPos = pTransform->GetCurrentPosition();

	//Number of directions we have (6 all directions around the boid)
	constexpr int iDirectionCount = 4;

	//Get all of the directions that we want to cast in
	const glm::vec3 v3Forward = pTransform->GetEntityMatrixRow(MATRIX_ROW::FORWARD_VECTOR);
	const glm::vec3 v3Right = pTransform->GetEntityMatrixRow(MATRIX_ROW::RIGHT_VECTOR);
	glm::vec3 vV3PositiveDirections[2] = { v3Forward,v3Right };

	//Loop through the directions and mutiply half of them by -1 so we have the inverse's
	for (int i = 0; i < iDirectionCount; ++i)
	{
		//Divide the current direction by 2 so that we grab
		//the direction twice and on 1 of those times * it by -1 so
		//we get the inverse
		const int iCurrentDirectionIndex = floor(i / 2);

		glm::vec3 v3CurrentRayDir = vV3PositiveDirections[iCurrentDirectionIndex];

		//If the index is odd then divide by 2 so we get the inerse
		if (i % 2 != 0)
		{
			v3CurrentRayDir *= -1;
		}

		//Create a ray from the direction and our current position
		const glm::vec3 v3EndPos = v3CurrentPos + (v3CurrentRayDir * mc_fBombRange); //End pos is direction * distance, in this case our neighbourbood radius plus our velcityy
		rp3d::Ray* pCreatedRay = new rp3d::Ray(v3CurrentPos, v3EndPos);
		vRays.push_back(pCreatedRay);
	}

	return vRays;
}