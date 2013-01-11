#pragma once
#ifndef VECTOR_TUPLE_HPP
#define VECTOR_TUPLE_HPP
#include <vector>
#include <utility>
#include <boost/concept/assert.hpp>
#include "Component.hpp"
namespace skald{
template<class... components> struct VectorTuple;
template<> struct VectorTuple<> {};
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

//found this code on stackoverflow at
//http://stackoverflow.com/questions/6032089/position-of-a-type-in-a-variadic-template-parameter-pack/6032645#6032645

//find_first has an element value holding the index of the first element
//of type T in the paramter pack of the given Tuple type

template<class Tuple, class T, std::size_t Index = 0>
struct find_first;

//find_first_final_test is used to check that the type exists in the parameter pack
//a compile time error will be thrown if the type isn't found
template<std::size_t Index,bool Valid>
struct find_first_final_test : public std::integral_constant<std::size_t,Index>{};

template<std::size_t Index>
struct find_first_final_test<Index,false>{
	static_assert(Index == -1, "Type not found in paramater pack");
};

template<class Head, class T, std::size_t Index>
struct find_first<VectorTuple<Head>,T,Index>
	: public find_first_final_test<Index, std::is_same<Head,T>::value>{};

template<class Head, class... Rest, class T, std::size_t Index>
struct find_first<VectorTuple<Head,Rest...>,T,Index>
	: public std::conditional<std::is_same<Head,T>::value,
					std::integral_constant<std::size_t,Index>,
					find_first<VectorTuple<Rest...>,T,Index+1>>::type
{	
};

/************************************************************/

//get an element at a certain index, used in get<index> to get the elment of a tuple at index
template<int index>
struct getVec{
	template <class ret, class front, class... args>
	static ret getVal(VectorTuple<front,args...>& v){
		return getVec<index - 1>::template getVal<ret,args...>(v.next());
	}

	template<class ret,class front,class... args>
	static const ret getVal(const VectorTuple<front,args...>& v){
		return getVec<index - 1>::template getVal<ret,args...>(v.next());
	}
};

template<>
struct getVec<0>{
	template<class ret,class... args>
	static ret getVal(VectorTuple<args...>& v){
		return v.front();
	}
	template<class ret,class... args>
	static const ret getVal(const VectorTuple<args...>& v){
		return v.front();
	}
};

/***************************************************************/

//A tuple of std::vectors of the given elements
//getting the type of an std::tuple of vectors of types at compile time
//turns out to be really hard, so we use our own tuple class

template<class T, class... components>
struct VectorTuple<T,components...> : private VectorTuple<components...>{
	template<int index> using indexedVec = std::vector<typename typeAtIndex<index,T,components...>::type>;

	//assert each component type satisfies component concept
	BOOST_CONCEPT_ASSERT((Component<T>));

	VectorTuple():VectorTuple<components...>(),_front()
	{
	}

	//get the vector at index i
	template<int index>
	const std::vector<typename typeAtIndex<index,T,components...>::type>&
	get()const{
		//return getVec<index>::template getVal<std::vector<typename typeAtIndex<index,T,components...>::type>,T,components...>(*this);
		return getVec<index>::template getVal<indexedVec<index>&,T,components...>(*this);
	}

	template<int index>
	std::vector<typename typeAtIndex<index,T,components...>::type>&
	get(){
		//return getVec<index>::template getVal<std::vector<typename typeAtIndex<index,T,components...>::type>&,T,components...>(*this);
		return getVec<index>::template getVal<indexedVec<index>&,T,components...>(*this);
	}

	//get the first vector containing objects of type Key
	template<class Key>
	const std::vector<Key>& getByType()const{
		const int index = find_first<VectorTuple<T,components...>,Key>::value;
		//static_assert(index < sizeof...(components),
		//	"Compilation error, requested type not found in the list of components");
		return get<index>();
	}

	template<class Key>
	std::vector<Key>& getByType(){
		const int index = find_first<VectorTuple<T,components...>,Key>::value;
		//static_assert(index < sizeof...(components),
		//	"Compilation error, requested type not found in the list of components");
		return get<index>();
	}

	const std::vector<T>& front()const{return _front;}
	std::vector<T>& front(){return _front;}
	
	const VectorTuple<components...>& next()const{return *this;}
	VectorTuple<components...>& next(){return *this;}
private:
	std::vector<T> _front;
};

}//namespace
#endif