#ifndef __BOX_PRIMITIVE_COMPONENT_H__
#define __BOX_PRIMITIVE_COMPONENT_H__

//Project Includes
#include <glm/glm.hpp>
#include "PrimitiveComponent.h"

/// <summary>
/// Class for a drawing a box primative
/// </summary>
class BoxPrimitiveComponent : public PrimitiveComponent
{
public:
	explicit BoxPrimitiveComponent(Entity* a_pOwner, glm::vec3 a_v3BoxDimensions = glm::vec3(1, 1, 1));
	~BoxPrimitiveComponent() = default;

	//Function to set size
	void SetDimensions(glm::vec3 a_v3NewDimensions);
	
	//Update/Draw Functions - have no implementation as primatives
	//cannot be created 
	void Update(float a_fDeltaTime) override;
	void Draw(Shader* a_pShader) override;

	RakNet::RakString GetName(void) const override { return RakNet::RakString("BoxPrimitiveComponent"); }
	
private:
	glm::vec3 m_v3BoxDimensions;
};

#endif //!__BOX_PRIMITIVE_COMPONENT_H__