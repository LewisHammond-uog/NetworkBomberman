#ifndef __CYLINDER_COMPONENT_H__
#define __CYLINDER_COMPONENT_H__

//Project Includes
#include "PrimitiveComponent.h"

class CylinderPrimitiveComponent : public PrimitiveComponent
{
public:

	explicit CylinderPrimitiveComponent(Entity* a_pOwner, float a_fCylinderRadius = 1.0f, float a_fCylinderHeight = 1.0f);
	~CylinderPrimitiveComponent();
	
	void Update(float a_fDeltaTime) override;
	void Draw(Shader* a_pShader) override;

	RakNet::RakString GetName(void) const override { return RakNet::RakString("CylinderPrimitiveComponent"); };

private:
	//Cylinder Settings
	float m_fCylinderRadius;
	float m_fCylinderHeight;
	const unsigned m_iCylinderSegments = 20u;
};

#endif //!__CYLINDER_COMPONENT_H__
