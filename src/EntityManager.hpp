#pragma once
#ifndef SKALD_ENTITY_MANAGER_HPP
#define SKALD_ENTITY_MANAGER_HPP
#include <cassert>
#include <bitset>
#include <stdint.h>
#include <vector>
#include <array>
#include <utility>
#include <set>
#include <type_traits>
#include "VectorTuple.hpp"

namespace skald{
//typedef uint16_t entityID;

/*
 * Class representing an entity in the system.
 * For the template parameters, maxComponents is
 * the total number of component classes, while
 * indexType is the smallest integer type that can
 * index the maximum number of entities in the game.
 */
template<size_t maxComponents,class indexType>
class Entity{
	static_assert(std::is_unsigned<indexType>::value == true,
		"indexType must be an unsigned arithmetic type");
	template<class T,class... args>
	friend class EntityManager;
public:
	typedef indexType entityID;

	inline void clear(){
		mask.reset();
		indicies.fill(0);
	}

	entityID id;
	std::bitset<maxComponents> mask;
	std::array<indexType,maxComponents> indicies;
private:
	
	//only EntityManager can create an entity
	explicit Entity(const entityID _id):id{_id},mask(),indicies{} {}
};
/*
 * Class handling a list of entities and all their components.
 * The first template parameter should be the indexing type for
 * your entities, chosen based on the maximum number of entities
 * you plan to have, for instance if you know you'll never need
 * more than 256 entities you can use uint8_t. The remaining
 * template parameters should consist of every component class
 * you intend to use.
 */
template<class indexType,class... components>
class EntityManager{
public:
	static_assert(std::is_unsigned<indexType>::value == true,
		"indexType must be an unsigned arithmetic type");

	typedef Entity<sizeof...(components),indexType> entity;
	typedef indexType entityID;
	typedef std::array<std::vector<indexType>,sizeof...(components)> componentIndexArray;
	template<class Key>
	using getTypeIndex = find_first<VectorTuple<components...>,Key>;

	EntityManager():nextID{0}{}
	~EntityManager(){}

/************************ACCESSORS************************************/
	/* WARNING!
	 * Non const accessors are provided to allow users direct access to
	 * the underlying data, modifying data in this way can really screw
	 * up the operation of the framework if you're not careful.
	 * Prefer not modifying the raw data structures unless you:
	 * 1) Can't do something with a built in function
	 * 2) Are very careful
	 */

	//get the vector of entities
	std::vector<entity> & getEntityVector(){
		return entities;
	}
	const std::vector<entity> & getEntityVector()const{
		return entities;
	}

	//get the set of initialized but unused entities
	std::set<entityID> & getFreeEntites(){
		return freeEntities;
	}
	const std::set<entityID> & getFreeEntites()const{
		return freeEntities;
	}

	//get the VectorTuple of component vectors
	VectorTuple<components...> & getComponentVectors(){
		return componentVectors;
	}
	const VectorTuple<components...> & getComponentVectors()const{
		return componentVectors;
	}

	//get the array of vectors of initialized but unused components
	componentIndexArray & getFreeComponents(){
		return freeComponents;
	}
	const componentIndexArray & getFreeComponents()const{
		return freeComponents;
	}

/***************************END ACCESSORS******************************/

	entityID createEntity(){
		if(freeEntities.empty() == true){
			//check that we're not about to overflow
			assert(nextID != std::numeric_limits<entityID>::max());

			nextID++;
			if(nextID > entities.size())
				entities.resize(nextID,entity(nextID - 1));
			return nextID - 1;
		}
		else{
			auto i = freeEntities.begin();
			entityID e = *i;
			freeEntities.erase(i);
			return e;
		}
	}

	//Sets the ID and components of an entity to be available when creating a new
	//entity, but does not delete the components. If your code makes assumptions
	//about the data layout after removing an entity, you may want to use purgeEntity()
	//instead.
	void removeEntity(const entityID id){
		//check that id is a valid entityID
		assert(freeEntities.count(id) == 0);
		assert(id < nextID);

		if(id == nextID - 1)
			nextID--;
		else
			freeEntities.insert(id);
		//add components to the free components lists
		for(int i = 0; i < entities[id].indicies.size(); ++i){
			if(entities[id].mask[i] == true)
				freeComponents[i].push_back(entities[id].indicies[i]);
		}
		entities[id].clear();
	}

