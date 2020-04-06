#ifndef __TEST_OBJECT_H__
#define __TEST_OBJECT_H__

#include "RakPeerInterface.h"
#include "ReplicaManager3.h"
#include "ReplicaEnums.h"
#include "VariableDeltaSerializer.h"
#include "BitStream.h"
#include "RakString.h"

#include <string.h>

/// <summary>
/// Abstract Class for Server Created Objects. Does not contain Serialize/Deserialize, Serialize/DeserializeConstructoion or Destruction
/// functions which are needed to the implemented in derived classes 
/// </summary>
class ServerCreatedObject : public RakNet::Replica3 {
protected:
	ServerCreatedObject();
	~ServerCreatedObject();
	
	virtual RakNet::RakString GetName(void) const { return RakNet::RakString("ServerCreatedObject"); }

	virtual void WriteAllocationID(RakNet::Connection_RM3* destinationConnection, RakNet::BitStream* allocationIdBitstream) const;
	
	//Serialise/Deserialise functions
	virtual RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters* serializeParameters)=0;
	virtual void Deserialize(RakNet::DeserializeParameters* deserializeParameters)=0;

	//Fuctions for deserialise and serialize on create
	virtual void SerializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* destinationConnection)=0;
	virtual bool DeserializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* sourceConnection)=0;
	virtual void SerializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* destinationConnection)=0;
	virtual bool DeserializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* sourceConnection)=0;

	//Dellocating Object
	virtual void DeallocReplica(RakNet::Connection_RM3* sourceConnection);

	//Querying Remote Construction and Destruction - using ServerCreated_ServerSerialized
	virtual RakNet::RM3ConstructionState QueryConstruction(RakNet::Connection_RM3* destinationConnection, RakNet::ReplicaManager3* replicaManager3);
	virtual bool QueryRemoteConstruction(RakNet::Connection_RM3* sourceConnection);
	virtual RakNet::RM3QuerySerializationResult QuerySerialization(RakNet::Connection_RM3* destinationConnection);
	
	//Function for when we lose a connection 
	virtual RakNet::RM3ActionOnPopConnection QueryActionOnPopConnection(RakNet::Connection_RM3* droppedConnection) const;

	//Saves and compares the variables of this serialise and the last serialize
	//call. If they are different then true is writen to the bitstream and the value,
	//otherwise false and no value
	RakNet::VariableDeltaSerializer m_variableDeltaSerializer;
};

#endif // !__TEST_OBJECT_H__