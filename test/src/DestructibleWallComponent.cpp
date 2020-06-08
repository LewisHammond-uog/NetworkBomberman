#include "stdafx.h"
#include "DestructibleWallComponent.h"

//Project Includes
#include "Entity.h"

typedef Component PARENT;

/// <summary>
/// Create a Wall Component
/// </summary>
/// <param name="a_pOwnerEntity"></param>
DestructibleWallComponent::DestructibleWallComponent(Entity* a_pOwnerEntity) :
	PARENT(a_pOwnerEntity)
{
}

DestructibleWallComponent::~DestructibleWallComponent()
{
}

/// <summary>
/// Attempt to destroy the wall.
/// Wall will only be destroyed if it is a destructable wall
/// </summary>
/// <returns>If the wall was destroyed</returns>
void DestructibleWallComponent::DestroyWall() const
{
	//Set our entity to inactive
	if (m_pOwnerEntity) {
		m_pOwnerEntity->SetEnabled(false);
	}
}
