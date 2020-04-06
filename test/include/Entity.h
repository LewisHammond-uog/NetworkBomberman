#ifndef __ENTITY_H__
#define __ENTITY_H__

//std includes
#include <vector>
#include <map>

//Rak Includes
#include <ReplicaManager3.h>

//Project Includes
#include "Component.h"
#include "ServerCreatedObject.h"

//Forward Declare
class Shader;

class Entity : public ServerCreatedObject
{
public:
	Entity();
	~Entity();



	virtual void Update(float a_fDeltaTime);
	virtual void Draw(Shader* a_pShader);

	//Add/get components
	void AddComponent(Component* a_pComponent);
	Component* GetComponent(COMPONENT_TYPE a_eComponentType) const;

	//Get the ID of this entity
	unsigned int GetEntityID() const { return m_uEntityID; }
	//Get all of the entities that we have created
	static const std::map<const unsigned int, Entity*>& GetEntityMap() { return s_xEntityMap; }


#pragma region Replica Manager Functions
	//Entity Name
	virtual RakNet::RakString GetName(void) const { return RakNet::RakString("Entity"); }
	
	//Serialise/Deserialise functions
	virtual RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters* serializeParameters);
	virtual void Deserialize(RakNet::DeserializeParameters* deserializeParameters);

	//Fuctions for deserialise and serialize on create
	virtual void SerializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* destinationConnection);
	virtual bool DeserializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* sourceConnection);
	virtual void SerializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* destinationConnection);
	virtual bool DeserializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* sourceConnection);
#pragma  endregion 

private:
	unsigned int m_uEntityID;
	std::vector<Component*> m_apComponentList;


	static unsigned int s_uEntityCount;
	static std::map<const unsigned int, Entity*> s_xEntityMap;
};

#endif // ! __ENTITY_H__
