#pragma once
#ifndef COMPONENT_HPP
#define COMPONENT_HPP
namespace skald{
//example component classes
//components should have a field named id that can be used for masking.
template<size_t maxComponents>
struct movable{
	float xPos;
	float yPos;
	float xVel;
	float yVel;
	const unsigned long id = 1UL << 0;
};
}
#endif