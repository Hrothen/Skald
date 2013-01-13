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
	typedef System<indexType,components...> sysType;
	World(){}
	~World(){}

	inline entityID addEntity(){
		return entities.createEntity();
	}

	inline void removeEntity(const entityID e,bool purge = false){
		purge ? entities.purgeEntity(e) : entities.removeEntity(e);
	}

	template<class T>
	inline void addComponent(const entityID e,T&& c){
		entities.addComponent(e,std::forward<T>(c));
	}

	template<class T>
	inline void removeComponent(const entityID e){
		entities.removeComponent<T>(e);
	}

	inline void addSystem(const SystemPtr s){
		systems.add(s);
		s->init(*this);
	}

	inline void removeSystem(const SystemPtr s){
		systems.remove(s);
		s->destroy(*this);
	}

	inline SystemPtr getSystem(const int id){
		return systems.get(id);
	}

	inline void update(const double t){
		systems.update(t);
	}
private:
	SystemManager<indexType,components...> systems;
	EntityManager<indexType,components...> entities;
};
}//namespace
#endif