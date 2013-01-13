#pragma once
#ifndef SKALD_TESTS_SHARED_HPP
#define SKALD_TESTS_SHARED_HPP

#include "World.hpp"

namespace skald{

struct compA{
	unsigned long id;
};

struct compB{
	unsigned long id;
	int first;
};

struct compC{
	unsigned long id;
	int first;
	int second;
};

typedef World<uint8_t,compA,compB,compC> testWorld;
typedef typename testWorld::SystemPtr sysPtr;



}//namespace
#endif