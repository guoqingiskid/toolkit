#include <iostream>
#include "study1.h"
#include "study2.h"
#include <assert.h>
#include <typeinfo>
#include <string>

template<typename T>
struct MyBase
{
  	void show();
};

template<>
struct MyBase<int>
{
	void show()
	{
		std::cout << "int base\n";
	}
};

template<>
struct MyBase<float>
{
	void show()
	{
		std::cout << "float base\n";
	}
};

template<>
struct MyBase<std::string>
{
	void show()
	{
		std::cout << "std::string base\n";
	}
};


int main1(int argc, char **argv)
{
	//study1
	MyContainer<int, std::vector<int> > my1;
	MyContainer<float, std::deque<float> > my2;
	MyContainer1<int> my3;
	MyPolicyHandler<int, SedPolicy> my4;

	MyTest<int, false> my;
	int test = 1;
	my.do_some_thing(&test);
	MyClass1 my5;
	my5.show();

	//study2
	int myArray[Length<TYPELIST_2(int, char)>::Value];
	std::cout << Length<TYPELIST_2(int, char)>::Value << std::endl;

	TypeListAt<TYPELIST_4(float, uint32_t, char, std::string), 3>::Result t = "hello";
	std::cout << "t = " << t << std::endl;
	std::cout << "index of: " << IndexOf<TYPELIST_4(int, char, int64_t, double), int>::Index << std::endl;
	std::cout << "append new type, new type index of " <<
		IndexOf<Append<TYPELIST_2(int, char), float>::Result, float>::Index << std::endl;

	std::cout << "remove dupclicate type: " << Length<RemoveDuplicate<TYPELIST_5(int, char, int, double, float)>::Result>::Value << std::endl;

	GenClass<TYPELIST_3(int, float, std::string), MyBase> myclass;
	get<0>(myclass).show();
	get<1>(myclass).show();
	get<2>(myclass).show();


	return 0;
}

#include<stdio.h>
#include<netdb.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<string.h>
int main(int argc, char *argv[])
{
    if(argc<2)
    {
        printf("the argc need more two\n");
        return 1;
    }
    struct hostent *host;
    const char *add = argv[1];
    char p[30];
    inet_pton(AF_INET, add, p);
    host = gethostbyaddr(p, strlen(p), AF_INET);
	if(host)
    	printf("hostname:%s\n",host->h_name);
	else
		printf("no thing!\n");
    return 0;
}
