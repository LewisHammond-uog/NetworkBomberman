#ifndef __LEVEL_LOADER_H__
#define __LEVEL_LOADER_H__

#include <utility>
#include <vector>

//Forward Declare
class Entity;
class Level;

/// <summary>
/// Class for loading the game level
/// </summary>
class LevelLoader
{
public:
	//Constructor / Destructor
	LevelLoader() = default;
	~LevelLoader();

	void LoadLevel(const std::string& a_sLevelName);
	void UnloadLevel();

	//Enum for level pieces
	enum class LEVEL_OBJECT
	{
		LEVEL_OBJECT_EMPTY = 0,
		LEVEL_OBJECT_SOLID_WALL = 1,
		LEVEL_OBJECT_DESTRUCT_WALL = 2,

		LEVEL_OBJECT_COUNT //Number of Level Objects
	};
	
private:



	Entity* SpawnDestructibleWall(glm::vec3 a_v3SpawnPos) const;
	Entity* SpawnSolidWall(glm::vec3 a_v3SpawnPos) const;

	//Currently Loaded Level
	Level* m_pCurrentLevel;
	
	//Level Load Path
	const char* m_szLevelLoadPath = "Levels/";

	//Colours for level objects
	glm::vec4 m_v4DestructableColor = glm::vec4(0.75f, 0.75f, 0.75f, 1.0f);
	glm::vec4 m_v4SolidColor = glm::vec4(0.f, 0.f, 0.f, 1.0f);
	
	//Level Settings
	const glm::vec3 m_v3StartPos = glm::vec3(-9, 0, -9); //World Position to start the level from
	const float m_fLevelCubeSize = 1.0f; //Size of level cube
	const float m_fLevelSpacing = 2.0f; //Spacing between level cubes
	const float m_fLevelZ = 0.f; //Z Level of all level objects
	
};

#endif //!__LEVEL_LOADER_H__
