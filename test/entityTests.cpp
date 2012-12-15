#include <gtest/gtest.h>
#include "EntityManager.hpp"

struct compA{
	const unsigned long id = 1UL << 0;
};

class EntityManagerTests : public ::testing::Test{
protected:

	skald::EntityManager<uint8_t,compA> e;
};

TEST_F(EntityManagerTests,DefaultConstructor){
	EXPECT_EQ(0,e.nextID);
	EXPECT_EQ(0,e.entities.size());
	EXPECT_EQ(0,e.freeEntities.size());
	EXPECT_EQ(0,e.componentVectors.get<0>().size());
	EXPECT_EQ(1,e.freeComponents.size());
	EXPECT_EQ(0,e.freeComponents.at(0).size())
}