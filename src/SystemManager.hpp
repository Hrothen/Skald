#pragma once
#ifndef SKALD_SYSTEM_MANAGER_HPP
#define SKALD_SYSTEM_MANAGER_HPP
#include <vector>
namespace skald{

class System;

//this class manages an ordered list of systems
//IMPORTANT: SystemManager does not own any of the systems it manages
//TODO: system pools for threading
class SystemManager{
public:
	SystemManager();
	~SystemManager();

	bool canModify()const{return lockSystems;}

	//adds a system to the manager
	void add(const System&);

	//removes a system from the manager
	void remove(const System&);

	//runs each systems update() method
	void update(const double);

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