#pragma once
#include <algorithm>

//一些有用的traits
template<typename Iterator>
class SortValueTypeTraits                    //for STL !!!
{
public:
	typedef typename Iterator::value_type value_type;
};

template<typename T>
class SortValueTypeTraits<T*>				//for raw array!!!
{
public:
	typedef T value_type;
};

template<typename T>
class IteratortypeTraits					//for STL!!!
{
public:
	typedef typename T::itertaor iter;
};

template<typename T, size_t N>
class IteratortypeTraits<T[N]>				//for raw array!!!
{
public:
	typedef typename T* iter;
};

//插入排序
template<typename Iterator, typename Compare = std::less<> >
void insert_sort(Iterator fst, Iterator lst)
{
	Compare comFunc;
	Iterator ptr = fst + 1;
	while (ptr != lst)
	{
		Iterator headPtr = fst;
		while (headPtr != ptr)
		{
			if (comFunc(*headPtr, *ptr))
			{
				SortValueTypeTraits<Iterator>::value_type tmp = *headPtr;
				*headPtr = *ptr;
				*ptr = tmp;
			}

			++headPtr;
		}
		++ptr;
	}
}

//选择排序
template<typename Iterator, typename Compare = std::less<> >
void select_sort(Iterator fst, Iterator lst)
{
	Compare comFunc;
	Iterator ptr = fst;
	while (ptr != lst - 1)     //每一趟都取一个最值
	{
		Iterator nextPtr = ptr;
		SortValueTypeTraits<Iterator>::value_type most_val = *nextPtr;
		while (nextPtr != lst)
		{
			if (comFunc(*nextPtr, most_val))
			{
				SortValueTypeTraits<Iterator>::value_type tmp = most_val;
				most_val = *nextPtr;
				*nextPtr = tmp;
			}

			++nextPtr;
		}
		*ptr = most_val;
		++ptr;
	}
}

//冒泡排序
template<typename Iterator, typename Compare = std::less<> >
void bubble_sort(Iterator fst, Iterator lst)
{
	Compare comFunc;
	Iterator ptr = lst;
	Iterator nextPtr = fst; 
	while (ptr != fst + 1)
	{
		nextPtr = fst;
		while (nextPtr + 1 != ptr)           //每次遍历都需要冒泡一个元素到已排序子集合中
		{
			if (!comFunc(*nextPtr, *(nextPtr + 1)))
			{
				SortValueTypeTraits<Iterator>::value_type tmp = *nextPtr;
				*nextPtr = *(nextPtr + 1);
				*(nextPtr + 1) = tmp;
			}

			++nextPtr;
		}
		--ptr;
	}
}


//堆排序
namespace HeapSort
{
	//第一步建立堆结构：一个具有根节点为最值的完全二叉树
	template<typename Iterator,typename Compare = std::less<> >
	void make_heap(Iterator fst, Iterator lst)					//原始数组从第一个元素开始一步步建立堆结构
	{
		Compare comFunc;
		size_t arrSize = std::distance(fst, lst);
		
		//已知子节点i，得到父节点:(i-1)/2，这个公式不论当前子节点是左还是右都能准确找到父节点。
		Iterator ptr = fst + 1;
		size_t curIndex = 1;
		while (ptr != lst)
		{
			size_t  parentIndex = curIndex;
			Iterator nextPtr = ptr;
			while (parentIndex)									//如果父节点不为根节点就需要一直比较
			{
				parentIndex = (parentIndex - 1) / 2;
				SortValueTypeTraits<Iterator>::value_type parentNodeVal = *(fst + parentIndex);
				if (comFunc(parentNodeVal, *nextPtr))				//如果当前加入堆的子节点比其父节点大或者小，那么交换值
				{
					SortValueTypeTraits<Iterator>::value_type tmp = parentNodeVal;
					*(fst + parentIndex) = *nextPtr;
					*nextPtr = tmp;
				}
				nextPtr = fst + parentIndex;
			}
			ptr = ptr + 1;
			curIndex++;
		}
	}

	//第二步删除堆的根节点，并维持堆结构，这里我们因为将堆保存在数组中，我们仅需要将堆的根与堆的最后个节点交互即可。
	template<typename Iterator, typename Compare = std::less<> >
	void remove_top(Iterator fst, Iterator lst)
	{
		Compare comFunc;
		size_t arrSize = std::distance(fst, lst);
		if (!arrSize)
			return;

		//直接将根元素与最后个节点交换
		SortValueTypeTraits<Iterator>::value_type tmp = *fst;
		*fst = *(fst + arrSize - 1);
		*(fst + arrSize - 1) = tmp;

		//这里来维持堆的属性：从根节点开始依次往下比较，因为下面的节点其实都满足堆。
		arrSize--;
		Iterator lst2 = fst + arrSize;

		//维持堆的属性：基本思路是，跟节点是很好得到最值，但是还要保证下面的所有节点都满足堆属性
		size_t curNodeIndex = 0;
		size_t leftIndex = 2 * curNodeIndex + 1 >= arrSize ? curNodeIndex : 2 * curNodeIndex + 1;
		size_t rightIndex = 2 * curNodeIndex + 2 >= arrSize ? curNodeIndex : 2 * curNodeIndex + 2;
		Iterator ptr = (fst + curNodeIndex);
		while (true)			//每次遍历调整一个节点
		{
			SortValueTypeTraits<Iterator>::value_type leftNodeVal = *(fst + leftIndex);
			SortValueTypeTraits<Iterator>::value_type rightNodeVal = *(fst + rightIndex);
			if (comFunc(leftNodeVal, *ptr) && comFunc(rightNodeVal, *ptr))
				break;
			Iterator mostValPtr = comFunc(leftNodeVal, rightNodeVal) ? fst + rightIndex : fst + leftIndex;
			SortValueTypeTraits<Iterator>::value_type tmp = *ptr;
			*ptr = *mostValPtr;
			*mostValPtr = tmp;

			curNodeIndex = std::distance(fst, mostValPtr);
			leftIndex = 2 * curNodeIndex + 1 >= arrSize ? curNodeIndex : 2 * curNodeIndex + 1;
			rightIndex = 2 * curNodeIndex + 2 >= arrSize ? curNodeIndex : 2 * curNodeIndex + 2;
			ptr = (fst + curNodeIndex);
			if (leftIndex == curNodeIndex || rightIndex == curNodeIndex)
				break;
		}
	}

