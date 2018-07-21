#ifndef OT_UTILITY_LAMBDA_HPP_
#define OT_UTILITY_LAMBDA_HPP_

namespace ot {

template <typename T>
struct CopyOnMove {

  CopyOnMove(T&& rhs) : object(std::move(rhs)) {}
  CopyOnMove(const CopyOnMove& other) : object(std::move(other.object)) {}

  T& get() { return object; }
  
  mutable T object; 
};


};  // end of namespace ot. -----------------------------------------------------------------------

#endif
