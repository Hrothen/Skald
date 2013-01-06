#include <gtest/gtest.h>
#include "EntityManager.hpp"

struct compA{
	unsigned long id;
};
namespace skald{
class EntityManagerTests : public ::testing::Test{
protected:

	skald::EntityManager<uint8_t,compA> e;
};

TEST_F(EntityManagerTests,DefaultConstructor){
	EXPECT_EQ(0,e.nextID);
	EXPECT_EQ(0,e.entities.size());
	EXPECT_EQ(0,e.freeEntities.size());
	auto a = e.componentVectors.template get<0>();
	EXPECT_EQ(0,a.size());
	ASSERT_EQ(1,e.freeComponents.size());
	EXPECT_EQ(0,e.freeComponents.at(0).size());
}

TEST_F(EntityManagerTests,CreateEntity){
	auto i = e.createEntity();
	ASSERT_EQ(0,static_cast<int>(i));
	EXPECT_EQ(1,e.entities.size());
	EXPECT_EQ(i,e.entities[0].id);
	EXPECT_EQ(1,e.nextID);
}

TEST_F(EntityManagerTests,RemoveEntity){
	auto i = e.createEntity();
	e.removeEntity(i);
	EXPECT_EQ(0,e.nextID);
	i = e.createEntity();
	auto j = e.createEntity();
	e.removeEntity(i);
	EXPECT_EQ(2,e.nextID);
	ASSERT_EQ(1,e.freeEntities.size());
}
TEST_F(EntityManagerTests,PurgeEntity){
	/*
	auto i = e.createEntity();
	auto j = e.createEntity();
	compA a,b;
	a.id = b.id = 1UL;
	e.addComponent(i,compA(a));
	e.addComponent(j,compA(b));
	EXPECT_EQ(0,e.entities[i].indicies[0]);
	EXPECT_EQ(1,e.entities[j].indicies[0]);
	e.purgeEntity(i);
	EXPECT_EQ(0,e.entities[j].indicies[0]);
	*/
}
TEST_F(EntityManagerTests,ReuseEntity){}
TEST_F(EntityManagerTests,AddComponent){
	auto i = e.createEntity();
	ASSERT_EQ(0,static_cast<int>(i));
	ASSERT_EQ(true,e.freeComponents.at(find_first<VectorTuple<compA>,compA>::value).empty());
	compA a {1UL};
	e.addComponent(i,compA(a));
}
TEST_F(EntityManagerTests,RemoveComponent){}

}//namespace