#ifndef OT_TIMER_INDEX_HPP_
#define OT_TIMER_INDEX_HPP_

#include <vector>

namespace ot {

// Class: IndexGenerator
template <typename T>
class IndexGenerator {

  static_assert(std::is_integral_v<T>, "IndexGenerator must take an integral type");
  
  public:

    template <typename... Args>
    IndexGenerator(Args&&...);

    T get();
    
    void recycle(T&&);
    void recycle(const T&);
   
  private:
    
    T _counter;
    std::vector<T> _recycle;
};

// Procedure: IndexGenerator
template <typename T>
template <typename... Args>
IndexGenerator<T>::IndexGenerator(Args&&... args) : _counter {std::forward<Args>(args)...} {
}

// Procedure: get
template <typename T>
T IndexGenerator<T>::get() {
  if(_recycle.empty()) {
    return _counter++;
  }
  T idx = _recycle.back();
  _recycle.pop_back();
  return idx;
}

// Procedure: recycle
template <typename T>
void IndexGenerator<T>::recycle(T&& idx) {
  _recycle.push_back(std::move(idx));
}

// Procedure: recycle
template <typename T>
void IndexGenerator<T>::recycle(const T& idx) {
  _recycle.push_back(idx);
}


};  // end of namespace ot. -----------------------------------------------------------------------


#endif
