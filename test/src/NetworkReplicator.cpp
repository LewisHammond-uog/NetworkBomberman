#include "stdafx.h"
#include "NetworkReplicator.h"

#include "Entity.h"
#include "TransformComponent.h"


RakNet::Replica3* TestConnection::AllocReplica(RakNet::BitStream* allocationId, RakNet::ReplicaManager3* replicaManager3)
{
	RakNet::RakString typeName;
	allocationId->Read(typeName);

	if (typeName == "Entity") { return new Entity; }
	if (typeName == "TransformComponent") { return new TransformComponent; }

	//Default nullptr - don't create anything
	return nullptr;
}

void TestConnection::DeallocReplica(RakNet::Connection_RM3* sourceConnection)
{
	delete sourceConnection;
}
