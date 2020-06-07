#include "stdafx.h"
#include "Colour.h"

//Project Includes
#include "MathsUtils.h"

//----Static Definitions for Colours----//
//
//Primary Colours
const glm::vec4 Colours::red = Colour(1.f, 0.f, 0.f, 1.f);
const glm::vec4 Colours::green = Colour(0.f, 1.f, 0.f, 1.f);
const glm::vec4 Colours::blue = Colour(0.f, 0.f, 1.f, 1.f);

//Secondary Colours
const glm::vec4 Colours::purple = Colour(0.5f, 0.f, 0.5f, 1.f);
const glm::vec4 Colours::orange = Colour(1.0f, 0.f, 0.65f, 1.f);
const glm::vec4 Colours::pink = Colour(1.0f, 0.41f, 0.71f, 1.f);

//Black / White
const glm::vec4 Colours::white = Colour(1.0f, 0.41f, 0.71f, 1.f);
const glm::vec4 Colours::black = Colour(1.0f, 0.41f, 0.71f, 1.f);

//--------------------------------------//

/// <summary>
/// Generate a Random Colour
/// </summary>
/// <returns>Vector 4 of a random colour</returns>
Colour Colours::RandomColour() 
{
	//Generate random RBG channles
	const float r = MathsUtils::RandomRange(0.f, 1.f);
	const float g = MathsUtils::RandomRange(0.f, 1.f);
	const float b = MathsUtils::RandomRange(0.f, 1.f);
	const float a = 1.f; //Constant Alpha of 1
	
	return glm::vec4(r,g,b,a);
}