#ifndef OT_TIMER_ENDPOINT_HPP_
#define OT_TIMER_ENDPOINT_HPP_

#include <ot/headerdef.hpp>
#include <ot/traits.hpp>

namespace ot {

// Forward declaration
class Test;
class PrimaryOutput;

// ------------------------------------------------------------------------------------------------

// Class: Endpoint
// A primary output port
// A data input pin of a sequential cell
// A pin that has an output delay specified. 
class Endpoint {

  friend class Timer;

  friend struct Path;

  public:
    
    Endpoint(Split, Tran, Test&);
	  Endpoint(Split, Tran, PrimaryOutput&);	

    float slack() const;

    inline Split split() const;
    inline Tran transition() const;

    inline const Test* test() const;
    inline const PrimaryOutput* primary_output() const;

    inline bool operator <  (const Endpoint&) const;
    inline bool operator >  (const Endpoint&) const;
    inline bool operator == (const Endpoint&) const;

  private:

    Split _el;
    Tran _rf;

    std::variant<Test*, PrimaryOutput*> _handle;
};

// Function: split
inline Split Endpoint::split() const {
  return _el;
}

// Function: transition
inline Tran Endpoint::transition() const {
  return _rf;
}

// Function: test
inline const Test* Endpoint::test() const {
  if(auto ptr = std::get_if<Test*>(&_handle)) {
    return *ptr;
  }
  else return nullptr;
}

// Function: primary_output
inline const PrimaryOutput* Endpoint::primary_output() const {
  if(auto ptr = std::get_if<PrimaryOutput*>(&_handle)){
    return *ptr;
  }
  else return nullptr;
}

// Operator <
inline bool Endpoint::operator < (const Endpoint& rhs) const {
  return slack() < rhs.slack();
}

// Operator >
inline bool Endpoint::operator > (const Endpoint& rhs) const {
  return slack() > rhs.slack();
}

// Operator ==
inline bool Endpoint::operator == (const Endpoint& rhs) const {
  return slack() == rhs.slack();
}

};  // end of namespace ot. -----------------------------------------------------------------------

#endif 






