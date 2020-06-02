#include "stdafx.h"
#include "DestructableWallComponent.h"

//Project Includes
#include "GameManager.h"
#include "Entity.h"
#include "ColliderComponent.h"
#include "BoxPrimitiveComponent.h"

typedef Component PARENT;

/// <summary>
/// Create a Wall Component
/// </summary>
/// <param name="a_pOwnerEntity"></param>
/// <param name="a_bIsDestructible"></param>
DestructableWallComponent::DestructableWallComponent(Entity* a_pOwnerEntity) :
	PARENT(a_pOwnerEntity)
{
	m_eComponentType = COMPONENT_TYPE::DESTRUCTABLE_WALL;
}

DestructableWallComponent::~DestructableWallComponent()
{
}

/// <summary>
/// Attempt to destroy the wall.
/// Wall will only be destroyed if it is a destructable wall
/// </summary>
/// <returns>If the wall was destroyed</returns>
void DestructableWallComponent::DestroyWall() const
{
	//Set our entity to inactive
	if (m_pOwnerEntity) {
		m_pOwnerEntity->SetEnabled(false);
	}
}
