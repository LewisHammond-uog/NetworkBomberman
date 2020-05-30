#ifndef __COMPONENT_H__
#define __COMPONENT_H__

//Project Includes
#include <RakString.h>
#include <ReplicaManager3.h>
#include <VariableDeltaSerializer.h>

#include "ServerCreatedObject.h"

//Forward Declare
class Entity;
class Shader;

enum class COMPONENT_TYPE {
	NONE,
	TRANSFORM,
	MODEL,
	COLLIDER,
	PLAYER_CONTROL,
	PLAYER_DATA,
	PRIMITIVE_BOX,
	PRIMITIVE_SPHERE
};

/// <summary>
/// Abstract class for a component which is used so we can
/// loop all components with Update() and Draw() functions
/// genericly 
/// </summary>
class Component : public ServerCreatedObject
{
public:
	Component(Entity* a_pOwner);
	~Component();

	//[TO DO] - Shared Function for Update/Draw?
	virtual void Update(float a_fDeltaTime) = 0; //Pure Virtual Function
	virtual void Draw(Shader* a_pShader) = 0; //Pure Virtual Function

	Entity* GetOwnerEntity() const;
	COMPONENT_TYPE GetComponentType() const;
	void RemoveOwnerEntity();

#pragma region Replica Manager Functions
	//Entity Name
	virtual RakNet::RakString GetName(void) const { return RakNet::RakString("Component"); }

	//Serialise/Deserialise functions
	virtual RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters* serializeParameters);
	virtual void Deserialize(RakNet::DeserializeParameters* deserializeParameters);

	//Fuctions for deserialise and serialize on create
	virtual void SerializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* destinationConnection);
	virtual bool DeserializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* sourceConnection);
	virtual void SerializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* destinationConnection);
	virtual bool DeserializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* sourceConnection);
#pragma  endregion 

protected:
	Entity* m_pOwnerEntity;
	COMPONENT_TYPE m_eComponentType;
};

#endif	//!__COMPONENT_H__

