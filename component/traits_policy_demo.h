#pragma once
#include <memory>
#include <vector>
#include <time.h>

//trait类
template<typename T>
class AccumulateTrait
{
public:
	typedef T accu_type;
	static const accu_type value;
};

template<>
class AccumulateTrait<char>
{
public:
	typedef int accu_type;
	static const accu_type value;
};

template<>
class AccumulateTrait<double>
{
public:
	typedef double accu_type;
	static const accu_type value;
};

const AccumulateTrait<double>::accu_type AccumulateTrait<double>::value = 1.0;
const AccumulateTrait<char>::accu_type AccumulateTrait<char>::value = 0;
template<typename T>
const typename AccumulateTrait<T>::accu_type AccumulateTrait<T>::value = T();


//policy类
template<typename T, template<typename> class AT = AccumulateTrait >
class SumPolicy
{
public:
	
	static typename AT<T>::accu_type accumulate(const T *beg, const T *end,const T val = T())
	{
		typename AT<T>::accu_type res = val;
		while (beg != end)
		{
			res += *beg;
			++beg;
		}

		return res;
	}

	typedef T vaue_type;
};

template<typename T, template<typename> class AT = AccumulateTrait >
class MultPolicy
{
public:

	static typename AT<T>::accu_type accumulate(const T *beg, const T *end, const T val = T())
	{
		typename AT<T>::accu_type res = val;
		while (beg != end)
		{
			res *= *beg;
			++beg;
		}
		return res;
	}

	typedef T vaue_type;
};

template<typename T, template<typename, template<typename> class AT > class Policy >
class ValueTrait;

template<typename T, template<typename, template<typename> class AT > class Policy = SumPolicy >
class ValueTrait
{
public:
	static const typename SumPolicy<T>::value_type value;
};

template<typename T, MultPolicy<T> >
class ValueTrait
{
public:
	static const typename MultPolicy<T>::value_type value;
};

template<typename T>
const typename SumPolicy<T>::value_type ValueTrait<T, SumPolicy >::value = 0;

template<typename T>
const typename MultPolicy<T>::value_type ValueTrait<T, MultPolicy >::value = 1;

template<typename T, typename Policy, typename AT = AccumulateTrait<T> >
typename AccumulateTrait<T>::accu_type accumulate(const T *beg, const T *end)
{
	typedef typename AT::accu_type acuu_type;
	
	return Policy::accumulate<acuu_type>(beg, end, ValueTrait<T, Policy>::value);
}
