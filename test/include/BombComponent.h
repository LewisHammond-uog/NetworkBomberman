#ifndef __BOMB_COMPONENT_H__
#define __BOMB_COMPONENT_H__

//Project Includes
#include "Component.h"

class BombComponent : public Component
{
public:
	BombComponent(Entity* a_pOwner);
	~BombComponent() = default;

	virtual void Update(float a_fDeltaTime);
	virtual void Draw(Shader* a_pShader);

#pragma region Replica Manager Functions
	//Entity Name
	virtual RakNet::RakString GetName(void) const { return RakNet::RakString("BombComponent"); }

#pragma endregion 

private:

	void ExplodeBomb() const;
	
	float mc_fBombExplosionTime = 5.0f;
	float m_fTimeSinceCreation;
	
};

#endif //!__BOMB_COMPONENT_H__