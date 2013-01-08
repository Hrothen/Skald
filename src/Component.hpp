#pragma once
#ifndef SKALD_COMPONENT_HPP
#define SKALD_COMPONENT_HPP
#include <bitset>
#include <boost/concept_check.hpp>

namespace skald{
	
template <class C>
struct Component{
public:

	BOOST_CONCEPT_USAGE(Component){
		//require POD
		static_assert(std::is_pod<C>::value == true,
			"Compilation error: component classes should be Plain Old Data");
	}

private:
	C _c;
};
}
#endif