//For ReplicaManager3 functions
// ReSharper disable CppInconsistentNaming
#include "stdafx.h"
#include "ServerCreatedObject.h"

//Test Includes
#include "TestProject.h"

//todo comments here

/// <summary>
/// Default Constructor
/// </summary>
ServerCreatedObject::ServerCreatedObject()
{

}

/// <summary>
/// Default Destructor
/// </summary>
ServerCreatedObject::~ServerCreatedObject()
{
}

void ServerCreatedObject::WriteAllocationID(RakNet::Connection_RM3* destinationConnection, RakNet::BitStream* allocationIdBitstream) const
{
	allocationIdBitstream->Write(GetName());
}

RakNet::RM3ConstructionState ServerCreatedObject::QueryConstruction(RakNet::Connection_RM3* destinationConnection, RakNet::ReplicaManager3* replicaManager3){
	
	return QueryConstruction_ServerConstruction(destinationConnection, TestProject::isServer);
}

bool ServerCreatedObject::QueryRemoteConstruction(RakNet::Connection_RM3* sourceConnection)
{
	return QueryRemoteConstruction_ServerConstruction(sourceConnection, TestProject::isServer);
}

RakNet::RM3QuerySerializationResult ServerCreatedObject::QuerySerialization(RakNet::Connection_RM3* destinationConnection)
{
	return QuerySerialization_ClientSerializable(destinationConnection, TestProject::isServer);
}

RakNet::RM3ActionOnPopConnection ServerCreatedObject::QueryActionOnPopConnection(RakNet::Connection_RM3* droppedConnection) const
{
	//Decide what to do when we lose a connection - this default deletes objects on 
	//connection lost
	return QueryActionOnPopConnection_Server(droppedConnection);
}


void ServerCreatedObject::DeallocReplica(RakNet::Connection_RM3* sourceConnection)
{
	//Simply Delete this object
	delete this;
}


