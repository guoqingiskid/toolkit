#pragma once
#include <memory>
#include <vector>
#include <time.h>

uint32_t GCD(uint32_t A, uint32_t B)   //GCD(A,B)=GCD(B,A MOD B)
{
	int remainder = A % B;
	if (!remainder)
		return B;
	else
		return GCD(B, remainder);
}


//循环版本
#pragma once
#include <memory>
#include <vector>
#include <time.h>

uint32_t GCD(uint32_t A, uint32_t B)   //GCD(A,B)=GCD(B,A MOD B)
{
	while (B)
	{
		int remainder = A % B;
		A = B;
		B = remainder;
	}

	return A;
}

