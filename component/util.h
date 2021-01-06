#ifndef KID_TOOLKIT_UTIL_H
#define KID_ALLOCATOR_H

/**
 * match the best type.
 */
template<typename... Args>
struct Matcher;

template<>
struct Matcher<>{
	 static void match(...);
};

template<typename T, typename... Args>
struct Matcher<T, Args...> : public Matcher<Args...>{
	static T match(T);
	using Matcher<Args...>::match;
};

template<typename T, typename... Types>
struct BestMatch{
	using Type = decltype(Matcher<T, Types...>::match(declval<T>()));
};





#endif