#include <gtest/gtest.h>
#include "World.hpp"

namespace skald{

struct compA{};
struct compB{};
struct compC{};

typedef World<uint8_t,compA,compB,compC> testWorld;

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
	TestSystem3(int p,int t):System(p,t),testVal(0){}
	~TestSystem3(){}
	int testVal;
	const int id()const override{return 3;}
	void init(testWorld*) override{}
	void destroy(testWorld*) override{}
	void update(const double d) override{
		testVal += static_cast<int>(d);
	}
};

class SystemTests : public ::testing::Test{
public:
	SystemTests(){}
	~SystemTests(){}
protected:
	SystemManager<uint8_t,compA,compB,compC> manager;
};

typedef typename testWorld::SystemPtr sysPtr;

TEST_F(SystemTests,AddSystem){
	EXPECT_EQ(0,manager.systems.size());
	manager.add(sysPtr(new TestSystem3(3,1)));
	manager.add(sysPtr(new TestSystem1(1,1)));
	manager.add(sysPtr(new TestSystem2(2,1)));
	EXPECT_EQ(3,manager.systems.size());
	EXPECT_EQ(1,manager.systems[0]->id());
	EXPECT_EQ(1,manager.systems[0]->getPriority());
	EXPECT_EQ(2,manager.systems[1]->id());
	EXPECT_EQ(2,manager.systems[1]->getPriority());
	EXPECT_EQ(3,manager.systems[2]->id());
	EXPECT_EQ(3,manager.systems[2]->getPriority());
	//make sure copies aren't added
	manager.add(sysPtr(new TestSystem1(1,1)));
	EXPECT_EQ(3,manager.systems.size());
	//make sure shared_ptr is operating as expected
	EXPECT_EQ(1,manager.systems[0].use_count());
	EXPECT_EQ(1,manager.systems[1].use_count());
	EXPECT_EQ(1,manager.systems[2].use_count());
	auto i = manager.systems[0];
	EXPECT_EQ(2,i.use_count());
}
TEST_F(SystemTests,RemoveSystem){
	manager.add(sysPtr(new TestSystem3(3,1)));
	manager.add(sysPtr(new TestSystem1(1,1)));
	manager.add(sysPtr(new TestSystem2(2,1)));
	EXPECT_EQ(3,manager.systems.size());
	manager.remove(2);
	ASSERT_EQ(2,manager.systems.size());
	EXPECT_EQ(1,manager.systems[0]->id());
	EXPECT_EQ(1,manager.systems[0]->getPriority());
	EXPECT_EQ(3,manager.systems[1]->id());
	EXPECT_EQ(3,manager.systems[1]->getPriority());
	manager.remove(sysPtr(new TestSystem3(3,1)));
	ASSERT_EQ(1,manager.systems.size());
	EXPECT_EQ(1,manager.systems[0]->id());
	EXPECT_EQ(1,manager.systems[0]->getPriority());

}
TEST_F(SystemTests,Update){
	manager.add(sysPtr(new TestSystem3(3,1)));
	manager.add(sysPtr(new TestSystem1(1,1)));
	manager.add(sysPtr(new TestSystem2(2,1)));
	EXPECT_EQ(0,dynamic_cast<TestSystem1*>(manager.systems[0].get())->testVal);
	EXPECT_EQ(0,dynamic_cast<TestSystem2*>(manager.systems[1].get())->testVal);
	EXPECT_EQ(0,dynamic_cast<TestSystem3*>(manager.systems[2].get())->testVal);
	manager.update(1.2);
	EXPECT_EQ(1,dynamic_cast<TestSystem1*>(manager.systems[0].get())->testVal);
	EXPECT_EQ(-1,dynamic_cast<TestSystem2*>(manager.systems[1].get())->testVal);
	EXPECT_EQ(1,dynamic_cast<TestSystem3*>(manager.systems[2].get())->testVal);
	manager.update(0.3);
	EXPECT_EQ(2,dynamic_cast<TestSystem1*>(manager.systems[0].get())->testVal);
	EXPECT_EQ(-2,dynamic_cast<TestSystem2*>(manager.systems[1].get())->testVal);
	EXPECT_EQ(1,dynamic_cast<TestSystem3*>(manager.systems[2].get())->testVal);
}
TEST_F(SystemTests,Get){
	manager.add(sysPtr(new TestSystem3(3,1)));
	manager.add(sysPtr(new TestSystem1(1,1)));
	manager.add(sysPtr(new TestSystem2(2,1)));
	EXPECT_EQ(1,manager.systems[0].use_count());
	EXPECT_EQ(1,manager.systems[1].use_count());
	EXPECT_EQ(1,manager.systems[2].use_count());
	auto i = manager.get(1);
	EXPECT_EQ(2,i.use_count());
	EXPECT_EQ(1,i->id());
	auto j = manager.get(2);
	EXPECT_EQ(2,j.use_count());
	EXPECT_EQ(2,j->id());
	auto k = manager.get(3);
	EXPECT_EQ(2,k.use_count());
	EXPECT_EQ(3,k->id());
	//check the shared_ptrs update as expected
	manager.update(1.3);
	EXPECT_EQ(1,dynamic_cast<TestSystem1*>(i.get())->testVal);
	EXPECT_EQ(-1,dynamic_cast<TestSystem2*>(j.get())->testVal);
	EXPECT_EQ(1,dynamic_cast<TestSystem3*>(k.get())->testVal);
}

}//namespace