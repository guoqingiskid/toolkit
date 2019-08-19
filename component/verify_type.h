#pragma once
#include <memory>
#include <vector>
#include <time.h>

//注意两点：第一个只有模板函数才有SFINAE原则，因此这里test必须是两个模板函数；第二点：该类写成模板是为了外部调用方便，模板参数T仅仅将类型传递给内部test的模板参数上。
template<typename T>
class VerifyClass
{
public:
	typedef char One;
	typedef struct { char arr[2]; } Two;

	template<typename C>
	static One test(int C::*);
	
	template<typename C>
	static Two test(...);

	enum { Yes = sizeof(VerifyClass<T>::test<T>(0)) == 1 };
	enum { No = !Yes };
};

//另外方式辨别基本类型
template<typename T>
class IsfundamentalType
{
public:
	enum { yes = 0, no = 1 };
};

#define MK_FUNDAMENTAL_TYPE(T)				\
template<> class IsfundamentalType<T> {		\
public:										\
	enum { yes = 1, no = 0 };				\
};										

MK_FUNDAMENTAL_TYPE(void)
MK_FUNDAMENTAL_TYPE(int)
MK_FUNDAMENTAL_TYPE(char)
MK_FUNDAMENTAL_TYPE(bool)
MK_FUNDAMENTAL_TYPE(unsigned int)
MK_FUNDAMENTAL_TYPE(float)
MK_FUNDAMENTAL_TYPE(double)
MK_FUNDAMENTAL_TYPE(short)
MK_FUNDAMENTAL_TYPE(unsigned short)
MK_FUNDAMENTAL_TYPE(long)
MK_FUNDAMENTAL_TYPE(long long)


//鉴别组合类型，这里的组合类型指比如int* int& int(MyClass::*)()成员函数等等。
template<typename T>
class CombinationTypeTraits
{
public:
	enum { isPtr = 0, isRef = 0, isArray = 0, isFunc = 0, isPtrMem = 0 };
	typedef T BaseType;
	typedef T BottomType;
	typedef CombinationTypeTraits<void> ClassType;
};

template<typename T>
class CombinationTypeTraits<T*>
{
public:
	enum { isPtr = 1, isRef = 0, isArray = 0, isFunc = 0, isPtrMem = 0 };
	typedef T BaseType;
	typedef typename CombinationTypeTraits<T>::BottomType BottomType;
	typedef CombinationTypeTraits<void> ClassType;
};

template<typename T>
class CombinationTypeTraits<T&>
{
public:
	enum { isPtr = 0, isRef = 1, isArray = 0, isFunc = 0, isPtrMem = 0 };
	typedef T BaseType;
	typedef typename CombinationTypeTraits<T>::BottomType BottomType;
	typedef CombinationTypeTraits<void> ClassType;
};

template<typename T, size_t N>
class CombinationTypeTraits<T[N]>
{
public:
	enum { isPtr = 0, isRef = 0, isArray = 1, isFunc = 0, isPtrMem = 0 };
	typedef T BaseType;
	typedef typename CombinationTypeTraits<T>::BottomType BottomType;
	typedef CombinationTypeTraits<void> ClassType;
};

template<typename T>
class CombinationTypeTraits<T[]>
{
public:
	enum { isPtr = 0, isRef = 0, isArray = 1, isFunc = 0, isPtrMem = 0 };
	typedef T BaseType;
	typedef typename CombinationTypeTraits<T>::BottomType BottomType;
	typedef CombinationTypeTraits<void> ClassType;
};

template<typename T, typename C>
class CombinationTypeTraits<T C::*>     //注意成员函数指针的定义
{
public:
	enum { isPtr = 0, isRef = 0, isArray = 0, isFunc = 0, isPtrMem = 1 };
	typedef T BaseType;
	typedef typename CombinationTypeTraits<T>::BottomType BottomType;
	typedef C ClassType;
};
