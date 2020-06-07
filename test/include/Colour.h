#ifndef __COLOUR_H__
#define __COLOUR_H__

#include "glm/glm.hpp"
// ReSharper disable CppInconsistentNaming - For Colour Names

//Struct of predefined colours
struct Colours
{
public:
	//Type Def vec4 as a colour
	typedef glm::vec4 Colour;
	
	//Primary Colours
	static const Colour red;
	static const Colour green;
	static const Colour blue;

	//Secondsary Colours
	static const Colour purple;
	static const Colour orange;
	static const Colour pink;

	//Black/White
	static const Colour white;
	static const Colour black;
};

// ReSharper restore CppInconsistentNaming - For Colour Names


#endif //!__COLOUR_H__