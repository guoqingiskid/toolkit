#pragma once
#include <memory>
#include <vector>
#include <time.h>


//算法注意：当算5开始时，1*5 2*5 3*5 4*5 一定以及被算过了，所有仅需要算5*5开始的，因此如果5*5都大于了最大数那么就直接返回即可
std::vector<int> FindPrimes(int max_number)
{
	bool *ptr = new bool[max_number + 1];
	memset(ptr, 0, max_number + 1);

	//删除2的倍数
	for (int idx = 4; idx < max_number; idx += 2)
	{
		ptr[idx] = true;
	}

	int pos = 3;
	int stop_pos = sqrt(max_number);  //每次都计算到这里结束，然后下一轮
	while (pos <= stop_pos)          //注意pos和stop_pos每次都会变，当pos>stop_pos时，则表明没有非素数了，因为之前一定都删完非素数了。
	{
		int end = max_number / pos;
		for (int idx = 2; idx <= end; ++idx)
		{
			ptr[pos*idx] = true;
		}

		pos += 2;  //跳过偶数
		while (pos <= max_number && ptr[pos]) pos += 2;   // 如果走到被删除的位置上，就继续跳过
	}

	std::vector<int> res;
	for (int i = 2; i < max_number + 1; ++i)
		if (!ptr[i])
			res.push_back(i);
	return res;
}


