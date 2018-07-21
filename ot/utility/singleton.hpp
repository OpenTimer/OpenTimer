#ifndef OT_UTILITY_SINGLETON_HPP_
#define OT_UTILITY_SINGLETON_HPP_

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <memory>

namespace ot {

// Class: EnableSingletonFromThis
// Guarantees that only a single instance of an object will exist throughout the
// lifetime of the program.
template <typename Derived>
class EnableSingletonFromThis {
    
  public:

    static Derived& get(); 

  protected:
    
    EnableSingletonFromThis() = default; 
    ~EnableSingletonFromThis() = default;                                        

  private:
    
    //EnableSingletonFromthis(const EnableSingletonFromThis&) = delete;
    //EnableSingletonFromthis(EnableSingletonFromThis&&) = delete;
    EnableSingletonFromThis & operator = (const EnableSingletonFromThis&) = delete;      
    EnableSingletonFromThis & operator = (EnableSingletonFromThis&&) = delete;   
};

// Function: get
template <typename Derived>
Derived& EnableSingletonFromThis<Derived>::get() {
  static Derived object;
  return object;
}

};  // End of ot namespace. --------------------------------------------------------------

#endif


