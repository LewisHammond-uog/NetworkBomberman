#ifndef __BOMB_COMPONENT_H__
#define __BOMB_COMPONENT_H__

//C++ Includes
#include <vector>

//Project Includes
#include "Component.h"

//React Physics Includes
#include "reactphysics3d.h"

/// <summary>
/// Class for dealing with a bomb in the world
/// Countdown to and explosion
/// </summary>
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

	//Function for getting collision rays
	std::vector<rp3d::Ray*> GetCollisionRays() const;

	//Time from creation to bomb explosion
	const float mc_fBombExplosionTime = 5.0f;

	//Range of the bomb
	const float mc_fBombRange = 5.0f;

	float m_fTimeSinceCreation;
	
};

#endif //!__BOMB_COMPONENT_H__