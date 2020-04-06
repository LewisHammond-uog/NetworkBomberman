#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

//Project Includes
#include "Component.h"

enum MATRIX_ROW 
{
	RIGHT_VECTOR,
	UP_VECTOR,
	FORWARD_VECTOR,
	POSTION_VECTOR
};

class TransformComponent : public Component
{
public:
	TransformComponent();//TODO remove - this is temp to statisfy AllocReplica
	TransformComponent(Entity* a_pOwner);	
	virtual ~TransformComponent();

	virtual void Update(float a_fDeltaTime) {};
	virtual void Draw(Shader* a_pShader) {};

	void Orthogonalize();

	//Get and set Matrix Row
	const glm::mat4& GetEntityMatrix() { return m_m4EntityMatrix; }
	void SetEntityMatrixRow(MATRIX_ROW a_eRow, glm::vec3 a_v3Vec);
	glm::vec3 GetEntityMatrixRow(MATRIX_ROW a_eRow);
	glm::vec3 GetCurrentPosition();

	virtual RakNet::RakString GetName(void) const { return RakNet::RakString("TransformComponent"); }

private:
	glm::mat4 m_m4EntityMatrix;
};

#endif // ! __TRANSFORM_COMPONENT_H__
