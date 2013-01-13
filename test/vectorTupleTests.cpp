#include <gtest/gtest.h>
#include "tests_shared.hpp"

namespace skald{

TEST(VectorTupleTests,FindFirst){
	typedef skald::VectorTuple<compA,compB,compC> tup;
	tup v;
	int i = skald::find_first<tup,compA>::value;
	int j = skald::find_first<tup,compB>::value;
	int k = skald::find_first<tup,compC>::value;
	EXPECT_EQ(0,i);
	EXPECT_EQ(1,j);
	EXPECT_EQ(2,k);
	//now check that this is avilible at compile time
	static_assert(skald::find_first<tup,compA>::value == 0,"test failed");
}

//test that get by type properly accesses the indexing struct at compile time
TEST(VectorTupleTests,GetByType){
	typedef skald::VectorTuple<compA,compB,compC> tup;
	tup v;
	auto & i = v.getByType<compB>();
	compB b;
	b.id = 1;
	b.first = 7;
	i.push_back(b);
	ASSERT_EQ(7,v.getByType<compB>().at(0).first);
}
}//namespace