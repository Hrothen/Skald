#include <gtest/gtest.h>
#include "System.hpp"

class SystemTester : public ::testing::Test{
protected:
	virtual void SetUp(){
		sys = System(1,1);
	}

	TestSystem sys;
};

class TestSystem : skald::System{
public:
	TestSystem(int p,int t):System(p,t){}
	~TestSystem();

	virtual void init(){}
	virtual void destroy(){}
	virtual void update(const double d){}
};