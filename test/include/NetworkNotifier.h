#ifndef __NETWORK_NOTIFIER_H__
#define __NETWORK_NOTIFIER_H__

//C++ Includes
#include <vector>
#include <type_traits>

//RakIncludes
#include "NetworkIDObject.h"
#include "BitStream.h"

//Project Includes
#include "ClientServerBase.h"
#include "PacketTypes.h"

#include "ReplicaManager3.h"

class TestConnection : public RakNet::Connection_RM3 {
public:
	virtual RakNet::Replica3* AllocReplica(RakNet::BitStream* allocationId, RakNet::ReplicaManager3* replicaManager3);
	virtual void DeallocReplica(RakNet::Connection_RM3* sourceConnection);

};

class NetworkReplicator : public RakNet::ReplicaManager3
{
public:
private:
};

#endif // !__NETWORK_NOTIFIER_H__
