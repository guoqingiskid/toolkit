#ifndef STUDY3_H
#define STUDY3_H
#include "study2.h"
#include <memory>

//泛化Functor

//通常我们写的非泛化的Functor
class NonGenericFunctor
{
public:
	NonGenericFunctor(){}
	~NonGenericFunctor(){}
	NonGenericFunctor(const NonGenericFunctor &other){}

	void operator()()
	{
		//do nothing
	}

	void operator()(int arg1)
	{
		//do nothing
	}

	void operator()(int arg1, int arg2)
	{
		//do nothing
	}
};

//针对返回值的泛化
template<typename R>
class RFunctor
{
public:
	R operator()()
	{
		//do nothing
		return R();
	}
};

//但是如何针对多个回调参数个数进行泛化呢？答案是使用typelist
//回顾Typelist，是一个携带类型的个数和型别的好玩意儿
template<typename R, typename TList>
class GenericFunctorImpl;

//无参数情况
template<typename R>
class GenericFunctorImpl<R, NullType>
{
public:
	virtual R operator()() = 0;
	virtual ~GenericFunctorImpl(){}
};

//一个参数情况
template<typename R, typename Arg1>
class GenericFunctorImpl<R, TYPELIST_1(Arg1)>
{
public:
	virtual R operator()(const Arg1 &arg1) = 0;
	virtual ~GenericFunctorImpl(){}
};

//两个参数情况
template<typename R, typename Arg1, typename Arg2>
class GenericFunctorImpl<R, TYPELIST_2(Arg1, Arg2)>
{
public:
	virtual R operator()(const Arg1 &arg1, const Arg2 &arg2) = 0;
	virtual ~GenericFunctorImpl(){}
};

//具体实作物
template<class Functor, class Fun>
class FunctorHandler : public GenericFunctorImpl<typename Functor::Result, typename Functor::Args>
{
public:
	typedef typename Functor::Result R;
	typedef typename Functor::Args Args;
	FunctorHandler(const Fun &fun) :
		_fun(fun)
	{

	}

	R operator()()
	{
		return _fun();
	}

	R operator()(const typename Functor::Arg1 &arg1)
	{
		return _fun(arg1);
	}

	R operator()(const typename Functor::Arg1 &arg1, const typename Functor::Arg2 &arg2)
	{
		return _fun(arg1, arg2);
	}

private:
	Fun _fun;
};


//具体实作物
template<class Functor, typename PObj, typename PMemFun>
class MemFunctorHandler : public GenericFunctorImpl<typename Functor::Result, typename Functor::Args>
{
public:
	typedef typename Functor::Result R;
	typedef typename Functor::Args Args;

	MemFunctorHandler(const PObj &obj, const PMemFun &pMemFn) :
		_obj(obj), _pMemFn(pMemFn)
	{

	}

	R operator()()
	{
		return ((*_obj).*_pMemFn)();
	}

	R operator()(const typename Functor::Arg1 &arg1)
	{
		return ((*_obj).*_pMemFn)(arg1);
	}

	R operator()(const typename Functor::Arg1 &arg1, const typename Functor::Arg2 &arg2)
	{
		return ((*_obj).*_pMemFn)(arg1, arg2);
	}

private:
	PObj _obj;
	PMemFun _pMemFn;
};

template<typename R, typename TList>
class GenericFunctor
{
public:
	typedef TList Args;
	typedef R Result;
	typedef typename TypeListAt<TList, 0>::Result Arg1;
	typedef typename TypeListAt<TList, 1>::Result Arg2;
	R operator()()
	{
		return (*_impl)();
	}

	R operator()(const Arg1 &arg1)
	{
		return (*_impl)(arg1);
	}

	R operator()(const Arg1 &arg1, const Arg2 &arg2)
	{
		return (*_impl)(arg1, arg2);
	}

	template<class Fun>
	GenericFunctor(Fun &&fun):
		_impl(new FunctorHandler<GenericFunctor<R, TList>, Fun>(std::forward<Fun>(fun)))
	{

	}

	template<typename PObj, typename PMemFn>
	GenericFunctor(PObj && pobj, PMemFn &&pMemFn) :
		_impl(new MemFunctorHandler<GenericFunctor<R, TList>, PObj, PMemFn>(pobj, pMemFn))
	{

	}

private:
	typedef GenericFunctorImpl<R, TList> Impl;
	std::unique_ptr<Impl> _impl;
};
#endif

