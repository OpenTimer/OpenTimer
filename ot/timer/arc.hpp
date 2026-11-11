#ifndef OT_TIMER_ARC_HPP_
#define OT_TIMER_ARC_HPP_

#include <ot/liberty/celllib.hpp>
#include <ot/timer/arc_list.hpp>
#include <ot/traits.hpp>

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

    std::optional<float> delay(Split split, Tran from, Tran to) const {
      return (_delay_valid & (1U << (4 * split + 2 * from + to)))
                 ? std::optional<float>(_delay_values[split][from][to])
                 : std::nullopt;
    }

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
    std::uint8_t _case_mask {15};
    std::uint8_t _delay_valid{0};

    std::variant<Net*, TimingView> _handle;

    std::optional<std::list<Arc>::iterator> _satellite;
    detail::Arc_link _fanout_link;
    detail::Arc_link _fanin_link;

    // Keep engagement independent of all float representations. Effective loads
    // are consumed while propagating each arc, rather than retained per arc.
    TimingData<float, MAX_SPLIT, MAX_TRAN, MAX_TRAN> _delay_values{};

    void _set_delay(Split split, Tran from, Tran to,
                    std::optional<float> value) {
      const auto bit = 1U << (4 * split + 2 * from + to);
      if (value) {
        _delay_values[split][from][to] = *value;
        _delay_valid |= bit;
      } else {
        _delay_valid &= ~bit;
      }
    }

    void _remap_timing(Split, const Timing&);
    void _fprop_slew();
    void _fprop_at();
    void _reset_delay();
    void _fprop_delay(bool delay_ready);
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






