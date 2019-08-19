#pragma once
#include <new>

template<typename T>
class NewHandlerHolder
{
public:
	NewHandlerHolder(const NewHandlerHolder &other) = delete;
	NewHandlerHolder &operator=(const NewHandlerHolder &other) = delete;
	NewHandlerHolder() = default;
	NewHandlerHolder(std::new_handler new_handler) 
	{
		_new_handler = new_handler;
	}

	static std::new_handler set_new_handler(std::new_handler new_handler)
	{
		std::new_handler old_handler = _new_handler;
		_new_handler = new_handler;
		return old_handler;
	}

	static void* operator new[](std::size_t size) throw(std::bad_alloc)				//自定义operator new
	{
		NewHandlerHolder<T> newHandlerHolder(std::set_new_handler(_new_handler));	//在new的时候设置了我们自己的回调函数，在该函数结束又将全局回调函数设置回去
		return ::operator new[](size);
	}

	static void* operator new(size_t size) throw(std::bad_alloc)
	{
		NewHandlerHolder<T> newHandlerHolder(std::set_new_handler(_new_handler));	//在new的时候设置了我们自己的回调函数，在该函数结束又将全局回调函数设置回去
		return ::operator new(size);
	}

	~NewHandlerHolder()
	{
		std::set_new_handler(_new_handler);
	}
private:
	static std::new_handler _new_handler;
};

template<typename T>
std::new_handler NewHandlerHolder<T>::_new_handler = nullptr;


//maim.cpp
void handler()
{
	std::cout << "bad memroy....\n";
	throw std::bad_alloc();
}
class Test : public NewHandlerHolder<Test>
{
public:
	Test()
	{
		Test::set_new_handler(handler);
		_arr = new Test[10240000];
	}
	void show()
	{
		std::cout << "test.....\n";
	}
	Test *_arr;
};

class Test2 : public NewHandlerHolder<Test>
{
public:
	Test2()
	{
		_arr = new Test2[10240000];
	}
	void show()
	{
		std::cout << "test.....\n";
	}
	Test2 *_arr;
};

void handle2()
{
	std::cout << "handler2...\n";
}
int main()
{
	std::set_new_handler(handle2);
	try
	{
		//Test::set_new_handler(handler);
		Test *ptr = new Test[10000000L];
	}
	catch (std::bad_alloc &ex)
	{
		Test2 *ptr = new Test2[10000000L];
	}
	
	getchar();
	return 1;
}