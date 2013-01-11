#pragma once
#ifndef SKALD_SYSTEM_HPP
#define SKALD_SYSTEM_HPP
namespace skald{

//Abstract base class for systems
class System{
public:
	System(int p,int t):priority(p),threadGroup(t){};
	~System(){}

	//not copyable
	System(const System&) = delete;
	System& operator=(const System&) = delete;

	//unique identifier for each system class
	virtual const int id()const = 0;

	//init is called once, when the system is registered with the manager
	virtual void init() = 0;
	//destroy is called once, when the system is removed from the manager
	virtual void destroy() = 0;
	//update is called every cycle
	virtual void update(const double) = 0;
	//sets the priority
	void setPriority(const int p){priority = p;}
	const int getPriority()const{return priority;}

protected:
	int priority;		//priority of the system in the queue, lower values are run first
	int threadGroup;	//thread group the system belongs to
};
}//namespace
#endif