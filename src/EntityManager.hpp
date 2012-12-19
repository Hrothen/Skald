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
class Entity{
	template<class T,class... args>
	friend class EntityManager;
public:
	entityID id;
private:

	FRIEND_TEST(EntityManagerTests,PurgeEntity);
	explicit Entity(const entityID _id):id{_id},mask(),indicies() {}

	inline void clear(){
		mask.reset();
		indicies.clear();
	}

	std::bitset<maxComponents> mask;
	std::vector<indexType> indicies;
};

template<class indexType = uint8_t,class... components>
class EntityManager{
public:
	typedef Entity<sizeof...(components),indexType> entity;

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
		//entities[id].mask.reset();
		//entities[id].indicies.clear();
		entities[id].clear();
	}

	//deletes an entity's components from the pool and discards it
	//only use this if you really need the components removed, it's slow
	void purgeEntity(const entityID id){
		deleteComponents(entities[id].mask.to_ulong(),entities[id].indicies);
		//entities[id].mask.reset();
		//entities[id].indicies.clear();
		entities[id].clear();
		if(id == nextID - 1)
			nextID--;
		else
			freeEntities.insert(id);
	}

	//adds a component to the specified entity
	template <class T>
	void addComponent(const entityID e,T&& component){
		auto & v = componentVectors.template getByType<T>();
		auto & f = freeComponents[getIndexOfType<T,components...>()];
		const std::bitset<sizeof...(components)> b(std::forward<T>(component).id);
		entities[e].mask |= b;
		auto acc = entities[e].indicies.begin();
		for(int i = 0;i < b.size(); ++i){
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
		int acc = 0;
		for(acc; acc < entities[e].mask.size(); ++acc)
			if(std::forward<T>(component).id >> acc == 1)
				break;
		int i = onesBelowIndex(entities[e].mask,acc);
		auto & f = freeComponents[getIndexOfType<T,components...>()];
		auto & v = componentVectors.template get<T>();
		f.push_back(entities[e].indicies[i + 1]);
		entities[e].indicies.erase(i+1);
		entities[e].mask.reset(acc);
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


	int onesBelowIndex(const std::bitset<sizeof...(components)> mask,const int index){
		int acc = 0;
		for(int i = index - 1; i >= 0; --i){
			if(mask[i] == true)
				acc++;
		}
		return acc;
	}

	//takes a bitmask and a vector of indicies and deletes all the corresponding
	//components from their lists, also updates the other entities so their
	//index vectors point to the right component
	template<unsigned int Index = sizeof...(components) - 1>
	inline typename std::enable_if<Index >= 0>::type
	deleteComponents(const unsigned long b,std::vector<indexType>& v){
		if(b & 1UL<<Index){
			auto & row = componentVectors.template get<Index>();
			auto iter = row.begin();
			std::advance(iter,v.back());
			row.erase(iter);
			//update entities with new component indicies
			for(auto e : entities){
				//this is pretty ugly, we need to check if each entity
				//has the component, then work out where it is in the tuple,
				//then check if it needs to be updated
				if (e.mask[Index] == true){
					int acc = 0;
					for(int i = e.mask.size() - 1 ; i > Index; i--){
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
	deleteComponents(const unsigned long b,const std::vector<indexType>& v){}

	entityID nextID;
	//vector of entities
	std::vector<entity> entities;
	//set of entityIDs that have been discarded and can be reused
	std::set<entityID> freeEntities;
	//tuple of vectors of components
	vectorTuple<components...> componentVectors;
	//array of index sets used to keep track of discarded components
	//TODO: might be better to use a deque stack than vectors, consider it
	std::array<std::vector<indexType>,sizeof...(components)> freeComponents;
};
}
#endif