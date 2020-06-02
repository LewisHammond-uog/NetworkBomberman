#ifndef __LEVEL_H__
#define __LEVEL_H__

//C++ Includes
#include <vector>

//Forward Declare
class Entity;

/// <summary>
/// Class to store the data about a level
/// </summary>
class Level
{
	friend class LevelManager; //Allow the level loader to access private members
public:
	bool IsCellFree(glm::vec3 a_v3Pos) const;
	static glm::vec3 GetNearestCell(glm::vec3 a_v3Pos);
	
	static const float sc_fLevelSpacing; //Spacing between level cubes
private:

	//Constructor for Levels
	Level(std::string a_sLevelName, glm::vec2 a_v2LevelSize);
	//Destructor for Levels
	~Level();

	//Level Settings
	std::string m_sName;
	glm::vec2 m_v2Size;
	Entity*** m_apLevelData; //2d Array of data for the level

	//Level Constants
	static const float m_fLevelCubeSize; //Size of level cube

	static const float sc_fLevelY; //Z Level of all level objects
};

#endif