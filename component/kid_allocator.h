#ifndef KID_ALLOCATOR_H
#define KID_ALLOCATOR_H
#include <new>
#include <cstddef>
#include <cstdlib>
#include <climits>
#include <iostream>

namespace KID
{
	template<typename T>
	inline T* _allocate(ptrdiff_t size, T*)
	{
		std::set_new_handler(0);
		T *tmp = (T*)::operator new((size_t)(size * sizeof(T)));
		if (tmp == 0)
		{
			std::cerr << "out of memory" << std::endl;
			exit(1);
		}	
		return tmp;
	}

	template<typename T>
	inline void _deallocate(T *ptr)
	{
		::operator delete(ptr);
	}

	template<typename T1,typename T2>
	inline void _construct(T1 *ptr, const T2 &value)
	{
		new(ptr) T1(value);          //call T1 type's constructor
	}

	template<typename T>
	inline void _destory(T *ptr)    //call T1 type's destructor
	{
		ptr->~T();
	}

	/** interface:allocator */
	template<typename T>
	class allocator
	{
	public: 

		/** 严格遵守接口定义来 */
		typedef T          value_type ;
		typedef T*         pointer ;
		typedef const T*   const_pointer ;
		typedef T&			reference;
		typedef const T&	const_reference;
		typedef size_t		size_type;
		typedef ptrdiff_t	difference_type;

		template<typename U>
		struct rebind
		{
			typedef allocator<U> other;
		};

		pointer allocate(size_type n, const void *hint = 0)
		{
			return _allocate((difference_type)n, (pointer)0);
		}

		void deallocate(pointer ptr,size_type n)
		{
			_deallocate(ptr);
		}

		void construct(pointer p, const T& value)
		{
			_construct(p, value);
		}

		void destory(pointer ptr)
		{
			_destory(ptr);
		}

		pointer address(reference x)
		{
			return (pointer)&x;
		}

		const_pointer const_address(reference x)
		{
			return (const_pointer)&x;
		}

		size_type max_size()const
		{
			return size_type(UINT_MAX / sizeof(T));
		}
		
	};
}

#endif    //KID_ALLOCATOR_H