	//堆排序
	template<typename Iterator, typename Compare = std::less<> >
	void heap_sort(Iterator fst, Iterator lst)
	{
		HeapSort::make_heap(fst, lst);
		size_t arrSize = std::distance(fst, lst);
		while (arrSize)
		{
			remove_top<Iterator,Compare>(fst, fst + arrSize);
			arrSize--;
		}
	}
}

//快速排序
namespace QuickSort
{
	template<typename Iterator, typename Compare = std::less<> >
	Iterator partition(Iterator fst, Iterator lst)
	{
		Compare compare;
		size_t arrSize = std::distance(fst, lst);
		SortValueTypeTraits<Iterator>::value_type divider = *(fst + arrSize - 1);		//简单使用数组最后个元素作为分界
		Iterator ptr = fst;
		size_t exchangePos = 0;
		while (ptr != lst)
		{
			if (compare(*ptr, divider))
			{
				SortValueTypeTraits<Iterator>::value_type tmp = *(fst + exchangePos);
				*(fst + exchangePos) = *ptr;
				*ptr = tmp;
				++exchangePos;
			}
			ptr = ptr + 1;
		}

		//将divider放置在exchangePos上
		SortValueTypeTraits<Iterator>::value_type tmp = *(fst + exchangePos);
		*(fst + exchangePos) = divider;
		*(fst + arrSize - 1) = tmp;
		return fst + exchangePos;
	}

	template<typename Iterator, typename Compare = std::less<> >
	void quick_sort(Iterator fst, Iterator lst)
	{
		if (fst == lst)
			return;
		Iterator dividerPtr = QuickSort::partition<Iterator, Compare>(fst, lst);
		quick_sort(fst, dividerPtr);
		quick_sort(dividerPtr + 1, lst);
	}
}

//归并排序
namespace MergeSort
{
	template<typename Iterator, typename Compare = std::less<> >
	void merge(Iterator fst, Iterator lst, Iterator fst2, size_t startPos, size_t endPos)
	{
		if (startPos == endPos)
			return;
		size_t midPos = (startPos + endPos) / 2;
		merge(fst, lst, fst2, startPos, midPos);
		merge(fst, lst, fst2, midPos + 1, endPos);
		MergeSort::partition<Iterator, Compare>(fst, lst, fst2, startPos, midPos);
	}

	template<typename Iterator, typename Compare = std::less<> >
	Iterator partition(Iterator fst, Iterator lst, Iterator fst2, size_t start, size_t end)
	{
		
	}
}

//main.cpp
int main()
{
	
	/** Test 1 */
	std::vector<int> vec{ 5,6,10,3,79,14,8,7 };
	std::string names[] = { "bhello","cworld","dliu","abc" };
	constexpr size_t arr_size = get_array_size(names);
	insert_sort(vec.begin(), vec.end());
	insert_sort<IteratortypeTraits<std::string[arr_size]>::iter, std::greater<> >(names, names + arr_size);
	std::copy(vec.begin(), vec.end(), std::ostream_iterator<int>(std::cout, " "));
	std::cout << "\n";
	std::copy(names, names + arr_size, std::ostream_iterator<std::string>(std::cout, " "));

	/** Test 2 */
	std::cout << "\n";
	std::vector<int> vec2{ 5,6,10,3,79,14,8,7 };
	select_sort(vec2.begin(), vec2.end());
	std::copy(vec2.begin(), vec2.end(), std::ostream_iterator<int>(std::cout, " "));

	/** Test 3 */
	std::cout << "\n";
	std::vector<int> vec3{ 5,6,10,3,79,14,8,7 };
	bubble_sort(vec3.begin(), vec3.end());
	std::copy(vec3.begin(), vec3.end(), std::ostream_iterator<int>(std::cout, " "));
	
	
	/** Test 4 */
	std::vector<int> vec4{ 5,6,10,3,79,14,8,7 };
	HeapSort::heap_sort(vec4.begin(), vec4.end());

	/*** Test 5 */
	std::vector<int> vec5{ 5,6,10,3,79,14,8,7 };
	QuickSort::quick_sort(vec5.begin(), vec5.end());

	getchar();
	return 1;
}
