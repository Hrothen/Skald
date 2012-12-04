#include "SystemManager.hpp"
#include <algorithm>
using namespace skald;

SystemManager::SystemManager():systems(0):lockSystems(false){}
SystemManager::~SystemManager(){}

void SystemManager::add(const System& s){
	auto i = std::upper_bound(systems.begin(),systems.end(),s,
		[](const System& a,const System& b){return a.getPriority() < b.getPriority()});
	systems.insert(i,s);
}

void SystemManager::remove(const System& s){
	auto i = std::lower_bound(systems.begin(),systems.end(),s,
		[](const System& a,const System& b){return a.getPriority() < b.getPriority()});
	systems.erase(i);
}

void SystemManager::update(const double t){
	lockSystems = true;
	for(auto s : systems)
		s.update(t);
	lockSystems = false;
}