#pragma once
#include <iostream>
class Default1
{

};

class Default2
{

};

class Default3
{

};

class Default4
{

};

//把缺省值都提及到基类，当我们给了一个实参，就将其缺省参数换掉。
struct DefaultPolicies
{
	typedef Default1 P1;
	typedef Default2 P2;
	typedef Default3 P3;
	typedef Default4 P4;
};

class DefualtPolicy :virtual public DefaultPolicies    //DefualtPolicy作为MyClass缺省模板参数，虚继承，保证后面派生类多继承上仅会有4个缺省值的定义P1 P2 P3 P4
{

};

template<typename T,int NUM>
class Selector :virtual public T {};    //定义它是为了下面的类在多继承上不会出错，比如当Arg1和Arg2相同时，如果不使用额外的辅助类就会出错，继承相同的类型。

template<typename Arg1,
		 typename Arg2,
		 typename Arg3,
		 typename Arg4>
class PolicySelector : public Selector<Arg1, 1>,
					   public Selector<Arg2, 2>,
					   public Selector<Arg3, 3>,
					   public Selector<Arg4, 4>
{
	//虚继承保证了该类里面仅有4个类型P1 P2 P3 P4，传入的模板参数不同，我们将会得到不同P值。
};

template<typename T>
class Policy_1 : virtual public DefualtPolicy
{
public:
	typedef T P1;
};

template<typename T>
class Policy_2 : virtual public DefualtPolicy
{
public:
	typedef T P2;
};


template<typename T>
class Policy_3 : virtual public DefualtPolicy
{
public:
	typedef T P3;
};

template<typename T>
class Policy_4 : virtual public DefualtPolicy
{
public:
	typedef T P4;
};



template<typename Arg1 = DefualtPolicy,
		 typename Arg2 = DefualtPolicy,
		 typename Arg3 = DefualtPolicy,
		 typename Arg4 = DefualtPolicy>
class MyClass
{
public:

	//通过一个辅助模板类获取到所有的Policy包括缺省的。
	typedef PolicySelector<Arg1, Arg2, Arg3, Arg4> policies;

	static void show()
	{
		std::cout << typeid(policies::P1).name() << "\t"
			<< typeid(policies::P2).name() << "\t"
			<< typeid(policies::P3).name() << "\t"
			<< typeid(policies::P4).name() << "\t";
	}
};


int main()
{
	MyClass<Policy_3<double> >::show();
	getchar();
	return 1;
}