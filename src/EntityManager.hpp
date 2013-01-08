#pragma once
#ifndef SKALD_ENTITY_MANAGER_HPP
#define SKALD_ENTITY_MANAGER_HPP
#include <cassert>
#include <bitset>
#include <cstdint>
#include <vector>
#include <array>
#include <utility>
#include <set>
#include <type_traits>
#include "VectorTuple.hpp"

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
class Entity{
	template<class T,class... args>
	friend class EntityManager;
public:
	entityID id;
private:

	FRIEND_TEST(EntityManagerTests,PurgeEntity);
	FRIEND_TEST(EntityManagerTests,CreateEntity);
	FRIEND_TEST(EntityManagerTests,AddComponent);
	FRIEND_TEST(EntityManagerTests,RemoveComponent);
	explicit Entity(const entityID _id):id{_id},mask(),indicies{} {}

	inline void clear(){
		mask.reset();
		indicies.fill(0);
	}

	std::bitset<maxComponents> mask;
	std::array<indexType,maxComponents> indicies;
};

template<class indexType = uint8_t,class... components>
class EntityManager{
public:
	typedef Entity<sizeof...(components),indexType> entity;
	template<class Key>
	using getTypeIndex = find_first<VectorTuple<components...>,Key>;

	EntityManager():nextID{0}{}
	~EntityManager(){}

	entityID createEntity(){
		if(freeEntities.empty() == true){
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

	//Sets the ID and components of an entity to be availible when creating a new
	//entity, but does not delete the components. If your code makes assumptions
	//about the data layout after removing an entity, you may want to use purgeEntity()
	//instead.
	void removeEntity(const entityID id){
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
		deleteComponents(entities[id].mask,entities[id].indicies);
		entities[id].clear();
		if(id == nextID - 1)
			nextID--;
		else
			freeEntities.insert(id);
	}

	//adds a component to the specified entity
	template <class T>
	void addComponent(const entityID e,T component){
		const int componentIndex = getTypeIndex<T>::value;
		auto & v = componentVectors.template getByType<T>();
		auto & f = freeComponents[componentIndex];

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

	template<class T>
	void removeComponent(const entityID e,T component){
		const int componentIndex = getTypeIndex<T>::value;
		auto & f = freeComponents[componentIndex];
		auto & v = componentVectors.template getByType<T>();
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
	//gtest friend declarations so we can access private members in tests
	friend class EntityManagerTests;
	FRIEND_TEST(EntityManagerTests,DefaultConstructor);
	FRIEND_TEST(EntityManagerTests,CreateEntity);
	FRIEND_TEST(EntityManagerTests,RemoveEntity);
	FRIEND_TEST(EntityManagerTests,PurgeEntity);
	FRIEND_TEST(EntityManagerTests,ReuseEntity);
	FRIEND_TEST(EntityManagerTests,AddComponent);
	FRIEND_TEST(EntityManagerTests,RemoveComponent);

	/*
	int onesBelowIndex(const std::bitset<sizeof...(components)> mask,const int index){
		int acc = 0;
		for(int i = index - 1; i >= 0; --i){
			if(mask[i] == true)
				acc++;
		}
		return acc;
	}
	*/

	//takes a bitmask and a vector of indicies and deletes all the corresponding
	//components from their lists, also updates the other entities so their
	//index vectors point to the right component
	template<unsigned int Index = sizeof...(components) - 1>
	inline typename std::enable_if<(Index > 0),void>::type
	deleteComponents(const std::bitset<sizeof...(components)>& b,
					std::array<indexType,sizeof...(components)>& indicies){
		if(b[Index] == true){
			auto & row = componentVectors.template get<Index>();
			auto iter = row.begin();
			std::advance(iter,indicies[Index]);
			row.erase(iter);

			//update entities with the new component indicies
			for(auto e : entities){
				//for each entity, if the entity has the component and its
				//index is greater than the index of the removed component
				//we decrement that index by one
				if(e.mask[Index] == true){
					if(e.indicies[Index] > indicies[Index])
						e.indicies[Index]--;
				}
			}
		}
		deleteComponents<Index - 1>(b,indicies);
	}

	//Base function to end the recursive template
	template<unsigned int Index>
	inline typename std::enable_if<Index == 0,void>::type
	deleteComponents(const std::bitset<sizeof...(components)>& b,
					std::array<indexType,sizeof...(components)>& indicies){
		if(b[Index] == true){
			auto & row = componentVectors.template get<Index>();
			auto iter = row.begin();
			std::advance(iter,indicies[Index]);
			row.erase(iter);

			//update entities with the new component indicies
			for(auto e : entities){
				//for each entity, if the entity has the component and its
				//index is greater than the index of the removed component
				//we decrement that index by one
				if(e.mask[Index] == true){
					if(e.indicies[Index] > indicies[Index])
						e.indicies[Index]--;
				}
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
}
#endif