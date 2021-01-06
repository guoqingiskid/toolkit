#ifndef LL_THREAD_POOL_H
#define LL_THREAD_POOL_H
#include "atomic.h"
#include <vector>
#include <mutex>
#include <thread>
#include <deque>
#include <functional>
#include <condition_variable>


class ThreadPool{
public:
	typedef std::function<void ()> Task;
	ThreadPool();
	~ThreadPool();
	void start(int numThreads);
  	void stop();
	void run(Task&& task);
	
private:
	ThreadPool::Task take();
	void runInThread();	
	bool full() { return _tasks.size() == static_cast<std::size_t>(_maxTaskSize);}
	bool empty(){ return _tasks.empty();}
	
private:
	std::vector<std::thread>	_threads;
	int 						_poolSize;
	int 						_maxTaskSize;
	AtomicIntBool				_isRunning;
	std::deque<Task>			_tasks;
	std::mutex					_mtx;
	std::condition_variable		_notEmpty;
	std::condition_variable		_notFull;

};

#endif
