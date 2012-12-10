#pragma once
#ifndef SKALD_ENTITY_MANAGER_HPP
#define SKALD_ENTITY_MANAGER_HPP
#include <assert>
#include <bitset>
#include <cstdint>
#include <vector>
#include <array>
#include <pair>
#include <set>
#include <type_traits>
#include "Components.hpp"
#include "vectorTuple.hpp"

namespace skald{
typedef uint16_t entityID;

/*
 * Class representing an entity in the system.
 * For the template parameters, maxComponents is
 * the total number of component classes, while
 * indexType is the smallest integer type that can
 * index the maximum number of entities in the game
 * the default is one byte, allowing 256 entities, which might
 * be a bit low for some users.
 */
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
		if(freeEntities.empty() == true){
			nextID++;
			return nextID - 1;
		}
		else{
			auto i = freeEntities.begin();
			entityID e = *i;
			freeEntities.erase(i);
		}
	}

	//Sets the ID and components of an entity to be availible when creating a new
	//entity, but does not delete the components. If your code makes assumptions
	//about the data layout after removing an entity, you may want to use purgeEntity()
	//instead.
	void removeEntity(const entityID id){
		if(id == nextID - 1)
			nextID--;
		else
			freeEntities.add(id);
		int acc = 0;
		for(int i : entities[id].indicies){
			//this only happens if we've somehow managed to insert an index without
			//updating the mask
			assert(acc >= entities[id].mask.size());
			while (entities[id].mask[acc] == false)
				acc++;
			//FIXME: this assumes components have been passed in to the
			//manager in order by their id field
			freeComponents[acc].push_back(i);
		}
		entities[id].mask.reset();
		entities[id].indicies.clear();
	}

	//deletes an entity's components from the pool and discards it
	//only use this if you really need the components removed, it's slow
	void purgeEntity(const entityID id){
		deleteComponents(entities[id].mask.to_ulong(),entities[id].indicies);
		entities[id].mask.reset();
		entities[id].indicies.clear();
		if(id == nextID - 1)
			nextID--;
		else
			freeEntities.add(id);
	}

	//adds a component to the specified entity
	template <class T>
	void addComponent(const entityID e,T&& component){
		auto & v = componentVectors.get<T>();
		auto & f = freeComponents[indexOfType<T>::index];
		const std::bitset<entities[e].mask.size()> b(std::forward<T>(component).id);
		entities[e].mask |= b;
		int acc = 0;
		for(int i = 0,i < b.size(), ++i){
			if(entities[e].mask[i] == true)
				acc++;
			if (b[i] == true)
				break;
		}
		if(f.empty() == false){
			v[f.back()] = component;
			entities[e].indicies.insert(acc,f.back());
			f.pop_back();
		}
		else{
			v.push_back(component);
			entities[e].indicies.insert(acc,v.size());
		}
	}

	template<class T>
	void removeComponent(const entityID e,T&& component){
	}
private:

	//takes a bitmask and a vector of indicies and deletes all the corresponding
	//components from their lists, also updates the other entities so their
	//index vectors point to the right component
	template<unsigned int Index = sizeof...(components) - 1>
	inline typename std::enable_if<Index >= 0>::type
	deleteComponents(const unsigned long b,const std::vector<indexType>& v){
		if(b & 1UL<<Index){
			auto & row = componentVectors.get<Index>();
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
	//set of entityIDs that have been discarded and can be reused
	std::set<entityID> freeEntities;
	//tuple of vectors of components
	vectorTuple<components...> componentVectors;
	//array of index sets used to keep track of discarded components
	//TODO: might be better to use a deque stack than vectors, consider it
	std::array<std::std::vector<indexType>,sizeof...(components)> freeComponents;
};
}
#endif