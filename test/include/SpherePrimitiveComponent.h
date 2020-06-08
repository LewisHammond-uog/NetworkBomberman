#ifndef __SPHERE_PRIMITIVE_COMPONENT_H__
#define __SPHERE_PRIMITIVE_COMPONENT_H__

//Project Includes

#include "PrimitiveComponent.h"

/// <summary>
/// Class for drawing a sphere primative
/// </summary>
class SpherePrimitiveComponent : public PrimitiveComponent
{
public:
	explicit SpherePrimitiveComponent(Entity* a_pOwner, float a_fSphereRadius = 1.0f);
	~SpherePrimitiveComponent() = default;

	//Function to set size
	void SetDimensions(float a_fNewRadius);
	
	//Update/Draw Functions - have no implementation as primatives
	//cannot be created 
	void Update(float a_fDeltaTime) override;
	void Draw(Shader* a_pShader) override;

	RakNet::RakString GetName(void) const override { return RakNet::RakString("SpherePrimitiveComponent"); }

private:
	float m_fSphereRadius;
	//Number of rows/columns to draw the circle our of
	//a higher number means a more precise sphere
	int m_iSphereResolution = 10;
};


#endif //!__SPHERE_PRIMITIVE_COMPONENT_H__
