#include "stdafx.h"
#include "NetworkReplicator.h"

//Project Includes
#include "Entity.h"
//Components
#include "BombComponent.h"
#include "BombSpawnerComponent.h"
#include "TransformComponent.h"
#include "SpherePrimitiveComponent.h"
#include "BoxPrimitiveComponent.h"
#include "PlayerControlComponent.h"
#include "PlayerDataComponent.h"


RakNet::Replica3* ClientConnection::AllocReplica(RakNet::BitStream* allocationId, RakNet::ReplicaManager3* replicaManager3)
{
	RakNet::RakString typeName;
	allocationId->Read(typeName);

	if (typeName == "Entity") { return new Entity; }
	if (typeName == "TransformComponent") { return new TransformComponent(nullptr); }
	if (typeName == "BoxPrimitiveComponent") { return new BoxPrimitiveComponent(nullptr); }
	if (typeName == "SpherePrimitiveComponent") { return new SpherePrimitiveComponent(nullptr); }
	if (typeName == "PlayerControlComponent") { return new PlayerControlComponent(nullptr); }
	if (typeName == "BombSpawnerComponent") { return new BombSpawnerComponent(nullptr); }
	if (typeName == "BombComponent") { return new BombComponent(nullptr); }
	if (typeName == "PlayerDataComponent") { return new PlayerDataComponent(nullptr, RakNet::RakNetGUID()); }
	
	//Default nullptr - don't create anything
	return nullptr;
}

void ClientConnection::DeallocReplica(RakNet::Connection_RM3* sourceConnection)
{
	delete sourceConnection;
}
