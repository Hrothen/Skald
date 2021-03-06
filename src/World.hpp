#pragma once
#ifndef SKALD_WORLD_HPP
#define SKALD_WORLD_HPP

#include "EntityManager.hpp"
#include "SystemManager.hpp"

namespace skald{

//controller class holding the system manager and entity manager
//this class is the user's interface into the entity-component system
template<class indexType,class... components>
class World
{
public:
	typedef indexType entityID;
	typedef typename SystemManager<indexType,components...>::SystemPtr SystemPtr;
	typedef System<indexType,components...> SystemBase;
	World(){}
	~World(){}

	//create a new entity and return its handle
	inline entityID addEntity(){
		return entities.createEntity();
	}

	//remove an entity from the manager, if purge == true its components
	//are destroyed, otherwise they are made availible for reuse
	inline void removeEntity(const entityID e,bool purge = false){
		purge ? entities.purgeEntity(e) : entities.removeEntity(e);
	}

	//Attach a component of type T to the given entity
	template<class T>
	inline void addComponent(const entityID e,T&& c){
		entities.addComponent(e,std::forward<T>(c));
	}

	//Construct a component of type T and attach it to the given entity
	template<class T, class... Args>
	inline T& constructComponent(const entityID e,Args&& ...args){
		return entities.template constructComponent<T>(e,std::forward<Args>(args)...);
	}

	//Remove a component of Type T from the given entity
	template<class T>
	inline void removeComponent(const entityID e){
		entities.template removeComponent<T>(e);
	}

	//Add a new system to the manager
	inline void addSystem(const SystemPtr s){
		systems.add(s);
		s->init(this);
	}

	//Remove a system from the manager 
	inline void removeSystem(const SystemPtr s){
		systems.remove(s);
		s->destroy(this);
	}

	//Get a pointer to the system with the given id, if it exists
	//nullptr is returned if the System is not found
	inline SystemPtr getSystem(const int id){
		return systems.get(id);
	}

	//Call the update() function of each system
	inline void update(const double t){
		systems.update(t);
	}

/********************************ACCESSORS************************************/

	SystemManager<indexType,components...>& getSystemManager(){
		return systems;
	}

	const SystemManager<indexType,components...>& getSystemManager()const{
		return systems;
	}

	EntityManager<indexType,components...>& getEntityManager(){
		return entities;
	}

	const EntityManager<indexType,components...>& getEntityManager()const{
		return entities;
	}

/***********************************END ACCESSORS*****************************/
private:
	SystemManager<indexType,components...> systems;
	EntityManager<indexType,components...> entities;
};
}//namespace
#endif