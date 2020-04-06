#ifndef __NETWORK_NOTIFIER_H__
#define __NETWORK_NOTIFIER_H__

//C++ Includes
#include <vector>

//RakIncludes
#include "BitStream.h"
#include "ReplicaManager3.h"

class TestConnection : public RakNet::Connection_RM3 {
public:
	TestConnection(const RakNet::SystemAddress& _systemAddress, RakNet::RakNetGUID _guid) : Connection_RM3(_systemAddress, _guid) {}
	virtual ~TestConnection() {}

	virtual RakNet::Replica3* AllocReplica(RakNet::BitStream* allocationId, RakNet::ReplicaManager3* replicaManager3);
	virtual void DeallocReplica(RakNet::Connection_RM3* sourceConnection);

};

class NetworkReplicator : public RakNet::ReplicaManager3
{
public:
	virtual RakNet::Connection_RM3* AllocConnection(const RakNet::SystemAddress& systemAddress, RakNet::RakNetGUID rakNetGUID) const {
		return new TestConnection(systemAddress, rakNetGUID);
	}
	virtual void DeallocConnection(RakNet::Connection_RM3* connection) const {
		delete connection;
	}
};

#endif // !__NETWORK_NOTIFIER_H__
