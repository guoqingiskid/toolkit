#pragma once

//求N的M次幂，计算发生在编译期
template<int N,int M>
class Power
{
public:
	enum { result = N * Power<N,M - 1>::result };
};

template<int N>
class Power<N,0>
{
public:
	enum { result = 1 };
};


//求平方根 比如Sqrt<16,1,16>::result = 4
template<int N, int L0 = 0, int HI = N>
class Sqrt
{
public:
	enum { mid = (L0 + HI + 1) / 2 };
	typedef typename IfThenElese<(N < mid*mid),
		Sqrt<N, L0, mid - 1>,
		Sqrt<N, mid, HI> >::type SubT;
	enum { result = SubT::result };
};

template<int N, int S>
class Sqrt<N, S, S>
{
public:
	enum { result = S };
};