#pragma once
#ifndef SKALD_SYSTEM_MANAGER_HPP
#define SKALD_SYSTEM_MANAGER_HPP
#include <vector>
#include <algorithm>
namespace skald{

class System;

//this class manages an ordered list of systems
//IMPORTANT: SystemManager does not own any of the systems it manages
//TODO: system pools for threading
class SystemManager{
public:
	SystemManager():systems(0):lockSystems(false){}
	~SystemManager(){}

	bool canModify()const{return lockSystems;}

	//adds a system to the manager
	void add(const System& s){
		auto i = std::upper_bound(systems.begin(),systems.end(),s,
			[](const System& a,const System& b){return a->getPriority() < b->getPriority()});
		systems.insert(i,s);
	}

	//removes a system from the manager
	void remove(const System& s){
		auto i = std::lower_bound(systems.begin(),systems.end(),s,
			[](const System& a,const System& b){return a->getPriority() < b->getPriority()});
		systems.erase(i);
	}

	//runs each systems update() method
	void update(const double t){
		lockSystems = true;
		for(auto s : systems)
			s->update(t);
		lockSystems = false;
	}

	System& get(const int id){
		for(auto S : systems)
			if(S->getID() == id)
				return S;
	}

private:
	//vector of system pointers, sorted by priority
	std::vector<System*> systems;
	//during an update loop we don't want to allow outside access to the system list
	//TODO: this should be a mutex
	bool lockSystems;
};
}
#endif