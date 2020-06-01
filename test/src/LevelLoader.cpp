#include "stdafx.h"
#include "LevelLoader.h"

//C++ Include
#include <fstream>
#include <iostream>


//Project Includes
#include "ConsoleLog.h"
#include "Entity.h"
#include "BoxPrimitiveComponent.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "DestructableWallComponent.h"
#include "NetworkServer.h"

/// <summary>
/// Loads a level of a given name
/// </summary>
/// <param name="a_sLevelName">Name of the level to be loaded</param>
void LevelLoader::LoadLevel(const std::string& a_sLevelName)
{
	//Find the level file
	const std::string sLevelFileName = m_szLevelLoadPath + a_sLevelName + ".txt";

	//Attempt to open the level files, if fail then exit out and present error
	std::fstream sLevelFile = std::fstream(sLevelFileName, std::ios_base::in);
	if(!sLevelFile.is_open())
	{
		ConsoleLog::LogError("[ERROR] Could not open level file. Does not exist or is already open");
		return;
	}

	//Load Level Data
	/*-----FILE LAYOUT-----
	 *LevelName
	 *Level Width (X)
	 *Level Height (Y)
	 *Level Array
	*/
	
	//Level Info
	std::string lvlName;
	int lvlWidth = 0;
	int lvlHeight = 0;
	sLevelFile >> lvlName >> lvlWidth >> lvlHeight;

	//Create Level Struct
	Level loadingLevel(lvlName, glm::vec2(lvlWidth, lvlHeight));

	//Load in the level data from file in to the data array.
	//Spawn the approprite object 
	for(int y = 0; y < lvlHeight; ++y)
	{
		for(int x = 0; x < lvlWidth; ++x)
		{
			//Load in to array
			sLevelFile >> loadingLevel.m_aiLevelData[x][y];

			//Load the object
			LEVEL_OBJECT lvlObjectType = static_cast<LEVEL_OBJECT>(loadingLevel.m_aiLevelData[x][y]);

			//Calculate Position to create object
			glm::vec3 v3ObjPos = m_v3StartPos + glm::vec3(x * m_fLevelSpacing, m_fLevelZ, y * m_fLevelSpacing);

			switch (lvlObjectType) {
				case LEVEL_OBJECT::LEVEL_OBJECT_EMPTY:
					//No Object Required
					break;
				case LEVEL_OBJECT::LEVEL_OBJECT_SOLID_WALL:
					//Spawn Solid Wall
					loadingLevel.m_vpLevelEntities.push_back(SpawnSolidWall(v3ObjPos));
					break;
				case LEVEL_OBJECT::LEVEL_OBJECT_DESTRUCT_WALL:
					//Spawn Destructable Wall
					loadingLevel.m_vpLevelEntities.push_back(SpawnDestructibleWall(v3ObjPos));
					break;
				default:
					//Error, not a valid level object
					ConsoleLog::LogError("[ERROR] Invalid Level Object Read from file. Aborting Load");
					return;
			}
		}
	}

	//Close the file
	sLevelFile.flush();
	sLevelFile.close();
}

/// <summary>
/// Spawn a destructable wall in the world
/// </summary>
/// <param name="a_v3SpawnPos">Position to spawn the wall at</param>
/// <returns>The Wall entity that was created</returns>
Entity* LevelLoader::SpawnDestructibleWall(glm::vec3 a_v3SpawnPos) const
{
	//Create a solid wall and add a destructable wall component
	Entity* pWallEntity = SpawnSolidWall(a_v3SpawnPos);
	DestructableWallComponent* pWallDestruction = new DestructableWallComponent(pWallEntity);
	pWallEntity->AddComponent(pWallDestruction);

	//Set the colour of the wall
	BoxPrimitiveComponent* pBoxPrimative = dynamic_cast<BoxPrimitiveComponent*>(pWallEntity->GetComponent(COMPONENT_TYPE::PRIMITIVE_BOX));
	if(pBoxPrimative)
	{
		pBoxPrimative->SetColour(m_v4DestructableColor);
	}

	//Send to clients
	NetworkReplicator* pReplicator = ServerClientBase::GetNetworkReplicator();
	pReplicator->Reference(pWallDestruction);
	
	return pWallEntity;
}

/// <summary>
/// Spawn a solid (non destructable) wall in the world
/// </summary>
/// <param name="a_v3SpawnPos">Position to spawn the wall at</param>
/// <returns>The Wall entity that was created</returns>
Entity* LevelLoader::SpawnSolidWall(glm::vec3 a_v3SpawnPos) const
{
	//Size of the wall block
	glm::vec3 v3WallSize = glm::vec3(m_fLevelCubeSize, m_fLevelCubeSize, m_fLevelCubeSize);
	
	//Create a wall entity which has a transform, collider and box
	Entity* pWallEntity = new Entity();
	TransformComponent* pWallTransform = new TransformComponent(pWallEntity);
	ColliderComponent* pWallCollider = new ColliderComponent(pWallEntity);
	BoxPrimitiveComponent* pBox = new BoxPrimitiveComponent(pWallEntity, v3WallSize);

	//Set the position of the level wall to be the given position
	pWallTransform->SetEntityMatrixRow(MATRIX_ROW::POSTION_VECTOR, a_v3SpawnPos);
	//Add Box Collider to collider component
	pWallCollider->AddBoxCollider(v3WallSize, glm::vec3(0, 0, 0));
	//Set the non destructable colour
	pBox->SetColour(m_v4SolidColor);

	
	//Add Components to Wall
	pWallEntity->AddComponent(pWallTransform);
	pWallEntity->AddComponent(pWallCollider);
	pWallEntity->AddComponent(pBox);


	//Send to clients
	NetworkReplicator* pReplicator = ServerClientBase::GetNetworkReplicator();
	pReplicator->Reference(pWallEntity);
	pReplicator->Reference(pWallTransform);
	pReplicator->Reference(pWallCollider);
	pReplicator->Reference(pBox);

	return pWallEntity;
}
