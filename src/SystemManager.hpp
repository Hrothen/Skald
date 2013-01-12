#pragma once
#ifndef SKALD_SYSTEM_MANAGER_HPP
#define SKALD_SYSTEM_MANAGER_HPP
#include <vector>
#include <algorithm>
#include <memory>
#include "System.hpp"
namespace skald{

struct Comparator{
	bool operator()(const std::shared_ptr<System> a,const std::shared_ptr<System> b){
		return a->getPriority() < b->getPriority();
	}
	bool operator()(const std::shared_ptr<System> a,const int b){
		return a->getPriority() < b;
	}
};

//this class manages an ordered list of systems
//IMPORTANT: SystemManager does not own any of the systems it manages
//TODO: system pools for threading
class SystemManager{
public:
	SystemManager():systems(0),lockSystems(false){}
	~SystemManager(){}

	bool canModify()const{return lockSystems;}

	//adds a system to the manager
	void add(const std::shared_ptr<System> s){
		auto i = std::upper_bound(systems.begin(),systems.end(),s,Comparator());
		if(i == systems.begin())
			systems.insert(i,s);
		else{
			//check that the system isn't already in systems
			auto j = i;
			j--;
			if((*j)->id() != s->id())
				systems.insert(i,s);
		}
	}

	//removes a system from the manager
	void remove(const std::shared_ptr<System> s){
		auto i = std::lower_bound(systems.begin(),systems.end(),s,Comparator());
		if(i != systems.end())
			systems.erase(i);
	}

	void remove(const int id){
		auto i = std::lower_bound(systems.begin(),systems.end(),id,Comparator());
		if(i != systems.end())
			systems.erase(i);
	}

	//runs each systems update() method
	void update(const double t){
		lockSystems = true;
		for(auto s : systems)
			s->update(t);
		lockSystems = false;
	}

	std::shared_ptr<System> get(const int id){
		for(auto S : systems)
			if(S->id() == id)
				return S;
		return nullptr;
	}

private:

	friend class SystemTests;
	FRIEND_TEST(SystemTests,AddSystem);
	FRIEND_TEST(SystemTests,RemoveSystem);
	FRIEND_TEST(SystemTests,Update);
	FRIEND_TEST(SystemTests,Get);

	//vector of system pointers, sorted by priority
	std::vector<std::shared_ptr<System>> systems;
	//during an update loop we don't want to allow outside access to the system list
	//TODO: this should be a mutex
	bool lockSystems;
};
}//namespace
#endif