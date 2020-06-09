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

/// <summary>
/// Class for an entity within the world that has components
/// attached
/// </summary>
class Entity : public ServerCreatedObject
{
public:
	Entity();
	~Entity();

	virtual void Update(float a_fDeltaTime);
	virtual void Draw(Shader* a_pShader);

	bool IsEnabled() const;
	void SetEnabled(bool a_bEnabled);
	
	//Add/get components
	void AddComponent(Component* a_pComponent);
	void RemoveComponent(Component* a_pComponentToRemove, bool a_bDeleteComponent = false);
	template<class returnType>
	returnType GetComponent() const; 	//Function for getting a component of this entity based on it's return type

	//Get the ID of this entity
	unsigned int GetEntityID() const { return m_uEntityID; }
	//Get all of the entities that we have created
	static const std::map<const unsigned int, Entity*>& GetEntityMap() { return s_xEntityMap; }


#pragma region Replica Manager Functions
	//Entity Name
	RakNet::RakString GetName(void) const override { return RakNet::RakString("Entity"); }
	
	//Serialise/Deserialise functions
	RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters* serializeParameters) override;
	void Deserialize(RakNet::DeserializeParameters* deserializeParameters) override;

	//Fuctions for deserialise and serialize on create
	void SerializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* destinationConnection) override;
	bool DeserializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* sourceConnection) override;
	void SerializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* destinationConnection) override;
	bool DeserializeDestruction(RakNet::BitStream* destructionBitstream, RakNet::Connection_RM3* sourceConnection) override;
#pragma  endregion 

private:
	//Info about this entity
	unsigned int m_uEntityID;
	std::vector<Component*> m_apComponentList;

	bool m_bEnabled;
	
	//Statics for all entities
	static unsigned int s_uEntityCount;
	static std::map<const unsigned int, Entity*> s_xEntityMap;
};


/// <summary>
/// Template Function that returns a pointer to a component, based on the
/// template argument (returnType), for example GetComponent<TransformComponent*>() will return
/// a pointer to the transform component of the calling entity.
/// Template Type must be a a Component* (e.g TransformComponent*, ModelComponent*)
/// Will return nullptr if there is not a component of returnType attached to this entity 
/// </summary>
/// <returns>Pointer to the component of the returnType attached to this entity or
/// nullptr if one is not attached</returns>
template<class returnType>
returnType Entity::GetComponent() const
{
	//Loop through all of the components that we have,
	//try and cast each one to the returnType, if we can cast to it
	//then it is a component of the type that we are looking to return, so return it.
	//If we go through all of the components that we have and have not succesfully
	//cast then we do not have a component of returnType, so return nullptr.
	std::vector<Component*>::const_iterator xIter;
	if (!m_apComponentList.empty()) {
		for (xIter = m_apComponentList.begin(); xIter != m_apComponentList.end(); ++xIter)
		{
			returnType pComponent = dynamic_cast<returnType>(*xIter);
			if (pComponent != nullptr)
			{
				return pComponent;
			}
		}
	}

	return nullptr;
}

#endif // ! __ENTITY_H__
