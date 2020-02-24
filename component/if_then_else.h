#pragma once
template<bool BT_,typename T1,typename T2>
class IfThenElse;

template<typename T1,typename T2>
class IfThenElse<true, T1, T2>
{
public:
	typedef T1 type;
};

template<typename T1,typename T2>
class IfThenElse<false, T1, T2>
{
public:
	typedef T2 type;
};