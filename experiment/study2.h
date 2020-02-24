#ifndef STUDY2_H
#define STUDY2_H
#include <stdint.h>

//TypeLists
template<typename T1, typename T2>
struct TypeList
{
	typedef T1 First;
	typedef T2 Second;
};

struct NullType
{};

#define TYPELIST_0() NullType;
#define TYPELIST_1(type1) TypeList<type1, NullType>
#define TYPELIST_2(type1, type2) TypeList<type1, TYPELIST_1(type2)>
#define TYPELIST_3(type1, type2, type3) TypeList<type1, TYPELIST_2(type2, type3)>
#define TYPELIST_4(type1, type2, type3, type4) TypeList<type1, TYPELIST_3(type2, type3, type4)>
#define TYPELIST_5(type1, type2, type3, type4, type5) TypeList<type1, TYPELIST_4(type2, type3, type4, type5)>
#define TYPELIST_6(type1, type2, type3, type4, type5, type6) TypeList<type1, TYPELIST_5(type2, type3, type4, type5, type6)>
#define TYPELIST_7(type1, type2, type3, type4, type5, type6, type7) TypeList<type1, TYPELIST_6(type2, type3, type4, type5, type6, type7)>
#define TYPELIST_8(type1, type2, type3, type4, type5, type6, type7, type8) TypeList<type1, TYPELIST_7(type2, type3, type4, type5, type6, type7, type8)>
#define TYPELIST_9(type1, type2, type3, type4, type5, type6, type7, type8, type9) TypeList<type1, TYPELIST_8(type2, type3, type4, type5, type6, type7, type8, type9)>
#define TYPELIST_10(type1, type2, type3, type4, type5, type6, type7, type8, type9, type10) TypeList<type1, TYPELIST_9(type2, type3, type4, type5, type6, type7, type8, type9, type10)>

//TypeList Length
template<typename TL>
struct Length{};

template<>
struct Length<NullType>
{
	enum{Value = 0};
};

template<typename T1, typename T2>
struct Length<TypeList<T1, T2> >
{
	enum{Value = 1 + Length<T2>::Value};
};

//TypeList Index
template<typename TL, uint32_t Index>
struct TypeListAt{};

template<typename T1, typename T2>
struct TypeListAt<TypeList<T1, T2>, 0>
{
	typedef T1 Result;
};

template<typename T1, typename T2, uint32_t Index>
struct TypeListAt<TypeList<T1, T2>, Index>
{
	typedef typename TypeListAt<T2, Index - 1>::Result Result;
};

template<uint32_t Index>
struct TypeListAt<NullType, Index>
{
	typedef NullType Result;
};

//TypeList IndexOf
template<typename TL, typename T>
struct IndexOf
{};

template<typename T>
struct IndexOf<NullType, T>
{
	enum{Index = -1};
};

template<typename T, typename Lst>
struct IndexOf<TypeList<T, Lst>, T>
{
	enum{Index = 0};
};

template<typename Fst, typename Lst, typename T>
struct IndexOf<TypeList<Fst, Lst>, T>
{
private:
	enum{Tmp = IndexOf<Lst, T>::Index == -1 ? -1 : IndexOf<Lst, T>::Index + 1};
public:
	enum{Index = Tmp};
};

//TypeList Append
template<typename TL, typename T>
struct Append{};

template<>
struct Append<NullType, NullType>
{
	typedef NullType Result;
};

template<typename T>
struct Append<NullType, T>
{
	typedef TYPELIST_1(T) Result;
};

template<typename Fst, typename Lst, typename T>
struct Append<TypeList<Fst, Lst>, T>
{
	typedef TypeList<Fst, typename Append<Lst, T>::Result> Result;
};

//TypeList Remove
template<typename TL, typename T>
struct Remove{};

template<typename T>
struct Remove<NullType, T>
{
	typedef NullType Result;
};

template<typename T, typename Lst>
struct Remove<TypeList<T, Lst>, T>
{
	typedef Lst Result;
};

template<typename Fst, typename Lst, typename T>
struct Remove<TypeList<Fst, Lst>, T>
{
	typedef TypeList<Fst, typename Remove<Lst, T>::Result> Result;
};

//TypeList RemoveAll
template<typename TL, typename T>
struct RemoveAll{};

template<typename T>
struct RemoveAll<NullType, T>
{
	typedef NullType Result;
};
template<typename T, typename Lst>
struct RemoveAll<TypeList<T, Lst>, T>
{
	typedef typename RemoveAll<Lst, T>::Result Result;
};

template<typename Fst, typename Lst, typename T>
struct RemoveAll<TypeList<Fst, Lst>, T>
{
	typedef TypeList<Fst, typename RemoveAll<Lst, T>::Result> Result;
};

//TypeList RemoveDuplicate
template<typename TL>
struct RemoveDuplicate{};

template<>
struct RemoveDuplicate<NullType>
{
	typedef NullType Result;
};

template<typename Fst, typename Lst>
struct RemoveDuplicate<TypeList<Fst, Lst> >
{
private:
	typedef typename RemoveDuplicate<Lst>::Result RDR;
	typedef typename Remove<RDR, Fst>::Result RR;

public:
	typedef TypeList<Fst, RR> Result;
};

//TypeList Replace
template<typename TL, typename T, typename U>
struct Replace{};

template<typename T, typename U>
struct Replace<NullType, T, U>
{
	typedef NullType Result;
};

template<typename T, typename Lst, typename U>
struct Replace<TypeList<T, Lst>, T, U>
{
	typedef TypeList<U, Lst> Result;
};

template<typename Fst, typename Lst, typename T, typename U>
struct Replace<TypeList<Fst, Lst>, T, U>
{
	typedef TypeList<Fst, typename Replace<Lst, T, U>::Result> Result;
};

//TypeList Inheritance
template<typename T, template<typename>class Base>
class GenClass;

template<typename BuiltIn, template<typename>class Base>
class GenClass : public Base<BuiltIn>
{};

template<template<typename>class Base>
class GenClass<NullType, Base>{};

template<typename Fst, typename Lst, template<typename>class Base>
class GenClass<TypeList<Fst, Lst>, Base> :
	public GenClass<Fst, Base>,
	public GenClass<Lst, Base>
{};

template<uint32_t Index, typename TL, template<typename>class Base>
Base<typename TypeListAt<TL, Index>::Result>&
	get(GenClass<TL, Base> &gc)
{
	return gc;
}


#endif
