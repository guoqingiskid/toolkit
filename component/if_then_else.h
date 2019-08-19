#pragma once
template<bool BT_,typename T1,typename T2>
class IfThenElese;

template<typename T1,typename T2>
class IfThenElese<true, T1, T2>
{
public:
	typedef T1 type;
};

template<typename T1,typename T2>
class IfThenElese<false, T1, T2>
{
public:
	typedef T2 type;
};