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

	inline entityID addEntity(){}

	inline void removeEntity(entityID){}

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