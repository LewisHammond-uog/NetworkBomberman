#ifndef __TEST_OBJECT_H__
#define __TEST_OBJECT_H__

#include "RakPeerInterface.h"
#include "ReplicaManager3.h"
#include "ReplicaEnums.h"
#include "VariableDeltaSerializer.h"
#include "BitStream.h"
#include "RakString.h"

#include <string.h>

class TestObject : public RakNet::Replica3 {
public:
	TestObject();
	~TestObject();

	virtual void WriteAllocationID(RakNet::Connection_RM3* destinationConnection, RakNet::BitStream* allocationIdBitstream) const;
	
	//Serialise/Deserialise functions
	virtual RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters* serializeParameters);
	virtual void Deserialize(RakNet::DeserializeParameters* deserializeParameters);

	//Fuctions for deserialise and serialize on create
	virtual void SerializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* destinationConnection);
	virtual bool DeserializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* sourceConnection);
	virtual void SerializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* destinationConnection);
	virtual bool DeserializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* sourceConnection);

	//Dellocating Object
	virtual void DeallocReplica(RakNet::Connection_RM3* sourceConnection);

	//Querying Remote Construction and Destruction - using ServerCreated_ServerSerialized
	virtual RakNet::RM3ConstructionState QueryConstruction(RakNet::Connection_RM3* destinationConnection, RakNet::ReplicaManager3* replicaManager3);
	virtual bool QueryRemoteConstruction(RakNet::Connection_RM3* sourceConnection);
	virtual RakNet::RM3QuerySerializationResult QuerySerialization(RakNet::Connection_RM3* destinationConnection);
	
	//Function for when we lose a connection 
	virtual RakNet::RM3ActionOnPopConnection QueryActionOnPopConnection(RakNet::Connection_RM3* droppedConnection) const;

	const int m_sObjName = 9;
	float m_fHealth = 22.f;

	void UpdateHealth();

	//Saves and compares the variables of this serialise and the last serialize
	//call. If they are different then true is writen to the bitstream and the value,
	//otherwise false and no value
	RakNet::VariableDeltaSerializer variableDeltaSerializer;
};

#endif // !__TEST_OBJECT_H__