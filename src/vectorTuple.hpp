#pragma once
#ifndef VECTOR_TUPLE_HPP
#define VECTOR_TUPLE_HPP
#include <vector>
#include <utility>
#include <boost/concept/assert.hpp>
#include "Component.hpp"
namespace skald{
template<class... components> struct vectorTuple;
template<> struct vectorTuple<> {};
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

template<class Key>
constexpr int getIndexOfType(){
	return 1;
}

template<class Key,class T,class... args>
constexpr int getIndexOfType(){
	return std::is_same<Key,T>::value ? 0 : getIndexOfType<Key,args...>() + 1;
}

/************************************************************/

//get an element at a certain index, used in get<index> to get the elment of a tuple at index
template<int index>
struct getVec{
	template <class ret, class front, class... args>
	static ret getVal(vectorTuple<front,args...>& v){
		return getVec<index - 1>::template getVal<ret,args...>(v.next());
	}

	template<class ret,class front,class... args>
	static const ret getVal(const vectorTuple<front,args...>& v){
		return getVec<index - 1>::template getVal<ret,args...>(v.next());
	}
};

template<>
struct getVec<0>{
	template<class ret,class... args>
	static ret getVal(vectorTuple<args...>& v){
		return v.front();
	}
	template<class ret,class... args>
	static const ret getVal(const vectorTuple<args...>& v){
		return v.front();
	}
};

/***************************************************************/

//A tuple of std::vectors of the given elements
//getting the type of an std::tuple of vectors of types at compile time
//turns out to be really hard, so we use our own tuple class

template<class T, class... components>
struct vectorTuple<T,components...> : private vectorTuple<components...>{
	template<int index> using indexedVec = std::vector<typename typeAtIndex<index,T,components...>::type>;

	//assert each component type satisfies component concept
	BOOST_CONCEPT_ASSERT((Component<T>));

	vectorTuple():vectorTuple<components...>(),_front()
	{
	}

	//get the vector at index i
	template<int index>
	std::vector<typename typeAtIndex<index,T,components...>::type>
	get()const{
		//return getVec<index>::template getVal<std::vector<typename typeAtIndex<index,T,components...>::type>,T,components...>(*this);
		return getVec<index>::template getVal<indexedVec<index>,T,components...>(*this);
	}

	template<int index>
	std::vector<typename typeAtIndex<index,T,components...>::type>&
	get(){
		//return getVec<index>::template getVal<std::vector<typename typeAtIndex<index,T,components...>::type>&,T,components...>(*this);
		return getVec<index>::template getVal<indexedVec<index>&,T,components...>(*this);
	}

	//get the first vector containing objects of type Key
	template<class Key>
	std::vector<Key> getByType()const{
		constexpr int index = getIndexOfType<Key,components...>();
		static_assert(index < sizeof...(components),
			"Compilation error, requested type not found in the list of components");
		return get<index>();
	}

	template<class Key>
	std::vector<Key>& getByType(){
		constexpr int index = getIndexOfType<Key,components...>();
		static_assert(index < sizeof...(components),
			"Compilation error, requested type not found in the list of components");
		return get<index>();
	}

	std::vector<T> front()const{return _front;}
	std::vector<T>& front(){return _front;}
	const vectorTuple<components...>& next()const{return *this;}
private:
	std::vector<T> _front;
};

}//namespace
#endif