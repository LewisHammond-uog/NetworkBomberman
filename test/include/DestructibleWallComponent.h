#ifndef __WALL_COMPONENT_H__
#define __WALL_COMPONENT_H__

//Project Includes
#include "Component.h"

class DestructibleWallComponent : public Component
{
public:

	//Constructor/Desctructor
	explicit DestructibleWallComponent(Entity* a_pOwnerEntity);
	~DestructibleWallComponent();

	//Update & Draw Functions
	void Update(float a_fDeltaTime) override {};
	void Draw(Shader* a_pShader) override {};

	void DestroyWall() const;

	RakNet::RakString GetName(void) const override { return RakNet::RakString("DestructableWallComponent"); }

};

#endif //__WALL_COMPONENT_H__

