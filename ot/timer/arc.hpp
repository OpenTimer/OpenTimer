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

  public:

    Arc(Pin&, Pin&, Net&);
    Arc(Pin&, Pin&, SplitView<Timing>);

    bool is_cell_arc() const;
    bool is_net_arc() const;
    bool is_pseg() const;
    bool is_tseg() const;

    size_t idx() const;

  private:

    Pin& _from;
    Pin& _to; 

    size_t _idx;

    std::variant<Net*, SplitView<Timing>> _handle;

    std::optional<std::list<Arc>::iterator> _satellite;
    std::optional<std::list<Arc*>::iterator> _fanout_satellite;
    std::optional<std::list<Arc*>::iterator> _fanin_satellite;
    
    std::array<std::array<std::array<std::optional<float>, MAX_TRAN>, MAX_TRAN>, MAX_SPLIT> _delay;

    void _remap_timing(Split, const Timing&);
    void _fprop_slew();
    void _fprop_at();
    void _reset_delay();
    void _fprop_delay();
    void _bprop_rat();

    SplitView<Timing> _timing();
}; 

// Function: idx
inline size_t Arc::idx() const {
  return _idx;
}


};  // end of namespace ot. -----------------------------------------------------------------------

#endif






