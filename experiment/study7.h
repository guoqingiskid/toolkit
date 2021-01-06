template<typename... Elements>
class TypeList {};

/**
 * Get TypeList Front.
 */
template<typename List>
struct FrontT;

template<typename Head, typename... Tail>
struct FrontT<TypeList<Head, Tail...>> {
	using Type = Head;
};

template<typename List>
using Front = typename FrontT<List>::Type;


/**
 * Pop TypeList Front. 
 */
template<typename List>
struct PopFrontT;

template<typename Head, typename... Tail>
struct PopFrontT<TypeList<Head, Tail...>> {
	using Type = TypeList<Tail...>;
};

template<typename List>
using PopFront = typename PopFrontT<List>::Type;


/**
 * Push TypeList Front. 
 */
template<typename List, typename Type>
struct PushFrontT;

template<typename... Elements, typename NewElement>
struct PushFrontT<TypeList<Elements...>, NewElement> {
	using Type = TypeList<NewElement, Elements...>;
};

template<typename List, typename NewElement>
using PushFront = typename PushFrontT<List, NewElement>::Type;

/**
 * Index of TypeList.
 */
template<typename List, int Index>
struct IndexOfT;

template<typename... Elements, int Index>
struct IndexOfT<TypeList<Elements...>, Index> : public IndexOfT<PopFront<TypeList<Elements...>>, Index - 1> {};

template<typename... Elements>
struct IndexOfT<TypeList<Elements...>, 0> : public FrontT<TypeList<Elements...>> {};

template<typename List, int Index>
using IndexOf = typename IndexOfT<List, Index>::Type;


/**
 * Match the Largest Type From TypeList.  
 */
template<bool Con, typename T1, typename T2>
struct IfThenElseT {
	using Type = T1;
};

template<typename T1, typename T2>
struct IfThenElseT<false, T1, T2> {
	using Type = T2;
};

template<bool Con, typename T1, typename T2>
using IfThenElse = typename IfThenElseT<Con, T1, T2>::Type;

template<typename List>
struct LargestTypeT;

template<typename... Elements>
struct LargestTypeT<TypeList<Elements...>> {
private:
	using TL = TypeList<Elements...>;
	using Fst = Front<TL>;
	using Rest = typename LargestTypeT<PopFront<TL>>::Type;
public:
	using Type = IfThenElse<sizeof(Fst) >= sizeof(Rest), Fst, Rest>;
};

template<>
struct LargestTypeT<TypeList<>> {
	using Type = char;
};

template<typename List>
using LargestType = typename LargestTypeT<List>::Type;


/**
 * Length of TypeList.
*/
template<typename List>
struct Length;

template<typename... Elements>
struct Length<TypeList<Elements...>> {
	static constexpr int Value = 1 + Length<PopFront<TypeList<Elements...>>>::Value;
};

template<>
struct Length<TypeList<>> {
	static constexpr int Value = 0;
};


/**
 * Empty of TypeList.
*/
template<typename List>
struct IsEmpty{};

template<typename... Elements>
struct IsEmpty<TypeList<Elements...>> {
	static constexpr bool Value = false;
};

template<>
struct IsEmpty<TypeList<>> {
	static constexpr bool Value = true;
};


/**
 * Push TypeList Back.  
*/
template<typename List, typename NewElement>
struct PushBackT;

template<typename... Elements, typename NewElement>
struct PushBackT<TypeList<Elements...>, NewElement> {
	using Type = TypeList<Elements..., NewElement>;
};

template<typename List, typename NewElement>
using PushBack = typename PushBackT<List, NewElement>::Type;


/**
 * Reverse TypeList. 
*/
template<typename List>
struct ReverseT;

template<typename... Elements>
struct ReverseT<TypeList<Elements...>> : public PushBackT<typename ReverseT<PopFront<TypeList<Elements...>>>::Type, Front<TypeList<Elements...>>> {};

template<>
struct ReverseT<TypeList<>> {
	using Type = TypeList<>;
};

template<typename List>
using Reverse = typename ReverseT<List>::Type;


/**
 * Pop Typelist Back. 
*/
template<typename List>
struct PopBackT;

template<typename... Elements>
struct PopBackT<TypeList<Elements...>> {
	using Type = Reverse<PopFront<Reverse<TypeList<Elements...>>>>;
};

template<typename List>
using PopBack = typename PopBackT<List>::Type;


/**
 * Transform TypeList. 
*/
template<typename List, template<typename T>class MetaFun>
struct TransformT;

template<typename... Elements, template<typename T>class MetaFun>
struct TransformT<TypeList<Elements...>, MetaFun> : public PushFrontT<typename TransformT<PopFront<TypeList<Elements...>>, MetaFun>::Type,
	typename MetaFun<Front<TypeList<Elements...>>>::Type> {};

template<template<typename T>class MetaFun>
struct TransformT<TypeList<>, MetaFun> {
	using Type = TypeList<>;
};

template<typename List, template<typename T>class MetaFun>
using Transform = typename TransformT<List, MetaFun>::Type;

//transform Test.
template<typename T>
struct AddConstT {
	using Type = const T;
};