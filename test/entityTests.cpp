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
}//namespace