#pragma once
#include <iostream>
#include <initializer_list>

template<typename T>
class SparseArrayTraits
{
public:
	static const T default_value;
};

template<typename T>
const T SparseArrayTraits<T>::default_value = T();

template<>
class SparseArrayTraits<int>
{
public:
	static const int default_value = 0xffffffff;
};

template<typename T>
class SparseArray
{
public:
	typedef T*	pointer;
	typedef T	value_type;
private:
	const int _default_value = SparseArrayTraits<T>::default_value;
	typedef struct _ColUnit
	{
		size_t		_col_number;
		T			_val;
		_ColUnit	*_next;

		_ColUnit(size_t num, const T &val, _ColUnit *next) : _col_number(num), _val(val), _next(next) {}
		~_ColUnit() { delete _next;}
	}ColUnit;

	typedef struct _RowUnit      //每行都放置一个哨兵
	{
		size_t		_row_number;
		_RowUnit	*_next;
		ColUnit		*_pCol;

		_RowUnit(size_t num, _RowUnit *next, ColUnit *pCol) : _row_number(num), _next(next), _pCol(pCol) {}
		~_RowUnit() { delete _next; delete _pCol;}
	}RowUnit;

	class AuxPrivate
	{
	public:
		SparseArray *_parent;
		RowUnit *_rowUnit;
		AuxPrivate() = default;
		AuxPrivate(SparseArray *unit, RowUnit *rowUnit) : _parent(unit), _rowUnit(rowUnit) {}
		void init(SparseArray *parent,RowUnit *rowunit)
		{
			_parent = parent;
			_rowUnit = rowunit;
		}

		T &operator[](size_t idx)
		{
			return _parent->findColEx(idx, _rowUnit, true)->_val;
		}
	};

	RowUnit *_top;
	AuxPrivate _aux;
public:
	SparseArray()
	{
		_top = new RowUnit(-1, nullptr, nullptr);
	}

	~SparseArray()
	{
		delete _top;
	}

	bool setValue(size_t row, size_t col, const T &val)
	{
		RowUnit *rowEle = findRowEx(row, true);
		if (rowEle)
		{
			ColUnit *colEle = findColEx(col, rowEle, true);
			if (colEle)
			{
				colEle->_val = val;
				return true;
			}
		}

		return false;
	}

	bool getValue(size_t row, size_t col, T &val)
	{
		ColUnit *colEle = findColEx(col, findRowEx(row, false), false);
		if (colEle)
			val = colEle->_val;
		return colEle ? true : false;
	}

	bool deleteValue(size_t row, size_t col)
	{
		//删除元素只能自己写遍历，不能使用findColEx，我们需要的是待删除元素的前一个元素
		RowUnit *rowEle = findRowEx(row, false);
		if (rowEle)
		{
			ColUnit *colEle = rowEle->_pCol;
			while (colEle->_next && colEle->_next->_col_number < col)
			{
				colEle = colEle->_next;
			}

			if (colEle->_next && colEle->_next->_col_number == col)
			{
				ColUnit *ptr = colEle->_next->_next;
				delete colEle->_next;
				colEle->_next = ptr;
				return true;
			}
		}
		return false;
	}

	template<typename CB>
	void foreach(CB &&cb)
	{
		RowUnit *rowEle = _top->_next;
		while (rowEle)
		{
			ColUnit *colEle = rowEle->_pCol->_next;
			while (colEle)
			{
				cb(colEle->_val);
				colEle = colEle->_next;
			}
			rowEle = rowEle->_next;
		}
	}

	AuxPrivate &operator[](size_t idx)
	{
		RowUnit *rowEle =  findRowEx(idx, true);
		_aux.init(this, rowEle);
		return _aux;
	}

	const AuxPrivate &operator[](size_t idx)const
	{
		RowUnit *rowEle = findRowEx(idx, true);
		_aux.init(this, rowEle);
		return _aux;
	}

private:
	RowUnit *findRowEx(size_t row, bool isCreated)       //行类型是排序的
	{
		RowUnit *ptr = _top;
		while (ptr->_next && ptr->_next->_row_number < row)
		{
			ptr = ptr->_next;
		}

		if (ptr->_next && ptr->_next->_row_number == row) {}
		else if(isCreated)
		{
			RowUnit *newEle = new RowUnit(row, ptr->_next, new ColUnit(_default_value, _default_value, nullptr));
			ptr->_next = newEle;
		}

		return ptr->_next;
	}

	ColUnit *findColEx(size_t col, const RowUnit *rowUnit, bool isCreated)
	{
		if (!rowUnit)
		{
			return nullptr;
		}

		const ColUnit *ptr = rowUnit->_pCol;
		while (ptr->_next && ptr->_next->_col_number < col)
		{
			ptr = ptr->_next;
		}

		if (ptr->_next && ptr->_next->_col_number == col) {}
		else if (isCreated)
		{
			ColUnit *newEle = new ColUnit(col, _default_value, ptr->_next);
			const_cast<ColUnit*>(ptr)->_next = newEle;
		}

		return ptr->_next;
	}
};



//main.cpp
void test_arr()
{
	SparseArray<int> sparseArr;
	sparseArr.setValue(0, 0, 5);
	sparseArr.setValue(0, 3, 10);
	sparseArr.setValue(0, 7, 15);
	sparseArr.setValue(2, 2, 20);
	sparseArr.setValue(3, 5, 25);
	sparseArr.deleteValue(2, 2);
	sparseArr.foreach(print_ele);
}

int main()
{
	test_arr();
	getchar();
	return 1;
}
