#ifndef __LEVEL_LOADER_H__
#define __LEVEL_LOADER_H__

//Forward Declare
class Entity;
class Level;

/// <summary>
/// Class for loading and managing the game level
/// </summary>
class LevelManager
{
public:
	//Constructor / Destructor
	LevelManager() = default;
	~LevelManager();

	void LoadLevel(const std::string& a_sLevelName);
	void UnloadLevel();

	static Level* GetCurrentLevel();
	
	//Enum for level pieces
	enum class LEVEL_OBJECT
	{
		LEVEL_OBJECT_EMPTY = 0,
		LEVEL_OBJECT_SOLID_WALL = 1,
		LEVEL_OBJECT_DESTRUCT_WALL = 2,
		LEVEL_OBJECT_PLAYER_SPAWN = 3,

		LEVEL_OBJECT_COUNT //Number of Level Objects
	};
	
private:

	Entity* SpawnDestructibleWall(glm::vec3 a_v3SpawnPos) const;
	Entity* SpawnSolidWall(glm::vec3 a_v3SpawnPos) const;

	//Currently Loaded Level
	static Level* s_pCurrentLevel;
	
	//Level Load Path
	const char* m_szLevelLoadPath = "Levels/";

	//Colours for level objects
	glm::vec4 m_v4DestructableColor = glm::vec4(0.75f, 0.75f, 0.75f, 1.0f);
	glm::vec4 m_v4SolidColor = glm::vec4(0.f, 0.f, 0.f, 1.0f);
	
	//Level Settings
	
};

#endif //!__LEVEL_LOADER_H__
