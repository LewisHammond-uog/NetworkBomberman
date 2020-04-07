#include "stdafx.h"
#include "Entity.h"

//todo remove
#include "ServerClientBase.h"

//Typedefs
typedef std::pair<const unsigned int, Entity*> EntityPair;

//Initalise Statics
unsigned int Entity::s_uEntityCount = 0;
std::map<const unsigned int, Entity*> Entity::s_xEntityMap;

Entity::Entity()
{
	//Increment entity count and add to entity list
	m_uEntityID = s_uEntityCount++;
	s_xEntityMap.insert(EntityPair(m_uEntityID, this));

	//todo remove
	char buffer[128];
	sprintf(buffer, "Entity, ID: ", GetNetworkID());
	ServerClientBase::LogConsoleMessage(buffer);
}

Entity::~Entity()
{
	//Loop all of the components that this object owns and delete
	//them
	std::vector<Component*>::const_iterator xIter;
	for (xIter = m_apComponentList.begin(); xIter < m_apComponentList.end(); ++xIter) {
		delete *xIter;
	}

	//Remove this entity from the entity map and
	//reduce entity count
	//TODO - Check that the entity is in the list?
	s_xEntityMap.erase(m_uEntityID);
	--s_uEntityCount;
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

	//Debug Draw Primative
	Gizmos::addBox(glm::vec3(0.f), glm::vec3(1.f), true, glm::vec4(1.f));
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