#pragma once

/// <summary>
/// Class to store the data about a level
/// </summary>
class Level
{
	friend class LevelLoader; //Allow the level loader to access private members
	
private:

	//Constructor for Levels
	Level(std::string a_sLevelName, glm::vec2 a_v2LevelSize)
	{
		//Set the level name
		m_sName = std::move(a_sLevelName);
		m_v2Size = a_v2LevelSize;

		//Initalise the 2D level Array
		m_aiLevelData = new int* [m_v2Size.x]; //Create 1D Array of columns
		for (int i = 0; i < m_v2Size.x; ++i)
		{
			m_aiLevelData[i] = new int[m_v2Size.y];
		}
	}

	//Destructor for Levels
	~Level()
	{
		//Destroy all of the entites created by the level
		for(int i = 0; i < m_vpLevelEntities.size(); ++i)
		{
			delete m_vpLevelEntities[i];
		}

		//Delete the 2D array of data
		for (int i = 0; i < m_v2Size.x; ++i)
		{
			delete[] m_aiLevelData[i];
		}
		delete[] m_aiLevelData;
	}


	std::string m_sName;
	glm::vec2 m_v2Size;
	std::vector<Entity*> m_vpLevelEntities; //List of entites that are level objects
	int** m_aiLevelData; //2d Array of data for the level
};
