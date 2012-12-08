#pragma once
#ifndef SKALD_COMPONENT_HPP
#define SKALD_COMPONENT_HPP
#include <bitset>
#include <boost/concept.hpp>

namespace skald{
	
template <class C>
struct Component{
public:

	BOOST_CONCEPT_USAGE(Component){
		//require POD
		static_assert(std::is_pod<C>::value == true,
			"Compilation error: component classes should be Plain Old Data");
		auto i = _c.id;						//check id field exists
		std::bitset<sizeof(i) * 8> b(i);	//check convertible to bitset
		b |= i;								//check or comparable to bitset
		b &= i;								//check and comparable to bitset
	}

private:
	C _c;
};
}
#endif