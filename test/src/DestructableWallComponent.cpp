#include "stdafx.h"
#include "DestructableWallComponent.h"

//Project Includes
#include "GameManager.h"

typedef Component PARENT;

/// <summary>
/// Create a Wall Component
/// </summary>
/// <param name="a_pOwnerEntity"></param>
/// <param name="a_bIsDestructible"></param>
DestructableWallComponent::DestructableWallComponent(Entity* a_pOwnerEntity) :
	PARENT(a_pOwnerEntity)
{
}

DestructableWallComponent::~DestructableWallComponent()
{
}

/// <summary>
/// Attempt to destroy the wall.
/// Wall will only be destroyed if it is a destructable wall
/// </summary>
/// <returns>If the wall was destroyed</returns>
bool DestructableWallComponent::DestroyWall() const
{
	//Wall is destructable, destroy after this frame
	GameManager::GetInstance()->DeleteEntityAfterUpdate(m_pOwnerEntity);
	return false;
}
