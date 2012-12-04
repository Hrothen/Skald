#pragma once
#ifndef SKALD_ENTITY_MANAGER_HPP
#define SKALD_ENTITY_MANAGER_HPP
#include <bitset>
#include <cstdint>
#include <vector>
#include <array>
#include <pair>
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
		entity<sizeof...(components),indexType> e(nextID);
		++nextID;
		if(discardedEntities.empty() == false){
			e.id = discardedEntities.back();
			discardedEntities.pop_back();
		}
		else if (e.id > entities.size()){
			entities.resize(e.id + 1);
		}
		return e.id;
	}

	void deleteEntity(const entityID id){
		//flag component vectors for cleanup
		dirtyflags |= entities[id].mask;
		dirtyTuples.add(entities[id].indicies);
		if(id == nextID - 1){
			entities.pop_back();
			--nextID;
		}
		else{
			entities.rem
		}
	}
private:
	entityID nextID;
	//vector of entities
	std::vector<entity<sizeof...(components),indexType>> entities;
	//tuple of vectors of components
	vectorTuple<components...> componentVectors;
	std::bitset<sizeof...(components)> dirtyFlags;
	std::set<std::pair<std::bitset<sizeof...(components),std::vector<int>>> dirtyTuples;
	std::vector<entityID> discardedEntities;
};
}
#endif