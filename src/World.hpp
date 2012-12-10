#pragma once
#ifndef SKALD_WORLD_HPP
#define SKALD_WORLD_HPP

#include "EntityManager.hpp"
#include "SystemManager.hpp"

namespace skald{

//controller class holding the system manager and entity manager
//this class is the user's interface into the entity-component system
template<class indexType = uint8_t,class... components>
class World
{
public:
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
	inline void removeComponent(const entityID e,T&& c){
		entities.removeComponent(e,std::forward<T>(c));
	}

	inline void addSystem(const System& s){
		systems.addSystem(s);
		s.init();
	}

	inline void removeSystem(const System& s){
		systems.remove(s);
		s.destroy();
	}

	inline System& getSystem(const int id){
		return systems.get(id);
	}

	inline void update(const double t){
		systems.update(t);
	}
private:
	SystemManager systems;
	EntityManager<indexType,components...> entities;
};
}
#endif