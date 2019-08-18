#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <algorithm>
#include "type_private.h"

/** 一个任务会保存多个号码，任务以预拨时间为基准 */
template<typename T,int MAX_CONCURRENT = 20>
class TaskQueue
{
public:
	using value_t = typename std::vector<T>::value_type;
	using task_list = std::vector<T>;

public:
	TaskQueue(const TaskQueue &other) = delete;
	TaskQueue& operator=(const TaskQueue &other) = delete;
	TaskQueue() = default;

	/***/
	void push_task(T &&t,int size)
	{
		std::lock_guard<std::mutex> lc(_mutex);
		insert_task(std::forward<T&&>(t));
		sort_task();
		
		if(isNotify(size))
			_cv.notify_one();
	}

	bool isNotify(int size)
	{
		int task_size = 0;
		for (int idx = 0; idx < _tasks.size(); ++idx)
		{
			task_size += _tasks[idx].second.size();
		}

		return size <= task_size ? true : false;
	}

	value_t pop_task()
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_cv.wait(lock,[this]() {return !this->_tasks.empty(); });

		value_t val = _tasks.back();
		_tasks.pop_back();
		return val;
	}

	bool try_pop_back(value_t &val)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		if (_tasks.empty())
			return false;

		val = _tasks.back();
		_tasks.pop_back();
		return true;
	}

private:
	
	void insert_task(T &&t)
	{
		auto ptr = std::find_if(_tasks.begin(), _tasks.end(), [&t](auto &item) {
			if (item.first == t.first)
				return true;
			return false;
		});

		if (ptr != _tasks.end() && ptr->second.size() < MAX_CONCURRENT)
		{
			ptr->second.push_back(t.second[0]);	
			return;
		}
		
		_tasks.push_back(t);
	}


	void sort_task()
	{
		std::stable_sort(_tasks.begin(), _tasks.end(), [](auto &fst,auto &sed) {
			if (fst.first < sed.first)
				return true;
			return false;
		});
	}
private:
	task_list _tasks;
	mutable std::mutex _mutex;
	mutable std::condition_variable _cv;
};


template<typename T>
class thread_safe_queue{
private:
    using queue_t = std::queue<T>;
    using value_type= typename queue_t::value_type;
    using container_type = typename queue_t::container_type;
    mutable std::mutex _mtx;
    mutable std::condition_variable _cond;
    queue_t _que;
public:
    thread_safe_queue()=default;
    thread_safe_queue(const thread_safe_queue&)=delete;
    thread_safe_queue& operator=(const thread_safe_queue&)=delete;
    template<typename _InputIterator>
    thread_safe_queue(_InputIterator first, _InputIterator last){
        for(auto itor=first;itor!=last;++itor){
            _que.push(*itor);
        }
    }
    explicit thread_safe_queue(const container_type &c):_que(c){}
    thread_safe_queue(std::initializer_list<value_type> list):thread_safe_queue(list.begin(),list.end()){
    }
    void push(const value_type &new_value){
        std::lock_guard<std::mutex>lk(_mtx);
        _que.push(std::move(new_value));
        _cond.notify_one();
    }
    value_type wait_and_pop(){
        std::unique_lock<std::mutex>lk(_mtx);
        _cond.wait(lk,[this]{return !this->_que.empty();});
        auto value=std::move(_que.front());
        _que.pop();
        return value;
    }
    bool try_pop(value_type& value){
        std::lock_guard<std::mutex>lk(_mtx);
        if(_que.empty())
            return false;
        value=std::move(_que.front());
        _que.pop();
        return true;
    }
    bool try_pop(value_type& value, size_t ms){
        std::unique_lock<std::mutex>lk(_mtx);
        if(_cond.wait_for(lk, std::chrono::milliseconds(ms), [&](){ return !_que.empty(); }))
        {
            value = std::move(_que.front());
            _que.pop();
            return true;
        }
        return false;
    }
    auto empty() const->decltype(_que.empty()) {
        std::lock_guard<std::mutex>lk(_mtx);
        return _que.empty();
    }
    auto size() const->decltype(_que.size()){
        std::lock_guard<std::mutex>lk(_mtx);
        return _que.size();
    }
};

