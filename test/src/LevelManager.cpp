#include "stdafx.h"
#include "LevelManager.h"

//C++ Include
#include <fstream>
#include <filesystem>

//Project Includes
#include "ConsoleLog.h"
#include "Entity.h"
#include "BoxPrimitiveComponent.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "DestructibleWallComponent.h"
#include "NetworkServer.h"
#include "Level.h"

//Static Initalisations
Level* LevelManager::s_pCurrentLevel = nullptr;
const char* LevelManager::s_szLevelLoadPath = "Levels/";

LevelManager::LevelManager() :
	m_iCurrentLevelIndex(-1)
{
}

/// <summary>
/// Delete the Level loader, unloading the level
/// </summary>
LevelManager::~LevelManager()
{
	//Unload the current level
	if (s_pCurrentLevel != nullptr) {
		UnloadLevel();
	}
}

/// <summary>
/// Sets the level map rotations
/// </summary>
/// <param name="a_vsLevelRotationVector">Vector of Level Names to use as the level rotation</param>
void LevelManager::SetLevelRotation(const std::vector<std::string> a_vsLevelRotationVector)
{
	m_vsLevelRotation = a_vsLevelRotationVector;
}

/// <summary>
/// Loads a level of a given name
/// </summary>
void LevelManager::LoadNextLevel()
{
	//If a level is already loaded then unload it
	if(s_pCurrentLevel != nullptr)
	{
		UnloadLevel();
	}

	//Check that we have a level rotation
	if(m_vsLevelRotation.empty())
	{
		ConsoleLog::LogError("[ERROR] No Level Rotation found cannot load the next level!");
		return;
	}
	
	//Find our next level file, if the level index is over the bounds of the vector
	//then reset to the start of our rotation
	++m_iCurrentLevelIndex;
	if(m_iCurrentLevelIndex >= m_vsLevelRotation.size() || m_iCurrentLevelIndex < 0)
	{
		m_iCurrentLevelIndex = 0;
	}

	//Get file name from level name
	std::string sCurrentLevelName =  m_vsLevelRotation[m_iCurrentLevelIndex];
	const std::string sLevelFileName = s_szLevelLoadPath + sCurrentLevelName + ".txt";

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
	sLevelFile >> lvlName >> lvlWidth >> lvlHeight; //Read in Level Info

	//Create Level Struct
	s_pCurrentLevel = new Level(lvlName, glm::vec2(lvlWidth, lvlHeight));

	//Load in the level data from file in to the data array.
	//Spawn the approprite object 
	for(int y = 0; y < lvlHeight; ++y)
	{
		for(int x = 0; x < lvlWidth; ++x)
		{
			//Load in the object we need to create
			int iLvlObjectID = 0;
			sLevelFile >> iLvlObjectID;
			LEVEL_OBJECT lvlObjectType = static_cast<LEVEL_OBJECT>(iLvlObjectID);

			//Calculate Position to create object
			const float fLevelSpacing = Level::sc_fLevelSpacing;
			glm::vec3 v3ObjPos = glm::vec3(x * fLevelSpacing, Level::sc_fLevelY, y * fLevelSpacing);

			switch (lvlObjectType) {
				case LEVEL_OBJECT::LEVEL_OBJECT_EMPTY:
					//No Object Required
					s_pCurrentLevel->m_apLevelEntities[x][y] = nullptr;
					break;
				case LEVEL_OBJECT::LEVEL_OBJECT_SOLID_WALL:
					//Spawn Solid Wall
					s_pCurrentLevel->m_apLevelEntities[x][y] = SpawnSolidWall(v3ObjPos);
					break;
				case LEVEL_OBJECT::LEVEL_OBJECT_DESTRUCT_WALL:
					//Spawn Destructable Wall
					s_pCurrentLevel->m_apLevelEntities[x][y] = SpawnDestructibleWall(v3ObjPos);
					break;
				case LEVEL_OBJECT::LEVEL_OBJECT_PLAYER_SPAWN:
					//Player Spawn add to the list of player spawns
					s_pCurrentLevel->m_vv3PlayerSpawns.push_back(v3ObjPos);
					//Set no object at position
					s_pCurrentLevel->m_apLevelEntities[x][y] = nullptr;
					break;
				default:
					//Error, not a valid level object
					ConsoleLog::LogError("[ERROR] Invalid Level Object Read from file");
					break;
			}
		}
	}

	//Close the file
	sLevelFile.flush();
	sLevelFile.close();
}

/// <summary>
/// Unload the currently loaded level
/// </summary>
void LevelManager::UnloadLevel()
{
	//Check that we have a level
	if(s_pCurrentLevel == nullptr)
	{
		return;
	}
	
	//Delete the current level
	delete s_pCurrentLevel;

	//Set the current level to be null
	s_pCurrentLevel = nullptr;
}

/// <summary>
/// Gets the currently loaded level
/// </summary>
/// <returns>Pointer to the currently loaded level</returns>
Level* LevelManager::GetCurrentLevel()
{
	return s_pCurrentLevel;
}

/// <summary>
/// Gets the name of all the levels that are loadable in the levels folder
/// </summary>
/// <returns></returns>
std::vector<std::string> LevelManager::GetLoadableLevelNames()
{
	//Create Vector for level names
	std::vector<std::string> vsLevelFiles;
	
	//Get all of the files from the level load path,
	//Adding each one to the list
	for (const auto& entry : std::filesystem::directory_iterator("bin/Levels"))
	{
		vsLevelFiles.push_back(entry.path().stem().u8string());
	}

	//Return the files we have found
	return vsLevelFiles;
}

/// <summary>
/// Spawn a destructable wall in the world
/// </summary>
/// <param name="a_v3SpawnPos">Position to spawn the wall at</param>
/// <returns>The Wall entity that was created</returns>
Entity* LevelManager::SpawnDestructibleWall(glm::vec3 a_v3SpawnPos) const
{
	//Create a solid wall and add a destructable wall component
	Entity* pWallEntity = SpawnSolidWall(a_v3SpawnPos);
	DestructibleWallComponent* pWallDestruction = new DestructibleWallComponent(pWallEntity);
	pWallEntity->AddComponent(pWallDestruction);

	//Set the colour of the wall
	BoxPrimitiveComponent* pBoxPrimative = pWallEntity->GetComponent<BoxPrimitiveComponent*>();
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
Entity* LevelManager::SpawnSolidWall(glm::vec3 a_v3SpawnPos) const
{
	//Size of the wall block
	glm::vec3 v3WallSize = glm::vec3(Level::m_fLevelCubeSize, Level::m_fLevelCubeSize, Level::m_fLevelCubeSize);
	
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
