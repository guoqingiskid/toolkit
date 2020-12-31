#ifndef LL_QUEUE_H
#define LL_QUEUE_H
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <algorithm>

template<typename T>
class ThreadQueue{
private:
    using queue_t = std::queue<T>;
    using value_type= typename queue_t::value_type;
    using container_type = typename queue_t::container_type;
    mutable std::mutex _mtx;
    mutable std::condition_variable _cond;
    queue_t _que;
public:
    ThreadQueue()=default;
    ThreadQueue(const thread_safe_queue&)=delete;
    thread_safe_queue& operator=(const thread_safe_queue&)=delete;
	
    template<typename _InputIterator>
    ThreadQueue(_InputIterator first, _InputIterator last){
        for(auto itor=first; itor!=last; ++itor){
            _que.push(*itor);
        }
    }
    explicit ThreadQueue(const container_type& c):_que(c){}
    ThreadQueue(const std::initializer_list<value_type> &list)
    	: ThreadQueue(list.begin(),list.end()){
    }
    void push(const value_type& new_value){
        std::lock_guard<std::mutex>lk(_mtx);
        _que.push(std::move(new_value));
        _cond.notify_one();
    }
    value_type waitPop(){
        std::unique_lock<std::mutex>lk(_mtx);
        _cond.wait(lk,[this]{return !this->_que.empty();});
        auto value=std::move(_que.front());
        _que.pop();
        return value;
    }
    bool pop(value_type& value){
        std::lock_guard<std::mutex>lk(_mtx);
        if(_que.empty())
            return false;
        value=std::move(_que.front());
        _que.pop();
        return true;
    }
    bool tryPop(value_type& value, size_t ms){
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

#endif
