#include <gtest/gtest.h>
#include "System.hpp"

class TestSystem : skald::System{
public:
	TestSystem(int p,int t):System(p,t){}
	~TestSystem();
	TestSystem& operator =(const TestSystem& t){
		priority = t.priority;
		threadGroup = t.threadGroup;
	}
	virtual void init(){}
	virtual void destroy(){}
	virtual void update(const double d){}
};

class SystemTester : public ::testing::Test{
protected:
	virtual void SetUp(){
		sys = TestSystem(1,1);
	}

	TestSystem sys;
};