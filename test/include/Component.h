#ifndef __COMPONENT_H__
#define __COMPONENT_H__

//Project Includes
#include <RakString.h>
#include <ReplicaManager3.h>

#include "ServerCreatedObject.h"

//Forward Declare
class Entity;
class Shader;

/// <summary>
/// Abstract class for a component which is used so we can
/// loop all components with Update() and Draw() functions
/// genericly 
/// </summary>
class Component : public ServerCreatedObject
{
public:
	explicit Component(Entity* a_pOwner);
	~Component();

	//[TO DO] - Shared Function for Update/Draw?
	virtual void Update(float a_fDeltaTime) = 0; //Pure Virtual Function
	virtual void Draw(Shader* a_pShader) = 0; //Pure Virtual Function

	Entity* GetOwnerEntity() const;
	void RemoveOwnerEntity();

#pragma region Replica Manager Functions
	//Entity Name
	RakNet::RakString GetName(void) const override { return RakNet::RakString("Component"); }

	//Serialise/Deserialise functions
	RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters* serializeParameters) override;
	void Deserialize(RakNet::DeserializeParameters* deserializeParameters) override;

	//Fuctions for deserialise and serialize on create
	void SerializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* destinationConnection) override;
	bool DeserializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* sourceConnection) override;
	void SerializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* destinationConnection) override;
	bool DeserializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* sourceConnection) override;
#pragma  endregion 

protected:
	Entity* m_pOwnerEntity;
};

#endif	//!__COMPONENT_H__

