#ifndef OT_TIMER_TEST_HPP_
#define OT_TIMER_TEST_HPP_

#include <ot/liberty/celllib.hpp>

namespace ot {

// Forward declaration
class Pin;
class Arc;

// ------------------------------------------------------------------------------------------------

// Class: Test
class Test {
  
  friend class Timer;
  friend class Gate;
  friend class Cppr;
  friend class Endpoint;

  public:
    
    Test(Arc&);

    std::optional<float> rat(Split, Tran) const;
    std::optional<float> slack(Split, Tran) const;

    const Pin& constrained_pin() const;
    const Pin& related_pin() const;

  private:

    Arc& _arc;
    
    std::optional<std::list<Test>::iterator> _satellite;
    
    std::array<std::array<std::optional<float>, MAX_TRAN>, MAX_SPLIT> _rat;
    std::array<std::array<std::optional<float>, MAX_TRAN>, MAX_SPLIT> _cppr_credit;

    void _reset();
    void _fprop_rat(float);
    
    Pin& _constrained_pin();
    Pin& _related_pin();
};



};  // end of namespace ot. -----------------------------------------------------------------------


#endif








