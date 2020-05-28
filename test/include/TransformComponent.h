#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

//Project Includes
#include "Component.h"

//Forward Declare
class TransformHistory;
namespace RakNet {
	class Connection_RM3;
}

enum class MATRIX_ROW 
{
	RIGHT_VECTOR,
	UP_VECTOR,
	FORWARD_VECTOR,
	POSTION_VECTOR
};

/// <summary>
/// Class for a transform with a 4x4 Matrix
/// </summary>
class TransformComponent final : public Component
{
public:
	TransformComponent(Entity* a_pOwner);	
	virtual ~TransformComponent();

	void Update(float a_fDeltaTime) override;
	virtual void Draw(Shader* a_pShader) {}

	void Orthogonalize();

	//Get and set Matrix Row
	const glm::mat4& GetEntityMatrix() const { return m_m4EntityMatrix; }
	void SetEntityMatrixRow(MATRIX_ROW a_eRow, glm::vec3 a_v3Vec);
	glm::vec3 GetEntityMatrixRow(MATRIX_ROW a_eRow);
	glm::vec3 GetCurrentPosition();

	//Transform history used by the client to interpolate between server transfrom updates
	const RakNet::TimeMS m_fMaxWriteInterval = 30.f; //Time between writes for transform updates
	const RakNet::TimeMS m_fMaxHistoryTime = 3000.f; //Max time to keep the transform history for
	TransformHistory* m_pTransformHistory;

#pragma region RakNet Functions
	//RakNet Functions
	RakNet::RakString GetName(void) const override { return RakNet::RakString("TransformComponent"); }
	RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters* serializeParameters) override;
	void Deserialize(RakNet::DeserializeParameters* deserializeParameters) override;
	void SerializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* destinationConnection) override;
	bool DeserializeConstruction(RakNet::BitStream* constructionBitstream, RakNet::Connection_RM3* sourceConnection) override;
#pragma endregion

private:
	glm::mat4 m_m4EntityMatrix;
};

#endif // ! __TRANSFORM_COMPONENT_H__
