#ifndef LL_ATOMIC_H
#define LL_ATOMIC_H 
#include <stdint.h>
#include <type_traits>
#include <utility>
template<typename T> struct isIntegerType : std::false_type{};
template<> struct isIntegerType<int32_t> : std::true_type{};
template<> struct isIntegerType<uint32_t> : std::true_type{};
template<> struct isIntegerType<uint16_t> : std::true_type{};
template<> struct isIntegerType<int16_t> : std::true_type{};
template<> struct isIntegerType<int8_t> : std::true_type{};
template<> struct isIntegerType<uint8_t> : std::true_type{};
template<> struct isIntegerType<bool> : std::true_type{};
template<typename T>
struct isInt{
	static constexpr bool value = std::is_same<isIntegerType<T>, std::true_type>::value;
};


/**
 * AtomicIntegerT template class is used to represent atomic value.
 * due to linux atomic api is obscure, AtomicIntegerT apply easy-to-use interfaces.
 */
template<typename T>
class AtomicIntegerT{

//static_assert(detail::isInt<T>::value, "AtomicIntegerT<T> T must be ints famliy.");
	
public:

	template<typename U>
	AtomicIntegerT(const U& u) : _value(static_cast<T>(u)){}

	AtomicIntegerT() : _value(0){}

	template<typename U>
	AtomicIntegerT(const AtomicIntegerT<U>& other) = delete;

	template<typename U>
	AtomicIntegerT<T>& operator=(const AtomicIntegerT<U>& other) = delete;

	template<typename U>
	AtomicIntegerT(AtomicIntegerT<U>&& other) : _value(std::move(other._value)){
		other.getAndSet(0);
	}

	template<typename U>
	AtomicIntegerT<T>& operator=(AtomicIntegerT<U>&& other){
		if(this != &other){
			getAndSet(other._value);
			other.getAndSet(0);
		}
		return *this;
	}

	template<typename U>
	AtomicIntegerT<T>& operator=(const U& u){
		getAndSet(u);
		return *this;
	}

	operator T() const{
		return const_cast<AtomicIntegerT*>(this)->get();
	}

	T operator++(){
		return addAndGet(1);
	}

	T operator++(int){
		return getAndAdd(1);
	}

	T operator--(){
		return addAndGet(-1);
	}

	T operator--(int){
		return getAndAdd(-1);
	}

	T get(){
		return __sync_val_compare_and_swap(&_value, 0, 0);
	}

private:
	
	T getAndAdd(T x){
		return __sync_fetch_and_add(&_value, x);
	}

	T addAndGet(T x){
		return getAndAdd(x) + x;
	}

	void add(T x){
		getAndAdd(x);
	}

	T getAndSet(T newValue){
		return __sync_lock_test_and_set(&_value, newValue);
	}

private:
  volatile T  _value;
};

using AtomicInt32 = AtomicIntegerT<int32_t>;
using AtomicInt8    = AtomicIntegerT<int8_t>;
using AtomicIntBool = AtomicIntegerT<bool>;

#endif
