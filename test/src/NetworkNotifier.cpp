#include "stdafx.h"
#include "NetworkNotifier.h"
#include "TestObject.h"


RakNet::Replica3* TestConnection::AllocReplica(RakNet::BitStream* allocationId, RakNet::ReplicaManager3* replicaManager3)
{
	return new TestObject;
	//return nullptr;
}

void TestConnection::DeallocReplica(RakNet::Connection_RM3* sourceConnection)
{
	delete sourceConnection;
}
