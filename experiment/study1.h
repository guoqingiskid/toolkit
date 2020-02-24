#ifndef STUDY1_H
#define STUDY1_H
#include <vector>
#include <deque>
#include <iostream>

//Test1: Policy的运用与意义
template<typename T, typename Container>
class MyContainer
{
public:
	MyContainer(){}
};

//模板的模板参数
template<typename T,
	template<typename, typename = std::allocator<T> >
		class Container = std::vector>
class MyContainer1
{
private:
	Container<T> _vec;
};

template<typename T>
struct FstPolicy
{};

template<typename T>
struct SedPolicy
{};

//Policy如何定义与使用
template<typename T, template<typename>class Policy>
class MyPolicyHandler : Policy<T>
{
public:
	int get()
	{
		T t;
		return t.size();
	}
};


//Test2：常整数映射型别
template<int N>
struct Int2Type
{
	enum{value = N};
};

template<typename T, bool isType>
class MyTest
{
private:
	void do_some_thing(T *pObj, Int2Type<true>)	 //如果该函数从来没被调用过，那么就不会被编译
	{
		pObj->show();
	}

	void do_some_thing(T *pObj, Int2Type<false>)
	{
		std::cout << *pObj << std::endl;
	}

public:
	void do_some_thing(T *pObj)
	{
		do_some_thing(pObj, Int2Type<isType>());
	}
};


//Test3：类的模板成员函数必须写在类外部才是符号标准的，函数的重载不受影响
class MyClass1
{
public:
	template<typename T>
	void show(T &t)
	{
		std::cout << "show, type: " << sizeof(t) << std::endl;
	}

	void show()
	{
		std::cout << "xixi" << std::endl;
	}
};

template<>
void MyClass1::show<int>(int &data)
{
	std::cout << "show int: " << sizeof(data) << std::endl;
}


//Test4：类型选择，另外的可能实现是采用type traits
template<bool isType, typename T, typename U>
struct select_type
{
	typedef T type;
};

template<typename T, typename U>
struct select_type<false, T, U>
{
	typedef U type;
};

#endif