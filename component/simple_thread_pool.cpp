#include "thread_pool.h"
#include "logger.h"
#include <sys/types.h>
#include <algorithm>

ThreadPool::ThreadPool()
	: _isRunning(false){

}

ThreadPool::~ThreadPool(){
	if(_isRunning){
		this->stop();
	}
}

void ThreadPool::start(int numThreads){

	_isRunning = true;
	for(int idx = 0; idx < numThreads; ++idx){
		_threads.push_back(std::move(std::thread(&ThreadPool::runInThread, this)));
	}
	_maxTaskSize = numThreads * 10;
	LOG_INFO("start task threads.");
}

void ThreadPool::stop(){
	{
		std::lock_guard<std::mutex> lockGuard(_mtx);
		_notEmpty.notify_all();
	}

	_isRunning = false;
	std::for_each(_threads.begin(), _threads.end(), [&](std::thread& taskThread){
		taskThread.join();
	});
}

void ThreadPool::run(Task&& task){
	if(_threads.empty()){
		task();
	}
	else {
		std::unique_lock<std::mutex> lock(_mtx);
		while(full()){
			_notFull.wait(lock);
		}
		_tasks.push_back(std::move(task));

		_notEmpty.notify_all();
	}
}

ThreadPool::Task ThreadPool::take(){
	std::unique_lock<std::mutex> lock(_mtx);
	while(empty() && _isRunning){
		_notEmpty.wait(lock);
	}

	Task task ;
	if(!empty()){
		task = _tasks.front();
		_tasks.pop_front();
	}
	return task;
}

void ThreadPool::runInThread(){
	static int threadNo = 0;
	LOG_INFO("start thread: %d", threadNo++);
	while(_isRunning){
		Task task(take());
		if(task){
			task();
		}
	}
}


