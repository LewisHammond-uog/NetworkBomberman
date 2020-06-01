#include "stdafx.h"
#include "PrimitiveComponent.h"

//Typedefs
typedef Component PARENT;


PrimitiveComponent::PrimitiveComponent(Entity* a_pOwner) : PARENT(a_pOwner)
{
}


/// <summary>
/// Set the colour of the primitive
/// </summary>
/// <param name="a_v4Colour">New Color to set the primitive in the format (R: 0-1, G: 0-1, B: 0-1, A: 0-1)</param>
void PrimitiveComponent::SetColour(const glm::vec4 a_v4Colour)
{
	m_v4DrawColour = a_v4Colour;
}

/// <summary>
/// Serialise the Construction of our primative component. Setting the colour, called on server, sent to client
/// </summary>
/// <param name="constructionBitstream"></param>
/// <param name="destinationConnection"></param>
void PrimitiveComponent::SerializeConstruction(RakNet::BitStream* constructionBitstream,
	RakNet::Connection_RM3* destinationConnection)
{
	//Call base function
	Component::SerializeConstruction(constructionBitstream, destinationConnection);

	/* CONSTRUCTION DATA LAYOUT
	 * glm::vec3 Colour
	 */

	//Serialize the colour we have
	glm::vec4 v4Colour = m_v4DrawColour;
	constructionBitstream->Write(m_v4DrawColour);
}

/// <summary>
/// SDeerialise the Construction of our primative component. Setting the colour, sent by server, set on client
/// </summary>
/// <param name="constructionBitstream"></param>
/// <param name="sourceConnection"></param>
/// <returns></returns>
bool PrimitiveComponent::DeserializeConstruction(RakNet::BitStream* constructionBitstream,
	RakNet::Connection_RM3* sourceConnection)
{
	//If bitstream is empty then early out
	if (constructionBitstream->GetNumberOfUnreadBits() == 0) {
		return false;
	}

	//Call base function
	Component::DeserializeConstruction(constructionBitstream, sourceConnection);

	/* CONSTRUCTION DATA LAYOUT
	 * glm::vec3 Colour
	 */

	//Get colour value
	glm::vec4 receivedColour;
	constructionBitstream->Read(receivedColour);

	//Set Colour
	SetColour(receivedColour);
	
	
	return true;
}
