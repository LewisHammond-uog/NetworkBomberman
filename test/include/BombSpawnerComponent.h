#ifndef __BOMB_SPAWNER_COMPONENT_H__
#define __BOMB_SPAWNER_COMPONENT_H__

//Project Includes
#include "Component.h"

/// <summary>
/// Class to deal with spawning bombs when a key is pressed
/// </summary>
class BombSpawnerComponent : public Component
{
public:
	BombSpawnerComponent(Entity* a_pOwner);
	~BombSpawnerComponent() = default;

	virtual void Update(float a_fDeltaTime);
	virtual void Draw(Shader* a_pShader);

#pragma region Replica Manager Functions
	//Entity Name
	virtual RakNet::RakString GetName(void) const { return RakNet::RakString("BombSpawnerComponent"); }

#pragma endregion 

private:

	//Seperate Functions for Server and Client Updatey
	void ServerUpdateSpawner(float a_fDeltaTime);
	void ClientUpdateSpawner(float a_fDeltaTime);
	
	void SpawnBomb(glm::vec3 a_v3SpawnPosition);

	
	//Key for spawning bomb
	const int mc_iBombSpawnKey = GLFW_KEY_E;
	//Key State last frame so that we can't hold down and spawn lots of bombs
	bool m_bSpawnKeyPressedLastFrame;
};

#endif //!__BOMB_SPAWNER_COMPONENT_H__
