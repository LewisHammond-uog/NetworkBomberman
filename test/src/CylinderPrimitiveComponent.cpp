#include "stdafx.h"
#include "CylinderPrimitiveComponent.h"

//project includes
#include "Entity.h"
#include "TransformComponent.h"

//Typedefs
typedef PrimitiveComponent PARENT;

CylinderPrimitiveComponent::CylinderPrimitiveComponent(Entity* a_pOwner, float a_fCylinderRadius/* = 1.0f*/, const float a_fCylinderHeight /* = 1.0f*/) :
	PARENT(a_pOwner), f_cylinder_radius(a_fCylinderRadius)
{
	m_ePrimitiveType = PrimitiveTypes::CYLINDER;

	//Set the cylider extends
	m_fCylinderRadius = a_fCylinderRadius;
	m_fCylinderHeight = a_fCylinderHeight;
}

CylinderPrimitiveComponent::~CylinderPrimitiveComponent()
{
}

void CylinderPrimitiveComponent::Update(float a_fDeltaTime)
{
}

void CylinderPrimitiveComponent::Draw(Shader* a_pShader)
{
	//Get the transform component - so that we can create the box at our position
	TransformComponent* pTransform = m_pOwnerEntity->GetComponent<TransformComponent*>();
	if (!pTransform) {
		return;
	}

	//Draw a cylider to the screen, centered around the current position of the owner enity, at the given resolution, in the given colour,
	//with our owners transform
	Gizmos::addCylinder(pTransform->GetCurrentPosition(), m_fCylinderRadius, m_fCylinderHeight, m_iCylinderSegments,
	                    true, m_v4DrawColour, pTransform->GetEntityMatrix());
}
