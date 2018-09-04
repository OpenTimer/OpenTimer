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
  friend class Endpoint;

  friend struct Path;

  public:
    
    Test(Arc&);

    std::optional<float> rat(Split, Tran) const;
    std::optional<float> constraint(Split, Tran) const;
    std::optional<float> slack(Split, Tran) const;
    std::optional<float> raw_slack(Split, Tran) const;
    std::optional<float> cppr_credit(Split, Tran) const;

    const Pin& constrained_pin() const;
    const Pin& related_pin() const;
    const Arc& arc() const;

  private:

    Arc& _arc;
    
    std::optional<std::list<Test>::iterator> _satellite;
    std::optional<std::list<Test*>::iterator> _pin_satellite;
    
    TimingData<std::optional<float>, MAX_SPLIT, MAX_TRAN> _rat;
    TimingData<std::optional<float>, MAX_SPLIT, MAX_TRAN> _cppr_credit;
    TimingData<std::optional<float>, MAX_SPLIT, MAX_TRAN> _constraint;
    TimingData<std::optional<float>, MAX_SPLIT, MAX_TRAN> _related_at;

    void _reset();
    void _fprop_rat(float);
    
    Pin& _constrained_pin();
    Pin& _related_pin();
};



};  // end of namespace ot. -----------------------------------------------------------------------


#endif


