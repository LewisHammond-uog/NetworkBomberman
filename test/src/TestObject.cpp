#include "stdafx.h"
#include "TestObject.h"

/// <summary>
/// Default Constructor
/// </summary>
TestObject::TestObject()
{
}

/// <summary>
/// Default Destructor
/// </summary>
TestObject::~TestObject()
{
}

void TestObject::WriteAllocationID(RakNet::Connection_RM3* destinationConnection, RakNet::BitStream* allocationIdBitstream) const
{
	allocationIdBitstream->Write(m_sObjName);
}

void TestObject::SerializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* destinationConnection)
{
	// variableDeltaSerializer is a helper class that tracks what variables were sent to what remote system
	// This call adds another remote system to track
	variableDeltaSerializer.AddRemoteSystemVariableHistory(destinationConnection->GetRakNetGUID());

	//Write Name of Object
	constructionBitstream->Write(m_sObjName);
	//Write Health of Object
	constructionBitstream->Write(m_fHealth);
}

RakNet::RM3ConstructionState TestObject::QueryConstruction(RakNet::Connection_RM3* destinationConnection, RakNet::ReplicaManager3* replicaManager3)
{
	//TODO CHANGE SO THAT NETWORK TOPOLIGY IS CORRECT - replace false
	return QueryConstruction_ServerConstruction(destinationConnection, false);
}

bool TestObject::QueryRemoteConstruction(RakNet::Connection_RM3* sourceConnection)
{
	//TODO CHANGE SO THAT NETWORK TOPOLIGY IS CORRECT - replace false
	return QueryRemoteConstruction_ServerConstruction(sourceConnection, false);
}

RakNet::RM3QuerySerializationResult TestObject::QuerySerialization(RakNet::Connection_RM3* destinationConnection)
{
	//TODO CHANGE SO THAT NETWORK TOPOLIGY IS CORRECT - replace false
	return QuerySerialization_ServerSerializable(destinationConnection, false);
}

RakNet::RM3ActionOnPopConnection TestObject::QueryActionOnPopConnection(RakNet::Connection_RM3* droppedConnection) const
{
	//Decide what to do when we lose a connection - this default deletes objects on 
	//connection lost
	return QueryActionOnPopConnection_Server(droppedConnection);
}

void TestObject::Deserialize(RakNet::DeserializeParameters* deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;

	//Deserialise the data
	variableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	variableDeltaSerializer.DeserializeVariable(&deserializationContext, m_sObjName);
	variableDeltaSerializer.DeserializeVariable(&deserializationContext, m_fHealth);

	variableDeltaSerializer.EndDeserialize(&deserializationContext);
}

RakNet::RM3SerializationResult TestObject::Serialize(RakNet::SerializeParameters* serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;

	//Write reliability type
	serializeParameters->pro[0].reliability = PacketReliability::RELIABLE;
	
	//Begin Serialization of the data that we are going to send
	variableDeltaSerializer.BeginIdenticalSerialize(
		&serializationContext,
		serializeParameters->whenLastSerialized == 0,
		&serializeParameters->outputBitstream[0]
	);

	//Serialize Variables
	variableDeltaSerializer.SerializeVariable(&serializationContext, m_sObjName);
	variableDeltaSerializer.SerializeVariable(&serializationContext, m_fHealth);

	//Return that we should always serialize
	return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void TestObject::DeallocReplica(RakNet::Connection_RM3* sourceConnection)
{
	//Simply Delete this object
	delete this;
}

bool TestObject::DeserializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* sourceConnection)
{
	//Return true to allow destruction
	return true;
}

void TestObject::SerializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* destinationConnection)
{
	// variableDeltaSerializer is a helper class that tracks what variables were sent to what remote system
	// This call removes a remote system
	variableDeltaSerializer.RemoveRemoteSystemVariableHistory(destinationConnection->GetRakNetGUID());
}

bool TestObject::DeserializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* sourceConnection)
{
	//If bitstream is empty then early out
	if (constructionBitstream->GetNumberOfUnreadBits() == 0) {
		return false;
	}

	//Extact Data from Construction BitStream
	constructionBitstream->Read(m_sObjName);
	constructionBitstream->Read(m_fHealth);

	return true;
}
