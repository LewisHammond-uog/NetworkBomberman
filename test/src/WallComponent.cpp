#include "stdafx.h"
#include "WallComponent.h"

//Project Includes
#include "GameManager.h"

typedef Component PARENT;

/// <summary>
/// Create a Wall Component
/// </summary>
/// <param name="a_pOwnerEntity"></param>
/// <param name="a_bIsDestructible"></param>
WallComponent::WallComponent(Entity* a_pOwnerEntity, const bool a_bIsDestructible /*=false*/) :
	PARENT(a_pOwnerEntity),
	m_bIsDestructible(a_bIsDestructible)
{
}

WallComponent::~WallComponent()
{
}

/// <summary>
/// Attempt to destroy the wall.
/// Wall will only be destroyed if it is a destructable wall
/// </summary>
/// <returns>If the wall was destroyed</returns>
bool WallComponent::AttemptDestroyWall() const
{
	//Check that this wall is destructable, if it is not don't destroy it
	if(!m_bIsDestructible)
	{
		return false;
	}

	//Wall is destructable, destroy after this frame
	GameManager::GetInstance()->DeleteEntityAfterUpdate(m_pOwnerEntity);
	return false;
}
