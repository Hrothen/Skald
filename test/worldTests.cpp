#include <gtest/gtest.h>
#include "tests_shared.hpp"

namespace skald{
class WorldTests : public ::testing::Test{
protected:
	testWorld world;
};

//A lot of these are just wrapper functions around methods tested elsewhere
//so the test is very short

TEST_F(WorldTests,AddEntity){
	auto i = world.addEntity();
	ASSERT_EQ(0,i);
}
TEST_F(WorldTests,RemoveEntity){
	auto i = world.addEntity();
	auto j = world.addEntity();
	ASSERT_EQ(1,j);
	world.removeEntity(i);
	ASSERT_EQ(1,j);
}
TEST_F(WorldTests,AddComponent){
	auto i = world.addEntity();
	compA a{1UL};
	compB b{2UL,3};
	compC c{3UL,4,5};
	world.addComponent(i,a);
	world.addComponent(i,b);
	world.addComponent(i,c);
	auto & e = world.getEntityManager().getEntityVector();
	auto & v = world.getEntityManager().getComponentVectors();
	ASSERT_EQ(1,e.size());
	ASSERT_EQ(1,v.getByType<compA>().size());
	EXPECT_EQ(a.id,v.getByType<compA>().at(0).id);
	ASSERT_EQ(1,v.getByType<compB>().size());
	EXPECT_EQ(b.id,v.getByType<compB>().at(0).id);
	EXPECT_EQ(b.first,v.getByType<compB>().at(0).first);
	ASSERT_EQ(1,v.getByType<compC>().size());
	EXPECT_EQ(c.id,v.getByType<compC>().at(0).id);
	EXPECT_EQ(c.first,v.getByType<compC>().at(0).first);
	EXPECT_EQ(c.second,v.getByType<compC>().at(0).second);
}
TEST_F(WorldTests,RemoveComponent){
	auto i = world.addEntity();
	compA a{1UL};
	compB b{2UL,3};
	compC c{3UL,4,5};
	world.addComponent(i,a);
	world.addComponent(i,b);
	world.addComponent(i,c);
	auto & e = world.getEntityManager().getFreeComponents();
	auto & v = world.getEntityManager().getComponentVectors();
	ASSERT_EQ(1,v.getByType<compB>().size());
	EXPECT_EQ(0,e.at(1).size());
	world.removeComponent<compB>(i);
	EXPECT_EQ(1,v.getByType<compB>().size());
	EXPECT_EQ(1,e.at(1).size());
}
TEST_F(WorldTests,AddSystem){
	auto sys = std::make_shared<TestSystem3>(1,0);
	ASSERT_EQ(nullptr,sys->parent);
	world.addSystem(sys);
	ASSERT_EQ(&world,sys->parent);
}
TEST_F(WorldTests,RemoveSystem){
	auto sys = std::make_shared<TestSystem3>(1,0);
	ASSERT_EQ(nullptr,sys->parent);
	world.addSystem(sys);
	ASSERT_EQ(&world,sys->parent);
	EXPECT_EQ(2,sys.use_count());
	world.removeSystem(sys);
	ASSERT_EQ(nullptr,sys->parent);
	EXPECT_EQ(1,sys.use_count());
}
TEST_F(WorldTests,GetSystem){
	auto sys1 = std::make_shared<TestSystem1>(1,0);
	auto sys2 = std::make_shared<TestSystem2>(2,0);
	auto sys3 = std::make_shared<TestSystem3>(3,0);
	world.addSystem(sys1);
	world.addSystem(sys2);
	world.addSystem(sys3);
	ASSERT_EQ(2,sys1.use_count());
	ASSERT_EQ(2,sys2.use_count());
	ASSERT_EQ(2,sys3.use_count());
	auto newSys1 = world.getSystem(sys1->id());
	EXPECT_EQ(3,sys1.use_count());
	EXPECT_EQ(newSys1,sys1);
	auto newSys2 = world.getSystem(sys2->id());
	EXPECT_EQ(3,sys2.use_count());
	EXPECT_EQ(newSys2,sys2);
	auto newSys3 = world.getSystem(sys3->id());
	EXPECT_EQ(3,sys3.use_count());
	EXPECT_EQ(newSys3,sys3);
}
TEST_F(WorldTests,Update){
	auto sys1 = std::make_shared<TestSystem1>(1,0);
	auto sys2 = std::make_shared<TestSystem2>(2,0);
	auto sys3 = std::make_shared<TestSystem3>(3,0);
	world.addSystem(sys1);
	world.addSystem(sys2);
	world.addSystem(sys3);
	ASSERT_EQ(0, sys1->testVal);
	ASSERT_EQ(0, sys2->testVal);
	ASSERT_EQ(0, sys3->testVal);
	world.update(3.3);
	EXPECT_EQ(1, sys1->testVal);
	EXPECT_EQ(-1, sys2->testVal);
	EXPECT_EQ(3, sys3->testVal);
}
}//namespace