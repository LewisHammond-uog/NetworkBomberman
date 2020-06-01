#include <utility>
#include <vector>


#ifndef __LEVEL_LOADER_H__
#define __LEVEL_LOADER_H__

//Forward Declare
class Entity;

//Struct to store a level

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

	/// <summary>
	/// Struct to store the data about a level
	/// </summary>
	struct Level
	{
		//Constructor for Levels
		Level(std::string a_sLevelName, glm::vec2 a_v2LevelSize)
		{
			//Set the level name
			m_sName = std::move(a_sLevelName);
			m_v2Size = a_v2LevelSize;

			//Initalise the 2D level Array
			m_aiLevelData = new int*[m_v2Size.x]; //Create 1D Array of columns
			for(int i = 0; i < m_v2Size.x; ++i)
			{
				m_aiLevelData[i] = new int[m_v2Size.y];
			}
		}
		
		std::string m_sName;
		glm::vec2 m_v2Size;
		std::vector<Entity*> m_vpLevelEntities; //List of entites that are level objects
		int** m_aiLevelData; //2d Array of data for the level
	};


	Entity* SpawnDestructibleWall(glm::vec3 a_v3SpawnPos) const;
	Entity* SpawnSolidWall(glm::vec3 a_v3SpawnPos) const;

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
