#pragma once
#include <algorithm>
#include <iostream>

//一些有用的traits
template<typename Iterator>
class SearchValueTypeTraits						//for STL !!!
{
public:
	typedef typename Iterator::value_type value_type;
};

template<typename T>
class SearchValueTypeTraits<T*>					//for raw array!!!
{
public:
	typedef T value_type;
};

template<typename Iterator>
Iterator binary_search(Iterator fst, Iterator lst, const typename SearchValueTypeTraits<Iterator>::value_type &val)
{
	size_t maxIndex = std::distance(fst, lst);
	size_t minIndex = 0;
	while (minIndex < maxIndex)
	{
		size_t midIndex = (maxIndex + minIndex) / 2;
		if (val > *(fst + midIndex))
		{
			minIndex = midIndex + 1;
		}
		else if (val < *(fst + midIndex))
		{
			maxIndex = midIndex;
		}
		else
		{
			return fst + midIndex;
		}
	}

	return lst;
}

template<typename Iterator>
Iterator insert_search(Iterator fst, Iterator lst, const typename SearchValueTypeTraits<Iterator>::value_type &val)
{
	size_t maxIndex = std::distance(fst, lst);
	size_t minIndex = 0;
	typename SearchValueTypeTraits<Iterator>::value_type minVal = *fst;
	typename SearchValueTypeTraits<Iterator>::value_type maxVal = *(lst - 1);
	while (minIndex < maxIndex)
	{
		size_t midIndex = (maxIndex - minIndex)*(val - minVal) / (maxVal - minVal) + minIndex;		//估算一个大约的位置
		if (val > *(fst + midIndex))
		{
			minIndex = midIndex + 1;
		}
		else if (val < *(fst + midIndex))
		{
			maxIndex = midIndex;
		}
		else
		{
			return fst + midIndex;
		}
	}

	return lst;
}


//main.cpp
int main()
{
	/** Test 1 */
	std::vector<int> vec1{ 1,4,7,9,12,15,18,21,78 };
	auto ptr = ::insert_search(vec1.begin(), vec1.end(), 1);
	if (ptr != vec1.end())
	{
		std::cout << *ptr;
	}

	getchar();
	return 1;
}

