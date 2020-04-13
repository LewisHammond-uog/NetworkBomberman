#include "stdafx.h"
#include "NetworkReplicator.h"

#include "Entity.h"
#include "TransformComponent.h"
#include "SpherePrimitiveComponent.h"
#include "BoxPrimitiveComponent.h"


RakNet::Replica3* TestConnection::AllocReplica(RakNet::BitStream* allocationId, RakNet::ReplicaManager3* replicaManager3)
{
	RakNet::RakString typeName;
	allocationId->Read(typeName);

	if (typeName == "Entity") { return new Entity; }
	if (typeName == "TransformComponent") { return new TransformComponent(nullptr); }
	if (typeName == "BoxPrimitiveComponent") { return new BoxPrimitiveComponent(nullptr); }
	if (typeName == "SpherePrimitiveComponent") { return new SpherePrimitiveComponent(nullptr); }

	
	//Default nullptr - don't create anything
	return nullptr;
}

void TestConnection::DeallocReplica(RakNet::Connection_RM3* sourceConnection)
{
	delete sourceConnection;
}
