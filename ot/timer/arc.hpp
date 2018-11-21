#ifndef OT_TIMER_ARC_HPP_
#define OT_TIMER_ARC_HPP_

#include <ot/traits.hpp>
#include <ot/liberty/celllib.hpp>

namespace ot {

class Pin;
class Net;
class Test;

// ------------------------------------------------------------------------------------------------

// Class: Arc
class Arc {

  friend class Timer;
  friend class Pin;
  friend class Gate;
  friend class Test;
  friend class SCC;
  
  constexpr static int LOOP_BREAKER = 0x01;

  public:

    Arc(Pin&, Pin&, Net&);
    Arc(Pin&, Pin&, TimingView);

    std::string name() const;

    bool is_cell_arc() const;
    bool is_net_arc() const;
    bool is_pseg() const;
    bool is_tseg() const;
    bool is_self_loop() const;
    bool is_loop_breaker() const;

    size_t idx() const;

    TimingView timing_view() const;

    const Pin& from() const;
    const Pin& to() const;

  private:

    Pin& _from;
    Pin& _to; 

    size_t _idx;

    int _state {0};

    std::variant<Net*, TimingView> _handle;

    std::optional<std::list<Arc>::iterator> _satellite;
    std::optional<std::list<Arc*>::iterator> _fanout_satellite;
    std::optional<std::list<Arc*>::iterator> _fanin_satellite;
    
    TimingData<std::optional<float>, MAX_SPLIT, MAX_TRAN, MAX_TRAN> _delay;

    void _remap_timing(Split, const Timing&);
    void _fprop_slew();
    void _fprop_at();
    void _reset_delay();
    void _fprop_delay();
    void _bprop_rat();
    void _insert_state(int);
    void _remove_state(int = 0);

    bool _has_state(int) const;
}; 

// Function: idx
inline size_t Arc::idx() const {
  return _idx;
}

// Function: from
inline const Pin& Arc::from() const {
  return _from;
}

// Function: to
inline const Pin& Arc::to() const {
  return _to;
}

};  // end of namespace ot. -----------------------------------------------------------------------

#endif






