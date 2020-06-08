#ifndef __TEST_OBJECT_H__
#define __TEST_OBJECT_H__

//Raknet Includes
#include "ReplicaManager3.h"
#include "VariableDeltaSerializer.h"
#include "RakString.h"

/// <summary>
/// Abstract Class for Server Created Objects. Does not contain Serialize/Deserialize, Serialize/DeserializeConstructoion or Destruction
/// functions which are needed to the implemented in derived classes 
/// </summary>
class ServerCreatedObject : public RakNet::Replica3 {
protected:
	ServerCreatedObject();
	~ServerCreatedObject();
	
	virtual RakNet::RakString GetName(void) const { return RakNet::RakString("ServerCreatedObject"); }

	void WriteAllocationID(RakNet::Connection_RM3* destinationConnection, RakNet::BitStream* allocationIdBitstream) const override;
	
	//Serialise/Deserialise functions
	RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters* serializeParameters) override =0;
	void Deserialize(RakNet::DeserializeParameters* deserializeParameters) override =0;

	//Fuctions for deserialise and serialize on create
	void SerializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* destinationConnection) override =0;
	bool DeserializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* sourceConnection) override =0;
	void SerializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* destinationConnection) override =0;
	bool DeserializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* sourceConnection) override =0;

	//Dellocating Object
	void DeallocReplica(RakNet::Connection_RM3* sourceConnection) override;

	//Querying Remote Construction and Destruction - using ServerCreated_ServerSerialized
	RakNet::RM3ConstructionState QueryConstruction(RakNet::Connection_RM3* destinationConnection, RakNet::ReplicaManager3* replicaManager3) override;
	bool QueryRemoteConstruction(RakNet::Connection_RM3* sourceConnection) override;
	RakNet::RM3QuerySerializationResult QuerySerialization(RakNet::Connection_RM3* destinationConnection) override;
	
	//Function for when we lose a connection 
	RakNet::RM3ActionOnPopConnection QueryActionOnPopConnection(RakNet::Connection_RM3* droppedConnection) const override;

	//Saves and compares the variables of this serialise and the last serialize
	//call. If they are different then true is writen to the bitstream and the value,
	//otherwise false and no value
	RakNet::VariableDeltaSerializer m_variableDeltaSerializer;
};

#endif // !__TEST_OBJECT_H__