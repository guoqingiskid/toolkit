#pragma once
#include <string>
#include <memory>
#include <iostream>


template<typename T>
class SinglelinkedList
{
private:
	typedef struct _Unit
	{
		T _val;
		std::shared_ptr<_Unit> _next;
		_Unit() = default;
		_Unit(const T &val, const std::shared_ptr<_Unit> &ptr) :_val(val), _next(ptr) {}
	}Unit;

public:
	SinglelinkedList()
	{
		_top.reset(new Unit);
		_top->_val = 0xffffffff;
	}

	SinglelinkedList(const SinglelinkedList &other) : SinglelinkedList()
	{
		clone(other.top()->_next);
	}

	SinglelinkedList &operator=(const SinglelinkedList &other) 
	{
		if (&other == this)
			return *this;

		_top->_next.reset();
		clone(other.top()->_next);
		return *this;
	}

	SinglelinkedList(const std::initializer_list<T> &args) : SinglelinkedList()
	{
		if (!args.size())
			return;

		for (auto ptr = args.begin(); ptr != args.end(); ++ptr)
		{
			insertUnitBeg(*ptr);
		}
	}

	std::shared_ptr<Unit> findUnit(const T &val)
	{
		auto ptr = _top;
		while (ptr != nullptr && ptr->_val != val)
			ptr = ptr->_next;

		return ptr;
	}

	std::shared_ptr<Unit> insert(const std::shared_ptr<Unit> &p, const T &val)
	{
		if (!p)
			return insertUnitEnd(val);

		std::shared_ptr<Unit> pEle = std::make_shared<Unit>(val, p->_next);
		p->_next = pEle;
		return pEle;
	}

	std::shared_ptr<Unit> top() const{ return _top; }

	std::shared_ptr<Unit> insertAtHead(const T &val)
	{
		return insertUnitBeg(val);
	}

	std::shared_ptr<Unit> insertAtEnd(const T &val)
	{
		return insertUnitEnd(val);
	}

	std::shared_ptr<Unit> deleteVal(const T &val)
	{
		//不能直接调findUnit，我们需要找单链表要删元素的前一个元素
		auto ptr = _top;
		while (ptr->_next != nullptr && ptr->_next->_val != val)
			ptr = ptr->_next;
		
		if (!ptr->_next)
			return ptr;
		ptr->_next = ptr->_next->_next;
		return deleteVal(val);
	}

	std::shared_ptr<Unit> deleteAfterUnit(std::shared_ptr<Unit> ptr)
	{
		if(ptr && ptr->_next)
			ptr->_next = ptr->_next->_next;
		return ptr;
	}

	template<typename CB>
	void foreachConst(CB &&cb)
	{
		std::shared_ptr<Unit> ptr = _top->_next;
		while (ptr.get() != nullptr)
		{
			T val = ptr->_val;
			cb(val);
			ptr = ptr->_next;
		}
	}

	template<typename CB>
	void foreach(CB &&cb)
	{
		std::shared_ptr<Unit> ptr = _top->_next;
		while (ptr.get() != nullptr)
		{
			cb(ptr->_val);
			ptr = ptr->_next;
		}
	}

	void clear()
	{
		_top->_next.reset();
	}

	void insertionSort()
	{
		if(_top->_next)
			insertionSort(_top->_next);
	}

	void insertionSort(std::shared_ptr<Unit> pUnit)
	{
		if (!pUnit)
			return;
 		auto prev = pUnit;
		auto next = pUnit->_next;
		while (next)  //N-1趟
		{
			while (prev != next)  //每趟保证是有序的
			{
				if (prev->_val > next->_val)
				{
					T val = prev->_val;
					prev->_val = next->_val;
					next->_val = val;
				}
				prev = prev->_next;
			}
			next = next->_next;
			prev = pUnit;
		}
	}

	template<typename CB>
	void selectionSort(CB &&func)
	{
		selectionSort(_top->_next, std::forward<CB&&>(func));
		std::cout << typeid(func).name();
	}

	void selectionSort()
	{
		selectionSort(_top->_next, std::less<T>());
	}

	template<typename CB>
	void selectionSort(std::shared_ptr<Unit> pUnit, CB &&func)
	{
		if (!pUnit)
			return;
		auto prev = pUnit;
		auto next = pUnit;
		while (prev->_next) // N-1趟
		{
			T minVal = next->_val;
			while (next)
			{
				if (func(next->_val, minVal))
				{
					T val = next->_val;
					next->_val = minVal;
					minVal = val;
				}
				next = next->_next;
			}

			//每一趟都找到一个最值
			T tmpVal = minVal;
			minVal = prev->_val;
			prev->_val = tmpVal;

			prev = prev->_next;
			next = prev;
		}
	}


private:
	std::shared_ptr<Unit> insertUnitBeg(const T &val)
	{
		std::shared_ptr<Unit> ptr(new Unit);
		ptr->_val = val;
		ptr->_next = _top->_next;
		_top->_next = ptr;
		return ptr;
	}

	std::shared_ptr<Unit> insertUnitEnd(const T &val)
	{
		auto ptr = _top;
		while (ptr->_next != nullptr)
			ptr = ptr->_next;

		std::shared_ptr<Unit> pEle = std::make_shared<Unit>(val,nullptr);
		ptr->_next = pEle;
		return pEle;
	}

	void clone(std::shared_ptr<Unit> ptr)
	{
		//这里不能直接调insertUnitEnd，因为这会造成时间复杂度为O(N2次方)
		std::shared_ptr<Unit> p = _top;
		while (ptr != nullptr)
		{
			p->_next = std::make_shared<Unit>(ptr->_val, nullptr);
			p = p->_next;
			ptr = ptr->_next;
		}
	}

private:
	std::shared_ptr<Unit> _top;
};


//main.cpp
#include <iostream>
#include<fstream>
#include <bitset>
#include "myClass.h"
#include <iterator>
#include <vector>
#include <functional>
#include "linked_list.h"

static void print(const int &ele)
{
	std::cout << ele << " ";
}
int main()
{
	SinglelinkedList<int> single_list{1,2,3,4,5,6};
	single_list.insertAtHead(20);
	single_list.insertAtEnd(100);
	single_list.insert(single_list.findUnit(1), 200);
	single_list.insert(single_list.findUnit(0), 300);
	single_list.deleteVal(3);
	single_list.foreach(print);
	auto p = single_list.findUnit(0);
	if(p)
		std::cout << std::endl << p->_val << std::endl;
	
	SinglelinkedList<int> single_list_2 = single_list;
	SinglelinkedList<int> single_list_3{ 10,9,8,7 };

	std::cout << std::endl;
	single_list_3.foreachConst(print);

	std::cout << std::endl;
	single_list_2.foreachConst(print);
	
	single_list_3 = single_list_2;

	std::cout << std::endl;
	single_list_3.foreachConst(print);
	
	//排序测试
	SinglelinkedList<int> singList{ 1,14,36,13,8,7,6,87,13 };
	singList.selectionSort(std::greater<int>());
	singList.foreach(print);

	getchar();
	return 1;
}

