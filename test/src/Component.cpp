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
	//todo remove? - don't think we need this because we are a replica object and this is pre assigned by s_pReplicaManager->SetNetworkIDManager(GetNetworkIDManager()); in Client Server Base
	//Set our network manager
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

	/*
	 * CONSTRUCTION DATA LAYOUT
	 * NetworkID* OwnerNetworkID
	 */
	
	//Generate our network ID
	//Get the network ID of our owner
	constructionBitstream->Write(m_pOwnerEntity->GetNetworkID());
	
}

bool Component::DeserializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* sourceConnection)
{
	//If bitstream is empty then early out
	if (constructionBitstream->GetNumberOfUnreadBits() == 0) {
		return false;
	}

	/*
	 * CONSTRUCTION DATA LAYOUT
	 * NetworkID* OwnerNetworkID
	 */
	
	//Get the network ID of the entity that owns this component
	RakNet::NetworkID ownerNetworkID;
	constructionBitstream->Read(ownerNetworkID);

	//Get our owner entity based on the owner network ID that we were given from the server, then add this component
	//to our owner entity (as long as it's not nullptr), so that our entity and component are linked as they are
	//on the server
	m_pOwnerEntity = ServerClientBase::GetNetworkIDManager()->GET_OBJECT_FROM_ID<Entity*>(ownerNetworkID);
	if (m_pOwnerEntity != nullptr) {
		m_pOwnerEntity->AddComponent(this);
	}

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