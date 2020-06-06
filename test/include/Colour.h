#ifndef __COLOUR_H__
#define __COLOUR_H__

#include "glm/glm.hpp"
// ReSharper disable CppInconsistentNaming - For Colour Names

//Type Def vec4 as a colour
typedef glm::vec4 Colour;

//Struct of predefined colours
struct Colours
{
public:
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

//----Static Definitions for Colours----//
//
//Primary Colours
const Colour Colours::red = Colour(1.f, 0.f, 0.f, 1.f);
const Colour Colours::green = Colour(0.f, 1.f, 0.f, 1.f);
const Colour Colours::blue = Colour(0.f, 0.f, 1.f, 1.f);

//Secondary Colours
const Colour Colours::purple = Colour(0.5f, 0.f, 0.5f, 1.f);
const Colour Colours::orange = Colour(1.0f, 0.f, 0.65f, 1.f);
const Colour Colours::pink = Colour(1.0f, 0.41f, 0.71f, 1.f);

//Black / White
const Colour Colours::white = Colour(1.0f, 0.41f, 0.71f, 1.f);
const Colour Colours::black = Colour(1.0f, 0.41f, 0.71f, 1.f);

// ReSharper restore CppInconsistentNaming - For Colour Names

#endif //!__COLOUR_H__