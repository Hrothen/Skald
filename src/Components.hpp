#pragma once
#ifndef COMPONENT_HPP
#define COMPONENT_HPP
#include <bitset>
namespace skald{
//example component classes
//components should be templated on the total number of component classes
//and should have a field named id that can be used for masking
//in the examples I use a bitset, but you can use any type that
//will compare with a bitset via bitwise ops
template<size_t maxComponents>
struct movable{
	float xPos;
	float yPos;
	float xVel;
	float yVel;
	const std::bitset<maxComponents> id = 1UL << 0;
};
}
#endif