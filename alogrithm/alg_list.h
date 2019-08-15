#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <random>
#include <thread>
#include <unordered_set>

template<typename T>
class SinglelinkedList
{
protected:
	typedef struct _Unit
	{
		T _val;
		bool _tag;
		std::shared_ptr<_Unit> _next;
		_Unit() = default;
		_Unit(const T &val, const std::shared_ptr<_Unit> &ptr) :_val(val), _next(ptr),_tag(false) {}
	}Unit;

public:
	using pUnit = std::shared_ptr<Unit>;

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

	std::shared_ptr<Unit> deleteUnit(const T &val)
	{
		//不能直接调findUnit，我们需要找单链表要删元素的前一个元素
		auto ptr = _top;
		while (ptr->_next != nullptr && ptr->_next->_val != val)
			ptr = ptr->_next;
		
		if (!ptr->_next)
			return ptr;
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

protected:
	std::shared_ptr<Unit> _top;
};


//一些有用的traits
template<typename T, template<typename> class LinkedLst>
class CycleLinkedListTraits
{
public:
	typedef typename LinkedLst<T>::Unit  value_type;
	typedef typename LinkedLst<T>::pUnit value_pointer;
};

//打破链表循环的policy
template<typename T, template<typename> class LinkedLst>
class TagPolicy
{
public:
	static void breakCycle(typename CycleLinkedListTraits<T,LinkedLst>::value_pointer pStartUnit)
	{
		using value_type = typename CycleLinkedListTraits<T, LinkedLst>::value_type;
		auto ptr = pStartUnit;
		while (ptr->_next)
		{
			if (ptr->_next->_tag)
			{
				//走到了被标记的地方，该单元格为循环头
				ptr->_next.reset();
				break;
			}

			ptr->_tag = true;
			ptr = ptr->_next;
		}
	}
};

template<typename T, template<typename> class LinkedLst>
class HashTablPolicy
{
public:
	using value_pointer = typename CycleLinkedListTraits<T, LinkedLst>::value_pointer;
	static void breakCycle(value_pointer pStartUnit)
	{
		std::unordered_set<value_pointer> hashTable;
		auto ptr = pStartUnit;
		while (ptr->_next)
		{
			if (hashTable.count(ptr->_next))
			{
				ptr->_next.reset();
				break;
			}

			hashTable.insert(ptr);
			ptr = ptr->_next;
		}
	}
};

template<typename T, template<typename> class LinkedLst>
class LookBackPolicy
{
public:
	using value_pointer = typename CycleLinkedListTraits<T, LinkedLst>::value_pointer;
	static void breakCycle(value_pointer pStartUnit)
	{
		auto fst = pStartUnit;
		value_pointer sed ;
		while (fst->_next)
		{
			sed = pStartUnit;
			while (sed != fst)
			{
				if (sed->_next == fst->_next)
				{
					//找到循环头
					fst->_next = nullptr;
					return;
				}
				sed = sed->_next;
			}
			fst = fst->_next;
		}
	}
};

template<typename T, template<typename> class LinkedLst>
class CheckCyclePolicy
{
public:
	using value_pointer = typename CycleLinkedListTraits<T, LinkedLst>::value_pointer;
	static bool checkCycle(value_pointer pStartUnit)
	{
		//第一次反转，找到是否有循环
		value_pointer pEle = reverseList(pStartUnit);

		//第二次反转，恢复链表，无论是否有环，再次反转一定能恢复。
		reverseList(pEle);
		return pEle == pStartUnit ? true : false;
	}

private:
	static value_pointer reverseList(value_pointer pStartUnit)
	{
		//反转需要至少三个指针
		value_pointer prev = nullptr;
		value_pointer current = pStartUnit;
		while (current)
		{
			value_pointer next = current->_next;
			current->_next = prev;
			prev = current;
			current = next;
		}
		return prev;
	}
};

template<typename T, template<typename> class LinkedLst>
class TortoiseAndRabbitPolicy
{
public:
	using value_pointer = typename CycleLinkedListTraits<T, LinkedLst>::value_pointer;
	static void breakCycle(value_pointer pStartUnit)
	{
		value_pointer RabbitPtr = pStartUnit;
		value_pointer TortoisePtr = pStartUnit;
		while (RabbitPtr->_next && RabbitPtr->_next->_next)
		{
			TortoisePtr = TortoisePtr->_next;			//乌龟速度为1
			RabbitPtr = RabbitPtr->_next->_next;		//兔子速度为2

			if (RabbitPtr == TortoisePtr)
				break;
		}

		if (RabbitPtr == TortoisePtr)    //存在循环且此时兔子已经与乌龟相遇
		{
			RabbitPtr = pStartUnit;      //相遇后将兔子重头开始走，每一步走一格
			while (RabbitPtr != TortoisePtr)
			{
				RabbitPtr = RabbitPtr->_next;
				TortoisePtr = TortoisePtr->_next;
			}

			//退出循环，表明此时兔子和乌龟已经在循环头位置再次相遇
			while (RabbitPtr->_next != TortoisePtr)
			{
				//让兔子走，乌龟不动，由此打破循环
				RabbitPtr = RabbitPtr->_next;
			}

			RabbitPtr->_next = nullptr;
		}
		else   //不相等，则表明不存在循环
		{
			
		}
	}
};

template<typename T>
class CycleSinglelinkedList : public SinglelinkedList<T>
{
private:
	using parent_type = SinglelinkedList<T>;
	using _Unit = typename SinglelinkedList<T>::Unit;
public:
	using Unit  = _Unit;
	using pUnit = std::shared_ptr<_Unit>;
	CycleSinglelinkedList(const std::initializer_list<T> &args) : SinglelinkedList<T>()
	{
		if (!args.size())
			return;

		std::shared_ptr<Unit> unitEle = _top;
		int pos = randGenUnitPos(args.size());
		std::shared_ptr<Unit> posUnit;
		int idx = 0;
		for (auto ptr = args.begin(); ptr != args.end(); ++ptr, ++idx)
		{
			unitEle->_next = std::make_shared<Unit>(*ptr, nullptr);
			unitEle = unitEle->_next;
			if (idx == pos)
				posUnit = unitEle;
		}

		unitEle->_next = posUnit;
	}

	template<typename Policy>
	void breakCycle()
	{
		Policy::breakCycle(_top);
	}

	template<typename Policy>
	bool checkCycle()
	{
		return Policy::checkCycle(_top);
	}

	template<typename CB>
	void foreach_sleep(CB &&cb,int milc = 100, int test_count = 50)
	{
		std::shared_ptr<Unit> ptr = _top->_next;
		int idx = 0;
		while (ptr.get() != nullptr)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(milc));
			cb(ptr->_val);
			ptr = ptr->_next;
			if (++idx >= 50)
				break;
		}
	}

private:
	size_t randGenUnitPos(size_t count)
	{
		std::default_random_engine randEngine(time(NULL));
		std::uniform_int_distribution<int> uider(0, count);
		return uider(randEngine);
	}
};

//main
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
	
	CycleSinglelinkedList<int> cycleLst{ 5,9,6,10,3,4,7 };
	cycleLst.foreach_sleep(print);
	if (cycleLst.checkCycle<CheckCyclePolicy<int, CycleSinglelinkedList> >())
	{
		std::cout << "\nhas a cycle\n";
		cycleLst.breakCycle<TortoiseAndRabbitPolicy<int, CycleSinglelinkedList> >();
	}
	
	std::cout << "\nbreak cycle...\n";
	cycleLst.foreach(print);
	getchar();
	return 1;
}

