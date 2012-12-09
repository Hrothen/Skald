#include "SystemManager.hpp"
#include <algorithm>
#include "System.hpp"
using namespace skald;

SystemManager::SystemManager():systems(0):lockSystems(false){}
SystemManager::~SystemManager(){}

inline void SystemManager::add(const System& s){
	auto i = std::upper_bound(systems.begin(),systems.end(),s,
		[](const System& a,const System& b){return a->getPriority() < b->getPriority()});
	systems.insert(i,s);
}

inline void SystemManager::remove(const System& s){
	auto i = std::lower_bound(systems.begin(),systems.end(),s,
		[](const System& a,const System& b){return a->getPriority() < b->getPriority()});
	systems.erase(i);
}

inline void SystemManager::update(const double t){
	lockSystems = true;
	for(auto s : systems)
		s->update(t);
	lockSystems = false;
}