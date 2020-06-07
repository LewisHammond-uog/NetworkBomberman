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
#include "ColliderComponent.h"
#include "ConsoleLog.h"
#include "DestructableWallComponent.h"
#include "PlayerControlComponent.h"
#include "PlayerDataComponent.h"
#include "RaycastComponent.h"
#include "CylinderPrimitiveComponent.h"
#include "ServerClientBase.h"


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
	if (typeName == "CylinderPrimitiveComponent") { return new CylinderPrimitiveComponent(nullptr); }

	//RayCaster, Collision and Destructable Wall Components are not sent to the client as they do no processing there
	
	//Default nullptr - don't create anything
	return nullptr;
}

void ClientConnection::DeallocReplica(RakNet::Connection_RM3* sourceConnection)
{
	delete sourceConnection;
}
