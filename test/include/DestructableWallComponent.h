#ifndef __WALL_COMPONENT_H__
#define __WALL_COMPONENT_H__

//Project Includes
#include "Component.h"

class DestructableWallComponent : public Component
{
public:

	//Constructor/Desctructor
	DestructableWallComponent(Entity* a_pOwnerEntity);
	~DestructableWallComponent();

	//Update & Draw Functions
	void Update(float a_fDeltaTime) override {};
	void Draw(Shader* a_pShader) override {};

	bool DestroyWall() const;

	virtual RakNet::RakString GetName(void) const { return RakNet::RakString("DestructableWallComponent"); }
	
private:

	
};

#endif //__WALL_COMPONENT_H__

