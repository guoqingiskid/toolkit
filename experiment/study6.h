#ifndef STUDY6_H
#define STUDY6_H
#include <type_traits>
#include <string>
class Person
{
public:

	template<typename T, typename = typename std::enable_if<std::is_constructible<T, std::string>::value>::type >
	Person(T &&name)
		:_name(std::forward<T>(name))
	{
		std::cout << "Person T 构造函数\n";
	}

	Person(const Person &other)
		:_name(other._name)
	{
		std::cout << "Person cont 引用拷贝构造\n";
	}

	Person(Person &&other)
		:_name(std::move(other._name))
	{
		std::cout << "Person 移动构造\n";
	}

private:
	std::string _name;
};

#endif