	//deletes an entity's components from the pool and discards it
	//only use this if you really need the components removed, it's slow
	void purgeEntity(const entityID id){
		//check that id is a valid entityID
		assert(freeEntities.count(id) == 0);
		assert(id < nextID);

		deleteComponents(id);
		entities[id].clear();
		if(id == nextID - 1)
			nextID--;
		else
			freeEntities.insert(id);
	}

	//adds a component to the specified entity
	template<class T>
	void addComponent(const entityID e,T component){
		
		const int componentIndex = getTypeIndex<T>::value;
		auto & v = componentVectors.template getByType<T>();
		auto & f = freeComponents[componentIndex];

		//check that id is a valid entityID
		assert(freeEntities.count(e) == 0);
		assert(e < nextID);
		//make sure we aren't inserting a component that's already there
		assert(entities[e].mask[componentIndex] == false);
		
		entities[e].mask.set(componentIndex,true);
		
		//if we have a free component use that, otherwise add one
		if(f.empty() == false){
			v[f.back()] = component;
			entities[e].indicies[componentIndex] = f.back();
			f.pop_back();
		}
		else{
			v.push_back(component);
			entities[e].indicies[componentIndex] = v.size() - 1;
		}
	}

	template<class T,class... Args>
	T& constructComponent(const entityID e,Args&& ...args){

		const int componentIndex = getTypeIndex<T>::value;
		auto & v = componentVectors.template getByType<T>();
		
		T temp{std::forward<Args>(args)...};
		addComponent<T>(e,temp);
		//return a reference to the component in case the user
		//wants to modify it
		return v[entities[e].indicies[componentIndex]];
	}

	//removes a component of type T from the specified entity
	template<class T>
	void removeComponent(const entityID e){
		const int componentIndex = getTypeIndex<T>::value;
		auto & f = freeComponents[componentIndex];

		//check that id is a valid entityID
		assert(freeEntities.count(e) == 0);
		assert(e < nextID);
		//check that the entity actually has that component
		assert(entities[e].mask[componentIndex] == true);

		f.push_back(entities[e].indicies[componentIndex]);
		//Zero is still a valid component index, which is unfortunate.
		//I could deal with this by using signed ints, but then
		//the index array would be much heavier than it needs to be otherwise.
		//Another option is to only allow max_size - 1 elements and use the last
		//integer value as the sentinel
		entities[e].indicies[componentIndex] = 0;
		entities[e].mask.reset(componentIndex);
	}
private:
	//gtest friend declaration so we can access private members in tests
	friend class EntityManagerTests;

	//takes a bitmask and a vector of indicies and deletes all the corresponding
	//components from their lists, also updates the other entities so their
	//index vectors point to the right component
	template<unsigned int Index = sizeof...(components) - 1>
	inline typename std::enable_if<(Index > 0)>::type
	deleteComponents(const entityID id){
		if(entities[id].mask[Index] == true){
			auto & row = componentVectors.template get<Index>();
			auto iter = row.begin();
			std::advance(iter,entities[id].indicies[Index]);
			row.erase(iter);

			//update entities with the new component indicies
			for(auto & e : entities){
				//for each entity, if the entity has the component and its
				//index is greater than the index of the removed component
				//we decrement that index by one
				if(e.id != id
					&& e.mask[Index] == true
					&& e.indicies[Index] > entities[id].indicies[Index])
						e.indicies[Index] -= 1;
			}
		}
		deleteComponents<Index - 1>(id);
	}

	//Base function to end the recursive template
	template<unsigned int Index>
	inline typename std::enable_if<(Index == 0)>::type
	deleteComponents(const entityID id){
		if(entities[id].mask[Index] == true){
			auto & row = componentVectors.template get<Index>();
			auto iter = row.begin();
			std::advance(iter,entities[id].indicies[Index]);
			row.erase(iter);

			//update entities with the new component indicies
			for(auto & e : entities){
				//for each entity, if the entity has the component and its
				//index is greater than the index of the removed component
				//we decrement that index by one
				if(e.id != id
					&& e.mask[Index] == true
					&& e.indicies[Index] > entities[id].indicies[Index])
						e.indicies[Index] -= 1;
			}
		}
	}

	entityID nextID;
	//vector of entities
	std::vector<entity> entities;
	//set of entityIDs that have been discarded and can be reused
	std::set<entityID> freeEntities;
	//tuple of vectors of components
	VectorTuple<components...> componentVectors;
	//array of index sets used to keep track of discarded components
	//TODO: might be better to use a deque stack than vectors, consider it
	std::array<std::vector<indexType>,sizeof...(components)> freeComponents;
};
}//namespace
#endif