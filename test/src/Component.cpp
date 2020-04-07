#include "stdafx.h"
#include "Component.h"

//Project Include
#include "Entity.h"

//TODO REMOVE INCLUDES
#include "ServerClientBase.h"
#include "GameManager.h"

Component::Component(Entity* a_pOwner) : m_pOwnerEntity(a_pOwner),
											m_eComponentType(NONE)
{
	//Set our network mananger
	NetworkIDObject::SetNetworkIDManager(ServerClientBase::GetNetworkIDManager());

	//todo remove - this is just debug testing
	ServerClientBase::LogConsoleMessage("Create Component");
}


Component::~Component()
{
	
}


RakNet::RM3SerializationResult Component::Serialize(RakNet::SerializeParameters* serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;
	
	//Write reliability type
	serializeParameters->pro[0].reliability = PacketReliability::RELIABLE;

	//Begin Serialization of the data that we are going to send
	m_variableDeltaSerializer.BeginIdenticalSerialize(
		&serializationContext,
		serializeParameters->whenLastSerialized == 0,
		&serializeParameters->outputBitstream[0]
	);

	//Serialize Variables
	m_variableDeltaSerializer.SerializeVariable(&serializationContext, 0);
	m_variableDeltaSerializer.SerializeVariable(&serializationContext, 0);

	//Return that we should always serialize
	return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void Component::Deserialize(RakNet::DeserializeParameters* deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;


	//Deserialise the data
	m_variableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	//m_variableDeltaSerializer.DeserializeVariable(&deserializationContext, m_sObjName);
	//m_variableDeltaSerializer.DeserializeVariable(&deserializationContext, m_fHealth);

	m_variableDeltaSerializer.EndDeserialize(&deserializationContext);
}

void Component::SerializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* destinationConnection)
{
	// variableDeltaSerializer is a helper class that tracks what variables were sent to what remote system
	// This call adds another remote system to track
	m_variableDeltaSerializer.AddRemoteSystemVariableHistory(destinationConnection->GetRakNetGUID());

	//Generate our network ID
	//Get the network ID of our owner
	constructionBitstream->Write(GetNetworkID());
	constructionBitstream->Write(m_pOwnerEntity->GetNetworkID());
	
}

bool Component::DeserializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* sourceConnection)
{
	//If bitstream is empty then early out
	if (constructionBitstream->GetNumberOfUnreadBits() == 0) {
		return false;
	}

	//TODO - get the network ID of the object and create using GET_OBJECT_FROM_ID
	//Extact Data from Construction BitStream
	RakNet::NetworkID ourNetworkID;
	RakNet::NetworkID ownerNetworkID;
	constructionBitstream->Read(ourNetworkID);
	constructionBitstream->Read(ownerNetworkID);

	//Assign our owner based on ID
	m_pOwnerEntity = ServerClientBase::GetNetworkIDManager()->GET_OBJECT_FROM_ID<Entity*>(ownerNetworkID);
	m_pOwnerEntity->AddComponent(this);

	return true;
}

void Component::SerializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* destinationConnection)
{
	// variableDeltaSerializer is a helper class that tracks what variables were sent to what remote system
	// This call removes a remote system
	m_variableDeltaSerializer.RemoveRemoteSystemVariableHistory(destinationConnection->GetRakNetGUID());
}

bool Component::DeserializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* sourceConnection)
{
	//Return true to allow destruction
	return true;
}