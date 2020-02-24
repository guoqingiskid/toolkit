#program once
#include <thread>
#include <mutex>
#include <assert.h>

template<typename T>
struct has_no_destroy
{
  template <typename C> static char test(decltype(&C::no_destroy));
  template <typename C> static int32_t test(...);
  const static bool value = sizeof(test<T>(0)) == 1;
};
}

template<typename T>
class Singleton 
{
 public:
 template<typename...Args>
  static T& instance(Args&& ...args)
  {
    std::call_once(&_onceFlag, &Singleton::init<Args...>, std::forward<Args>(args)...);
    assert(_instance != NULL);
    return *_instance;
  }

 private:
  Singleton();
  ~Singleton();

  template<typename... Args>
  static void init(Args&& ...args)
  {
    _instance = new T(std::forward<Args>(args)...);
    if (!detail::has_no_destroy<T>::value)
    {
      ::atexit(destroy);
    }
  }

  static void destroy()
  {
    typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
    T_must_be_complete_type dummy; (void) dummy;

    delete _instance;
    _instance = NULL;
  }

 private:
  static std::once_flag 	_onceFlag;
  static T*             	_instance;
};

template<typename T>
std::once_flag Singleton<T>::_onceFlag;

template<typename T>
T* Singleton<T>::_instance = NULL;
