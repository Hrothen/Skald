#include <gtest/gtest.h>
#include "tests_shared.hpp"

using namespace std;
namespace skald{
class EntityManagerTests : public ::testing::Test{
public:
	void DefaultConstructor(){
		EXPECT_EQ(0,e.nextID);
		EXPECT_EQ(0,e.entities.size());
		EXPECT_EQ(0,e.freeEntities.size());
		auto a = get<0>(e.componentVectors);
		auto b = get<1>(e.componentVectors);
		auto c = get<2>(e.componentVectors);
		EXPECT_EQ(0,a.size());
		EXPECT_EQ(0,b.size());
		EXPECT_EQ(0,c.size());
		ASSERT_EQ(3,e.freeComponents.size());
		EXPECT_EQ(0,e.freeComponents.at(0).size());
		EXPECT_EQ(0,e.freeComponents.at(1).size());
		EXPECT_EQ(0,e.freeComponents.at(1).size());
	}
	void CreateEntity(){
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
	void RemoveEntity(){
		auto i = e.createEntity();
		e.removeEntity(i);
		EXPECT_EQ(0,e.nextID);
		i = e.createEntity();
		auto j = e.createEntity();
		e.removeEntity(i);
		EXPECT_EQ(2,e.nextID);
		ASSERT_EQ(1,e.freeEntities.size());
	}
	void PurgeEntity(){
		auto i = e.createEntity();
		auto j = e.createEntity();
		compA a{1UL},b{1UL};
		//a.id = b.id = 1UL;
		e.addComponent(i,a);
		e.addComponent(j,b);
		EXPECT_EQ(0,e.entities[i].indicies[0]);
		EXPECT_EQ(1,e.entities[j].indicies[0]);
		EXPECT_EQ(true,e.entities[i].mask[0]);
		EXPECT_EQ(true,e.entities[j].mask[0]);
		e.purgeEntity(i);
		auto & v = get<0>(e.componentVectors);
		EXPECT_EQ(1,v.size());
		EXPECT_EQ(1,e.freeEntities.size());
		EXPECT_EQ(0,e.freeComponents[0].size());
		EXPECT_EQ(0,e.entities[j].indicies[0]);
	}
	void ReuseEntity(){
		auto i = e.createEntity();
		auto j = e.createEntity();
		EXPECT_EQ(0,i);
		EXPECT_EQ(1,j);
		e.removeEntity(i);
		EXPECT_EQ(1,j);
		auto k = e.createEntity();
		EXPECT_EQ(0,k);
		EXPECT_EQ(1,j);
	}
	void AddComponent(){
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
	void RemoveComponent(){
		EXPECT_EQ(0,e.freeComponents[0].size());
		EXPECT_EQ(0,e.freeComponents[1].size());
		EXPECT_EQ(0,e.freeComponents[2].size());
		auto i = e.createEntity();
		auto j = e.createEntity();
		compA a {1UL};
		compB b {2UL,2};
		compC c {4UL,3,4};
		e.addComponent(i,a);
		e.addComponent(i,b);
		e.addComponent(i,c);
		e.addComponent(j,a);
		e.addComponent(j,b);
		e.addComponent(j,c);
		e.removeComponent<compA>(i);
		e.removeComponent<compB>(i);
		e.removeComponent<compC>(i);
		EXPECT_EQ(1,e.entities[j].indicies[0]);
		EXPECT_EQ(1,e.entities[j].indicies[1]);
		EXPECT_EQ(1,e.entities[j].indicies[2]);
		EXPECT_EQ(1,e.freeComponents[0].size());
		EXPECT_EQ(1,e.freeComponents[1].size());
		EXPECT_EQ(1,e.freeComponents[2].size());
	}
protected:

	skald::EntityManager<uint8_t,compA,compB,compC> e;
};

TEST_F(EntityManagerTests,DefaultConstructor){
	DefaultConstructor();
}

TEST_F(EntityManagerTests,CreateEntity){
	CreateEntity();
}

TEST_F(EntityManagerTests,RemoveEntity){
	RemoveEntity();
}
TEST_F(EntityManagerTests,PurgeEntity){
	PurgeEntity();
}
TEST_F(EntityManagerTests,ReuseEntity){
	ReuseEntity();
}
TEST_F(EntityManagerTests,AddComponent){
	AddComponent();
}
TEST_F(EntityManagerTests,RemoveComponent){
	RemoveComponent();
}

}//namespace