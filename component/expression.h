#pragma once
#include <memory>
#include <vector>
#include <time.h>

//例子：针对数组之间的运算，比如a*3 + a*b其结果是一个result数组
//比如：MyArray<int> x(1000), y(1000); 计算x = 1.5*x + x*y;

//数组可以使用std::vector或者自定义数组

template<typename T>
class MyArray
{
private:
	T *_arr;
	size_t _size;
public:
	MyArray(size_t size) :_arr(new T[size]), _size(size)
	{
		memset(_arr, 0, size * sizeof(T));
	}

	MyArray(const std::initializer_list<T> &args) : _arr(new T[args.size()]), _size(args.size())
	{
		int idx = 0;
		for (auto ptr = args.begin(); ptr != args.end(); ++ptr)
		{
			_arr[idx++] = *ptr;
		}
	}

	MyArray(MyArray &&other)
	{
		_arr = other._arr;
		_size = other._size;
		other._arr = nullptr;
	}

	T operator[](size_t idx)const
	{
		return _arr[idx];
	}

	size_t size() const{ return _size; }
	~MyArray()
	{
		delete _arr;
	}
};

//数组traits
template<typename T>
class OpTratis
{
public:
	typedef T value_type;
	typedef const T& const_ref;
};

template<typename T>
class Scalar;

//为标量局部化
template<typename T>
class OpTratis<Scalar<T> >
{
public:
	typedef Scalar<T> const_ref;
};

//定义表达式的二元
template<typename T, typename OP1, typename OP2>
struct DoubleUnit
{
	using value_type_1 = typename OpTratis<OP1>::const_ref;
	using value_type_2 = typename OpTratis<OP2>::const_ref;
	value_type_1 _op1;
	value_type_2 _op2;
	size_t _count;
	DoubleUnit(const OP1 &op1, const OP2 &op2, size_t size) :_op1(op1), _op2(op2), _count(size) {}
};


//加法表达式类模板
template<typename T, typename OP1, typename OP2>
class AddExpr : public DoubleUnit<T, OP1, OP2>
{
public:
	AddExpr(const OP1 &op1, const OP2 & op2) : DoubleUnit<T, OP1, OP2>(op1, op2, op1.size() ? op1.size() : op2.size()) {}

	T operator[](size_t idx) const
	{
		return _op1[idx] + _op2[idx];
	}

	size_t size()const
	{
		return _op1.size();
	}
};

//乘法表达式模板
template<typename T, typename OP1, typename OP2>
class MultExpr : public DoubleUnit<T, OP1, OP2>
{
public:
	MultExpr(const OP1 &op1, const OP2 & op2) : DoubleUnit<T, OP1, OP2>(op1, op2) {}

	T operator[](size_t idx) const
	{
		return _op1[idx] * _op2[idx];
	}

	size_t size()const
	{
		return _op1.size();
	}
};

//为内置类型定义外覆器，以作用于MultExpr
template<typename T>
class Scalar
{
private:
	const T &_val;

public:
	Scalar(const T &val) :_val(val) {}
	T operator[](size_t idx) const
	{
		return _val;
	}

	size_t size()const
	{
		return 0;
	}
};

template<typename T,typename Expr = MyArray<T> >
class BaseArray
{
private:
	const Expr _expr;

public:
	BaseArray(const Expr &expr) :_expr(expr) 
	{
		std::cout << "expr: " << typeid(Expr).name() << std::endl;
	}

	BaseArray(Expr &&expr) :_expr(std::move(expr))
	{
		std::cout << "expr: " << typeid(Expr).name() << std::endl;
	}

	T operator[](size_t idx)const
	{
		return _expr[idx];
	}

	BaseArray &operator=(const BaseArray &other)
	{

	}

	template<typename U,  typename expr2>
	BaseArray &operator=(const BaseArray<U, expr2> &other)
	{
		if (&other != this)
		{
			for (int idx = 0; idx < _expr.size(); ++idx)
			{
				_expr[idx] = other[idx];
			}
		}
		return *this;
	}

	const Expr &rep()const { return _expr; }
};


//加法运算符
template<typename T, typename OP1,typename OP2>
BaseArray<T, AddExpr<T, OP1, OP2> > operator+(const BaseArray<T,OP1> &op1, const BaseArray<T, OP2> & op2)
{
	return BaseArray<T, AddExpr<T, OP1, OP2> >(AddExpr<T, OP1, OP2>(op1.rep(), op2.rep()));
}

//乘法运算符
template<typename T, typename OP1, typename OP2>
BaseArray<T, MultExpr<T, OP1, OP2> > operator*(const BaseArray<T, OP1> &op1, const BaseArray<T, OP2> & op2)
{
	return BaseArray<T, MultExpr<T, OP1, OP2> >(MultExpr<T, OP1, OP2>(op1, op2));
}


//main.cpp
int main()
{
	BaseArray<int> vec1(MyArray<int>{ 1, 2, 3, 4, 5 });
	BaseArray<int> vec2(MyArray<int>{ 1, 2, 3, 4, 5 });

	BaseArray<int> vec3 = (vec1 + vec2);
	
	getchar();
	return 1;
}


