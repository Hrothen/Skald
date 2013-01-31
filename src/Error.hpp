#pragma once
#ifndef SKALD_ERROR_HPP
#define SKALD_ERROR_HPP

#include <numeric_limits>
namespace skald{
	
	template <class T>
	struct SKALD_ERROR{
		static const T ENTITY_OVERFLOW = std::numeric_limits<T>::max();
		static const int SUCCESS = 0;
		static const int FAILURE = -1;
		static const int ENTITY_NOT_FOUND_IN_LIST = 1;
		static const int COMPONENT_OVERFLOW = 2;
		static const int COMPONENT_ALREADY_ATTACHED = 3;
		static const int COMPONENT_NOT_ATTACHED = 4;
		static const int SYSTEM_ALREADY_IN_LIST = 5;
		static const int SYSTEM_NOT_FOUND_IN_LIST = 6;
		static const auto SYSTEM_NOT_IN_LIST = nullptr;
	};
}//namespace
#endif