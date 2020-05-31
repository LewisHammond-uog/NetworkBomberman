#ifndef __WALL_COMPONENT_H__
#define __WALL_COMPONENT_H__

//Project Includes
#include "Component.h"

class WallComponent : public Component
{
public:

	//Constructor/Desctructor
	WallComponent(Entity* a_pOwnerEntity, bool a_bIsDestructible = false);
	~WallComponent();

	//Update & Draw Functions
	void Update(float a_fDeltaTime) override {};
	void Draw(Shader* a_pShader) override {};

	bool AttemptDestroyWall() const;
	
private:
	
	//If this wall can be destroyed by a bomb
	bool m_bIsDestructible = false;
	
};

#endif //__WALL_COMPONENT_H__

