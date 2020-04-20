#include "stdafx.h"
#include "Entity.h"

//todo remove
#include "ServerClientBase.h"
#include "TestProject.h"

//Typedefs
typedef std::pair<const unsigned int, Entity*> EntityPair;

//Initalise Statics
unsigned int Entity::s_uEntityCount = 0;
std::map<const unsigned int, Entity*> Entity::s_xEntityMap;

Entity::Entity()
{
	//todo remove? - don't think we need this because we are a replica object and this is pre assigned by s_pReplicaManager->SetNetworkIDManager(GetNetworkIDManager()); in Client Server Base
	//Set our Network ID Manager
	SetNetworkIDManager(ServerClientBase::GetNetworkIDManager());
	
	//Increment entity count and add to entity list
	m_uEntityID = s_uEntityCount++;
	s_xEntityMap.insert(EntityPair(m_uEntityID, this));
}

Entity::~Entity()
{	
	//Loop all of the components that this object owns and remove them
	std::vector<Component*>::const_iterator xIter;
	for (xIter = m_apComponentList.begin(); xIter < m_apComponentList.end(); ++xIter) {
		delete* xIter;
	}
	m_apComponentList.clear();

	//Remove this entity from the entity map and
	//reduce entity count
	//TODO - Check that the entity is in the list?
	s_xEntityMap.erase(m_uEntityID);
	--s_uEntityCount;

	//Broadcast we have been destroyed
	//RakNet::UNASSIGNED_SYSTEM_ADDRESS means we exclude no systems
	ServerClientBase::GetNetworkReplicator()->BroadcastDestruction(this, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
}

///Update this entity
void Entity::Update(float a_fDeltaTime)
{
	//Loop through all of the components and update
	std::vector<Component*>::iterator xIter;
	for (xIter = m_apComponentList.begin(); xIter < m_apComponentList.end(); ++xIter)
	{
		Component* pComponent = *xIter;
		if (pComponent) {
			pComponent->Update(a_fDeltaTime);
		}
	}
	
}

///Draw all elements of this entity
void Entity::Draw(Shader* a_pShader)
{
	//Loop through all of the components and draw
	std::vector<Component*>::const_iterator xIter;
	for (xIter = m_apComponentList.begin(); xIter < m_apComponentList.end(); ++xIter)
	{
		Component* pComponent = *xIter;
		if (pComponent) {
			pComponent->Draw(a_pShader);
		}
	}
}

///Add a component to this entity
void Entity::AddComponent(Component* a_pComponent)
{
	//If we already have a component of the type we are trying
	//to add then early out
	if (GetComponent(a_pComponent->GetComponentType()) != nullptr) {
		return;
	}

	//Add component to our component list
	m_apComponentList.push_back(a_pComponent);
}

/// <summary>
/// Remove a component from this entity 
/// </summary>
/// <param name="a_pComponentToRemove">Component to Remove</param>
void Entity::RemoveComponent(Component* a_pComponentToRemove)
{
	//Loop through all of the components and check to see if we
	//have the component if we do the remove it
	std::vector<Component*>::const_iterator xIter;
	for (xIter = m_apComponentList.begin(); xIter < m_apComponentList.end(); ++xIter)
	{
		Component* pComponent = *xIter;
		if (pComponent == a_pComponentToRemove) {
			xIter = m_apComponentList.erase(xIter);
			break;
		}
	}
}

Component* Entity::GetComponent(COMPONENT_TYPE a_eComponentType) const
{
	//Loop through all of the components see if they have a component
	std::vector<Component*>::const_iterator xIter;
	for (xIter = m_apComponentList.begin(); xIter < m_apComponentList.end(); ++xIter)
	{
		Component* pComponent = *xIter;
		if (pComponent && pComponent->GetComponentType() == a_eComponentType) {
			return pComponent;
		}
	}

	//No component found, nullptr
	return nullptr;

}

RakNet::RM3SerializationResult Entity::Serialize(RakNet::SerializeParameters* serializeParameters)
{
	RakNet::VariableDeltaSerializer::SerializationContext serializationContext;

	//Write reliability type
	serializeParameters->pro[0].reliability = PacketReliability::RELIABLE;

	//Begin Serialization of the data that we are going to send
	m_variableDeltaSerializer.BeginIdenticalSerialize(
		&serializationContext,
		serializeParameters->whenLastSerialized == 0,
		&serializeParameters->outputBitstream[0]
	);

	//Serialize Variables
	//m_variableDeltaSerializer.SerializeVariable(&serializationContext, 0);
	//m_variableDeltaSerializer.SerializeVariable(&serializationContext, 0);

	//Return that we should always serialize
	return RakNet::RM3SR_SERIALIZED_ALWAYS;
}

void Entity::Deserialize(RakNet::DeserializeParameters* deserializeParameters)
{
	RakNet::VariableDeltaSerializer::DeserializationContext deserializationContext;

	//Deserialise the data
	m_variableDeltaSerializer.BeginDeserialize(&deserializationContext, &deserializeParameters->serializationBitstream[0]);
	//m_variableDeltaSerializer.DeserializeVariable(&deserializationContext, m_sObjName);
	//m_variableDeltaSerializer.DeserializeVariable(&deserializationContext, m_fHealth);


	m_variableDeltaSerializer.EndDeserialize(&deserializationContext);
}

void Entity::SerializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* destinationConnection)
{
	// variableDeltaSerializer is a helper class that tracks what variables were sent to what remote system
	// This call adds another remote system to track
	m_variableDeltaSerializer.AddRemoteSystemVariableHistory(destinationConnection->GetRakNetGUID());

	//Write Name of Object
	//constructionBitstream->Write(m_sObjName);
	//Write Health of Object
	//constructionBitstream->Write(m_fHealth);
}

bool Entity::DeserializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* sourceConnection)
{
	//If bitstream is empty then early out
	if (constructionBitstream->GetNumberOfUnreadBits() == 0) {
		return false;
	}

	//Extact Data from Construction BitStream
	//constructionBitstream->Read(m_sObjName);
	//constructionBitstream->Read(m_fHealth);


	return true;
}

void Entity::SerializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* destinationConnection)
{
	// variableDeltaSerializer is a helper class that tracks what variables were sent to what remote system
	// This call removes a remote system
	m_variableDeltaSerializer.RemoveRemoteSystemVariableHistory(destinationConnection->GetRakNetGUID());
}

bool Entity::DeserializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* sourceConnection)
{
	//Return true to allow destruction
	return true;
}