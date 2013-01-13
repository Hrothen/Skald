#pragma once
#ifndef SKALD_SYSTEM_MANAGER_HPP
#define SKALD_SYSTEM_MANAGER_HPP
#include <vector>
#include <algorithm>
#include <memory>
#include "System.hpp"
namespace skald{

template<class sys>
struct Comparator{
	bool operator()(const sys a,const sys b){
		return a->getPriority() < b->getPriority();
	}
	bool operator()(const sys a,const int b){
		return a->getPriority() < b;
	}
};

//this class manages an ordered list of systems
//TODO: system pools for threading
template<class... args>
class SystemManager{
public:
	typedef std::shared_ptr<System<args...>> SystemPtr;
	SystemManager():systems(0),lockSystems(false){}
	~SystemManager(){}

	bool canModify()const{return lockSystems;}

	//adds a system to the manager
	void add(const SystemPtr s){
		auto i = std::upper_bound(systems.begin(),systems.end(),
			s,Comparator<SystemPtr>());
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
	void remove(const SystemPtr s){
		auto i = std::lower_bound(systems.begin(),systems.end(),
			s,Comparator<SystemPtr>());
		if(i != systems.end())
			systems.erase(i);
	}

	void remove(const int id){
		auto i = std::lower_bound(systems.begin(),systems.end(),
			id,Comparator<SystemPtr>());
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

	SystemPtr get(const int id){
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
	std::vector<SystemPtr> systems;
	//during an update loop we don't want to allow outside access to the system list
	//TODO: this should be a mutex
	bool lockSystems;
};
}//namespace
#endif