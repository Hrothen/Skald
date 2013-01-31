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

class TestSystem1 : public testWorld::SystemBase{
public:
	TestSystem1(int p,int t):System(p,t),testVal(0){}
	~TestSystem1(){}
	int testVal;
	const int id()const override{return 1;}
	void init(testWorld*) override{}
	void destroy(testWorld*) override{}
	void update(const double d) override{
		testVal++;
	}
};

class TestSystem2 : public testWorld::SystemBase{
public:
	TestSystem2(int p,int t):System(p,t),testVal(0){}
	~TestSystem2(){}
	int testVal;
	const int id()const override{return 2;}
	void init(testWorld*) override{}
	void destroy(testWorld*) override{}
	void update(const double d) override{
		testVal--;
	}
};

class TestSystem3 : public testWorld::SystemBase{
public:
	TestSystem3(int p,int t):System(p,t),testVal(0),parent(nullptr){}
	~TestSystem3(){}
	int testVal;
	const int id()const override{return 3;}
	void init(testWorld* p) override{
		parent = p;
	}
	void destroy(testWorld*) override{
		parent = nullptr;
	}
	void update(const double d) override{
		testVal += static_cast<int>(d);
	}

	testWorld * parent;
};

}//namespace
#endif