#ifndef  __COLOUR_H__
#define __COLOUR_H__

#include "glm/glm.hpp"


// ReSharper disable CppInconsistentNaming - For Colour Names
// 
//Struct of predefined colours
struct Colours
{
public:
	//Type Def vec4 as a colour
	typedef glm::vec4 Colour;
	
	//Primary Colours
	Colour red = glm::vec4(1.f, 0.f, 0.f, 1.f);
	Colour green = glm::vec4(0.f, 1.f, 0.f, 1.f);
	Colour blue = glm::vec4(0.f, 0.f, 1.f, 1.f);

	//Secondsary Colours
	Colour purple = glm::vec4(0.5f, 0.f, 0.5f, 1.f);
	Colour orange = glm::vec4(1.0f, 0.f, 0.65f, 1.f);
	Colour pink = glm::vec4(1.0f, 0.41f, 0.71f, 1.f);

	//Black/White
	Colour white = glm::vec4(1.f, 1.f, 1.f, 1.f);
	Colour black = glm::vec4(0.f, 0.f, 0.f, 1.f);

};

// ReSharper restore CppInconsistentNaming - For Colour Names

#endif //!__COLOUR_H__