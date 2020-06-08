#include "stdafx.h"
#include "Level.h"

//C++ Includes
#include <math.h>

//Project Includes
#include "ConsoleLog.h"
#include "Entity.h"

const float Level::m_fLevelCubeSize = 1.0f; //Size of level cube
const float Level::sc_fLevelSpacing = 2.0f; //Spacing between level 
const float Level::sc_fLevelY = 0.f; //Z Level of all level objects

/// <summary>
/// Create a Level
/// </summary>
/// <param name="a_sLevelName">Name of the level</param>
/// <param name="a_v2LevelSize">Size of the level</param>
Level::Level(std::string a_sLevelName, glm::vec2 a_v2LevelSize)
{
	//Set the level name
	m_sName = std::move(a_sLevelName);
	m_v2Size = a_v2LevelSize;

	//Initalise the 2D level Array
	m_apLevelEntities = new Entity** [m_v2Size.x]; //Create 1D Array of columns
	for (int i = 0; i < m_v2Size.x; ++i)
	{
		m_apLevelEntities[i] = new Entity*[m_v2Size.y];
	}
}

/// <summary>
/// Destroy the Level
/// Deletes all of the level entities
/// </summary>
Level::~Level()
{
	//Delete the 2D array of entitess
	for (int i = 0; i < m_v2Size.x; ++i)
	{
		delete[] m_apLevelEntities[i];
	}

	delete[] m_apLevelEntities;
}

/// <summary>
/// Gets there is a free grid cell (i.e does not have a wall)
/// </summary>
/// <param name="a_v3Pos">World Position of the cell (does not have to be the
/// exact cell position)</param>
/// <returns></returns>
bool Level::IsCellFree(glm::vec3 a_v3Pos) const
{
	//Get the position within the array, ignoring Y as all walls are at the same y pos
	//Take away the level offset so our position is within the array bounds (> 0)
	glm::vec3 v3NearestCell = GetNearestCell(a_v3Pos);
	int iArrayX = v3NearestCell.x;
	int iArrayY = v3NearestCell.z;

	//Check that our array positions are within the bounds of the array
	if(iArrayX < 0 || iArrayX > m_v2Size.x ||
		iArrayY < 0 || iArrayY > m_v2Size.y)
	{
		//Cell is invalid
		return false;
	}

	//Now we know that the cell is valid we can check if there is an entity there
	Entity* pEntityAtPos = m_apLevelEntities[iArrayX][iArrayY];
	if(pEntityAtPos != nullptr)
	{
		if (pEntityAtPos->IsEnabled()) {
			//Cell if not free
			return false;
		}
	}

	

	//Checks passed, cell is free
	return true;
}

/// <summary>
/// Gets the position of the nearest grid cell
/// </summary>
/// <returns></returns>
glm::vec3 Level::GetNearestCell(glm::vec3 a_v3Pos)
{
	//Get the "array" position
	float fCellX = round(a_v3Pos.x / sc_fLevelSpacing);
	float fCellZ = round(a_v3Pos.z / sc_fLevelSpacing);
	
	return  glm::vec3(fCellX, sc_fLevelY, fCellZ);
}

/// <summary>
/// Gets the Nth player spawn for the level
/// </summary>
/// <param name="a_uIndex"></param>
/// <param name="a_pSpawnPos">[OUT] By reference inserts spawn positiopn in to vector</param>
/// <returns>If spawn Index is valid</returns>
bool Level::GetPlayerSpawn(const unsigned a_uIndex, glm::vec3& a_pSpawnPos)
{
	//Check that index is within size of the spawns
	if(m_vv3PlayerSpawns.size() > a_uIndex)
	{
		//Put spawn pos in var
		a_pSpawnPos = m_vv3PlayerSpawns[a_uIndex];
		return true;
	}else
	{
		//No valid spawn
		return false;
	}
}
