#ifndef __PRIMITIVE_COMPONENT_H__
#define __PRIMITIVE_COMPONENT_H__

//Glm includes
#include <glm/vec4.hpp>

//Project Includes
#include "Component.h"

/// <summary>
/// Abstract Class for types of primitives that we can draw to the screen
/// </summary>
class PrimitiveComponent : public Component
{
public:
	
	//Pure Virtual Update/Draw Functions -
	//have no implementation as primatives
	//cannot be created 
	virtual void Update(float a_fDeltaTime) = 0;
	virtual void Draw(Shader* a_pShader) = 0;
	
	//Function to set colour of the primitive
	void SetColour(glm::vec4 a_v4Colour);

	#pragma region RakNet Functions
	void SerializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* destinationConnection) override;
	bool DeserializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* sourceConnection) override;
	#pragma endregion
	
protected:
	//Protected Constructors so that we cannot instantiate this class
	PrimitiveComponent(Entity* a_pOwner);
	~PrimitiveComponent() = default;

	enum class PrimitiveTypes
	{
		SPHERE,
		BOX,
		CYLINDER
	};

	//Type of Primitive that we are
	PrimitiveTypes m_ePrimitiveType;

	//Store the colour of the Primitive,
	//in the format (R: 0-1, G: 0-1, B: 0-1, A: 0-1)
	glm::vec4 m_v4DrawColour;
};

#endif //!__PRIMITIVE_COMPONENT_H__
