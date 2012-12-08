#pragma once
#ifndef SKALD_ENTITY_MANAGER_HPP
#define SKALD_ENTITY_MANAGER_HPP
#include <bitset>
#include <cstdint>
#include <vector>
#include <array>
#include <pair>
#include <type_traits>
#include "Components.hpp"
#include "vectorTuple.hpp"

namespace skald{
typedef uint16_t entityID;

template<size_t maxComponents,class indexType = uint8_t>
class entity{
	friend class EntityManager;
public:
	entityID id;
private:

	explicit entity(const entityID _id):id(_id),mask(0),indicies(0){}

	std::bitset<maxComponents> mask;
	std::vector<indexType> indicies;
};

template<class indexType = uint8_t,class... components>
class EntityManager{
public:
	entityID createEntity(){
	}

	void deleteEntity(const entityID id){
	}
private:

	template<unsigned int Index = sizeof...(components) - 1>
	inline typename std::enable_if<Index >= 0>::type
	deleteComponents(const unsigned long b,const std::vector<indexType>& v){
		if(b & 1UL<<Index){
			auto & row = get<Index>(componentVectors);
			row.erase(v.back());
			//update entities with new component indicies
			for(auto e : entities){
				//this is pretty ugly, we need to check if each entity
				//has the component, then work out where it is in the tuple,
				//then check if it needs to be updated
				if (e.mask &= 1UL<<Index){
					int acc = 0;
					for(int i = e.mask.size() - 1 ; i > Index, i--){
						if (e.mask[i] == true)
							acc++;
					}
					if(e.indicies[acc] > v.back())
						e.indicies[acc]--;
				}
			}
			v.pop_back();
			if(v.empty() == true)
				return;
			deleteComponents<Index - 1>(b,v);
		}
	}

	//Empty function to end the recursive template
	//might want to make this throw an error though, since it should never be called
	template<unsigned int Index>
	inline typename std::enable_if<Index < 0>::type
	deleteComponents(const unsigned long b,const std::vector<indexType>& v;){}

	entityID nextID;
	//vector of entities
	std::vector<entity<sizeof...(components),indexType>> entities;
	//tuple of vectors of components
	vectorTuple<components...> componentVectors;
};
}
#endif