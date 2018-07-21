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

  using TestHandle = std::reference_wrapper<Test>;
  using PrimaryOutputHandle = std::reference_wrapper<PrimaryOutput>;

  public:
    
    Endpoint(Split, Tran, Test&);
	  Endpoint(Split, Tran, PrimaryOutput&);	

    float slack() const;

    inline Split split() const;
    inline Tran tran() const;

    bool is_po() const;
    bool is_test() const;

    bool operator <  (const Endpoint&) const;
    bool operator >  (const Endpoint&) const;
    bool operator == (const Endpoint&) const;

  private:

    Split _el;
    Tran _rf;

    std::variant<TestHandle, PrimaryOutputHandle> _handle;

    Test& _test();
    PrimaryOutput& _po();
};

// Function: split
inline Split Endpoint::split() const {
  return _el;
}

// Function: tran
inline Tran Endpoint::tran() const {
  return _rf;
}

// ------------------------------------------------------------------------------------------------

/*// Class: EndpointHeap
class EndpointHeap {
  
  public:

    void clear();
    void remove(Endpoint&);
    void insert(Endpoint&);

    bool empty() const;
    
    size_t size() const;

    Endpoint* pop();
    Endpoint* top() const;

  private:
    
    std::vector<Endpoint*> _array;

    bool _cmp_slack(const Endpoint&, const Endpoint&) const;
    void _bubble_up(size_t, Endpoint&);
    void _bubble_down(size_t, Endpoint&);
};
*/

};  // end of namespace ot. -----------------------------------------------------------------------

#endif 






