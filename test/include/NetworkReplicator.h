#ifndef __NETWORK_NOTIFIER_H__
#define __NETWORK_NOTIFIER_H__

//C++ Includes
#include <vector>

//RakIncludes
#include "BitStream.h"
#include "ReplicaManager3.h"

class ClientConnection : public RakNet::Connection_RM3 {
public:
	ClientConnection(const RakNet::SystemAddress& _systemAddress, RakNet::RakNetGUID _guid) : Connection_RM3(_systemAddress, _guid) {}
	virtual ~ClientConnection() {}

	RakNet::Replica3* AllocReplica(RakNet::BitStream* allocationId, RakNet::ReplicaManager3* replicaManager3) override;
	virtual void DeallocReplica(RakNet::Connection_RM3* sourceConnection);

};

class NetworkReplicator final : public RakNet::ReplicaManager3
{
public:
	RakNet::Connection_RM3* AllocConnection(const RakNet::SystemAddress& systemAddress, RakNet::RakNetGUID rakNetGUID) const override
	{
		return new ClientConnection(systemAddress, rakNetGUID);
	}

	void DeallocConnection(RakNet::Connection_RM3* connection) const override
	{
		delete connection;
	}
};

#endif // !__NETWORK_NOTIFIER_H__
