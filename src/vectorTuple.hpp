#pragma once
#ifndef VECTOR_TUPLE_HPP
#define VECTOR_TUPLE_HPP
#include <vector>
#include <boost/concept/assert,hpp>
#include "Component.hpp"
namespace skald{
//there's probably a c++11 function to do this already
//deduces the type of an element in a variadic pack
template<int index,class... args> struct typeAtIndex;

template<int index, class T, class... args>
struct typeAtIndex<index,T,args...>{
	static_assert(index < sizeof...(args) + 1,"index too large, out of bounds");
	typedef typename typeAtIndex<index - 1, args...>::type type;
};

template<class T, class... args>
struct typeAtIndex<0,T,args...>{
	typedef T type;
};

/************************************************************/

//finds the first occurence of a given type within a variadic pack
template<int I,class Key,class T,class... args>
struct indexOfType<I,Key,T,args...>{
	static_assert(I < sizeof...(args) + 1, "key not in list");
	static const int index = indexOfType<I+1,Key,args...>::index;
};

template<int I,class Key,Key K,class... args>
struct indexOfType<I,Key,K,args...>{
	static const int index = I;
};

template<int I,class... args> struct indexOfType;

/************************************************************/

//get an element at a certain index, used in get<index> to get the elment of a tuple at index
template<int index>
struct getVec{
	template <class ret, class front, class... args>
	static ret getVal(vectorTuple<front,args...>& v){
		return getVec<index - 1>::template getValue<ret,args...>(v.next());
	}

	template<class ret,class first,class... args>
	static const ret getVal(const vectorTuple<front,args...>& v){
		return getVec<index - 1>::template getVal<ret,args...>(v.next());
	}
};

template<>
struct getVec<0>{
	template<class ret,class... args>
	static ret getVal(vectorTuple<args...>& v){
		return v.head();
	}
	template<class ret,class... args>
	static const ret getVal(const vectorTuple<args...>& v){
		return v.head();
	}
};

/***************************************************************/

//A tuple of std::vectors of the given elements
//getting the type of an std::tuple of vectors of types at compile time
//turns out to be really hard, so we use our own tuple class
template<class... components> struct vectorTuple;

template<> struct vectorTuple<> {};

template<class T, class... components>
struct vectorTuple<T,components...> : private vectorTuple<components...>{

	//assert each component type satisfies component concept
	BOOST_CONCEPT_ASSERT((Component<T>));

	vectorTuple(const T& f, const components&... rest):
		vectorTuple<components...>(rest...),_front(f)
	{
	}

	//get the vector at index i
	template<int index>
	typename typeAtIndex<index,T,components...>::type
	get()const{
		return getVec<index>::template getVal<typename typeAtIndex<index,T,components...>::type,T,components...>(*this);
	}

	template<int index>
	typename typeAtIndex<index,T,components...>::type&
	get(){
		return getVec<index>::template getVal<typename typeAtIndex<index,T,components...>::type,T,components...>(*this);
	}

	//get the first vector containing objects of type Key
	template<class Key>
	std::vector<Key> get()const{
		return get<indexOfType<Key>>();
	}

	template<class Key>
	std::vector<Key>& get(){
		return get<indexOfType<Key>>()
	}

	T front()const{return _front;}
	const vectorTuple<components...>& next()const{return *this;}
private:
	T _front;
};

}
#endif