#ifndef OT_UTILITY_UNIQUE_GUARD_HPP_
#define OT_UTILITY_UNIQUE_GUARD_HPP_

#include <memory>
#include <mutex>

namespace ot {

// Class: UniqueGuard
template <typename T, typename M = std::mutex>
class UniqueGuard {

  class Deleter {

    public:
     
      Deleter(std::unique_lock<M>&&);

      void operator()(T*);

    private:

		  std::unique_lock<M> _lock;
  };
  
  using handle = std::unique_ptr<T, Deleter>;

  public:

    template <typename... ArgsT>
    UniqueGuard(ArgsT&&...);

    auto get();
    auto try_get();

  private:

    T _obj;
    M _mutex;
};

// Deleter
template <typename T, typename M>
UniqueGuard<T, M>::Deleter::Deleter(std::unique_lock<M>&& rhs) : _lock {std::move(rhs)} {
}

// Operator
template <typename T, typename M>
void UniqueGuard<T, M>::Deleter::operator()(T* ptr) {
  if(ptr) {
    _lock.unlock();
  }
}

// Constructor
template <typename T, typename M>
template <typename... ArgsT>
UniqueGuard<T, M>::UniqueGuard(ArgsT&&... args) : _obj {std::forward<ArgsT>(args)...} {
}

// Function: get
template <typename T, typename M>
auto UniqueGuard<T, M>::get() {
  std::unique_lock l(_mutex);
  return handle(&_obj, Deleter(std::move(l)));
}

// Function: try_get
template <typename T, typename M>
auto UniqueGuard<T, M>::try_get() {
  std::unique_lock l(_mutex, std::try_to_lock);
	if(l.owns_lock()) {
    return handle(&_obj, Deleter(std::move(l)));
  }
  else {
    return handle(nullptr, Deleter(std::move(l)));
  }
}


};  // end of namespace ot. -----------------------------------------------------------------------

#endif
