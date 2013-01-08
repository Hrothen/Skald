#include <gtest/gtest.h>
#include "EntityManager.hpp"

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

namespace skald{
class EntityManagerTests : public ::testing::Test{
protected:

	skald::EntityManager<uint8_t,compA,compB,compC> e;
};

TEST_F(EntityManagerTests,DefaultConstructor){
	EXPECT_EQ(0,e.nextID);
	EXPECT_EQ(0,e.entities.size());
	EXPECT_EQ(0,e.freeEntities.size());
	auto a = e.componentVectors.template get<0>();
	auto b = e.componentVectors.template get<1>();
	auto c = e.componentVectors.template get<2>();
	EXPECT_EQ(0,a.size());
	EXPECT_EQ(0,b.size());
	EXPECT_EQ(0,c.size());
	ASSERT_EQ(3,e.freeComponents.size());
	EXPECT_EQ(0,e.freeComponents.at(0).size());
	EXPECT_EQ(0,e.freeComponents.at(1).size());
	EXPECT_EQ(0,e.freeComponents.at(1).size());
}

TEST_F(EntityManagerTests,CreateEntity){
	auto i = e.createEntity();
	ASSERT_EQ(0,static_cast<int>(i));
	EXPECT_EQ(1,e.entities.size());
	EXPECT_EQ(i,e.entities[0].id);
	EXPECT_EQ(1,e.nextID);
	ASSERT_EQ(3,e.entities[i].indicies.size());
	EXPECT_EQ(0,e.entities[i].indicies[0]);
	EXPECT_EQ(0,e.entities[i].indicies[1]);
	EXPECT_EQ(0,e.entities[i].indicies[2]);
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
	compA a {1UL << 0};
	compB b {1UL << 1,5};
	compC c {1UL << 2,7,8};
	e.addComponent(i,a);
	e.addComponent(i,b);
	e.addComponent(i,c);
	EXPECT_EQ(0,e.entities[i].indicies[0]);
	EXPECT_EQ(0,e.entities[i].indicies[1]);
	EXPECT_EQ(0,e.entities[i].indicies[2]);
	auto j = e.createEntity();
	compA a2 {1UL << 0};
	compB b2 {1UL << 1,6};
	compC c2 {1UL << 2,9,10};
	e.addComponent(j,a2);
	e.addComponent(j,b2);
	e.addComponent(j,c2);
	EXPECT_EQ(1,e.entities[j].indicies[0]);
	EXPECT_EQ(1,e.entities[j].indicies[1]);
	EXPECT_EQ(1,e.entities[j].indicies[2]);
}
TEST_F(EntityManagerTests,RemoveComponent){
	auto i = e.createEntity();
}

}//namespace