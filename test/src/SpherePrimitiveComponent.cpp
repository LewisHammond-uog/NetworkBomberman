#include "stdafx.h"
#include "SpherePrimitiveComponent.h"

//Project Includes
#include "Entity.h"
#include "Gizmos.h"
#include "TransformComponent.h"

//Typedefs
typedef PrimitiveComponent PARENT;

SpherePrimitiveComponent::SpherePrimitiveComponent(Entity* a_pOwner, const float a_fSphereRadius) :
	PARENT(a_pOwner),
	m_fSphereRadius(a_fSphereRadius)
{
	//Set our Component Type
	m_eComponentType = COMPONENT_TYPE::PRIMITIVE_SPHERE;
	//Set our Primitive type
	m_ePrimitiveType = PrimitiveTypes::SPHERE;
}

/// <summary>
/// Set the radius of the sphere
/// </summary>
/// <param name="a_fNewRadius">Radius to the Sphere</param>
void SpherePrimitiveComponent::SetDimensions(const float a_fNewRadius)
{
	m_fSphereRadius = a_fNewRadius;
}

void SpherePrimitiveComponent::Update(float a_fDeltaTime)
{
}

void SpherePrimitiveComponent::Draw(Shader* a_pShader)
{
	//Get the transform component - so that we can create the box at our position
	TransformComponent* pTransform = dynamic_cast<TransformComponent*>(m_pOwnerEntity->GetComponent(COMPONENT_TYPE::TRANSFORM));
	if (!pTransform) {
		return;
	}

	//Draw a sphere to the screen, centered around the current position of the owner enity, at the given resolution, in the given colour,
	//with our owners transform
	Gizmos::addSphere(pTransform->GetCurrentPosition(), m_iSphereResolution, m_iSphereResolution, m_fSphereRadius,
		m_v4DrawColour, &pTransform->GetEntityMatrix());
}
