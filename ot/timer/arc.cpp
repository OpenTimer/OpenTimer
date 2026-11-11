#include <ot/timer/arc.hpp>
#include <ot/timer/pin.hpp>
#include <ot/timer/net.hpp>

namespace ot {

// Constructor
Arc::Arc(Pin& from, Pin& to, Net& net) :
  _from   {from},
  _to     {to},
  _handle {&net} {
}

// Constructor
Arc::Arc(Pin& from, Pin& to, TimingView t) : 
  _from   {from},
  _to     {to},
  _handle {t} {
}

// Function: name
std::string Arc::name() const {
  return _from.name() + "->" + _to.name();
}

// Function: is_self_loop
bool Arc::is_self_loop() const {
  return &_from == &_to;
}

// Function: is_loop_breaker
bool Arc::is_loop_breaker() const {
  return _has_state(LOOP_BREAKER);
}

// Function: is_net_arc
bool Arc::is_net_arc() const {
  return std::get_if<Net*>(&_handle) != nullptr;
}

// Function: is_cell_arc
bool Arc::is_cell_arc() const {
  return std::get_if<TimingView>(&_handle) != nullptr;
}

// Function: is_tseg
bool Arc::is_tseg() const {
  if(auto ptr = std::get_if<TimingView>(&_handle); ptr) {
    return (*ptr)[MIN]->is_constraint();
  }
  else return false;
}

// Function: is_pseg
bool Arc::is_pseg() const {
  if(auto ptr = std::get_if<TimingView>(&_handle); ptr) {
    return !(*ptr)[MIN]->is_constraint();
  }
  else return false;
}

// Function: timing_view
TimingView Arc::timing_view() const {
  if(auto tv = std::get_if<TimingView>(&_handle); tv) {
    return *tv; 
  }
  else return {nullptr, nullptr};
}

// Procedure: _remap_timing
void Arc::_remap_timing(Split el, const Timing& timing) {
  (std::get<TimingView>(_handle))[el] = &timing;
}

// Procedure: _reset_delay
void Arc::_reset_delay() { _delay_valid = 0; }

// Procedure: _fprop_slew
void Arc::_fprop_slew() {

  if(_has_state(LOOP_BREAKER) || _case_mask == 0) {
    return;
  }

  std::visit(Functors{
    // Case 1: Net arc
    [this] (Net* net) {
      FOR_EACH_EL_RF(el, rf) {
        // Delay and slew come from the same waveform at this propagation
        // boundary. Preserve the zero-slew delay fallback when no source
        // transition is available, without inventing a propagated slew.
        const float slew = _from._slew[el][rf] ? float(*_from._slew[el][rf]) : 0.0f;
        if(auto response = net->_response(el, rf, slew, _to)) {
          _set_delay(el, rf, rf, response->delay);
          if(_from._slew[el][rf]) {
            _to._relax_slew(this, el, rf, el, rf, response->slew);
          }
        }
      }
    },
    // Case 2: Cell arc
    [this] (TimingView tv) {
      FOR_EACH_EL_RF_RF_IF(el, frf, trf, (tv[el] && _from._slew[el][frf] && (_case_mask & (1U << (2 * frf + trf))))) {
        auto lc = (_to._net) ? _to._net->_load(el, trf) : 0.0f;
        const auto model = _to._net ? _to._net->_gate_response(*tv[el], el, frf, trf, *_from._slew[el][frf])
                                    : std::nullopt;
        const auto so = model ? std::optional<float>(model->slew)
                              : tv[el]->slew(frf, trf, *_from._slew[el][frf], lc);
        // Delay uses this arc's effective load, independent of which fanin
        // wins the output slew. Consume it before discarding the local fit.
        _set_delay(el, frf, trf,
                   tv[el]->delay(frf, trf, *_from._slew[el][frf],
                                 model ? model->capacitance : lc));
        if(so) {
          _to._relax_slew(this, el, frf, el, trf, *so);
          if (_to._net && _to._slew[el][trf] && _to._slew[el][trf]->pi_arc == this
              && _to._slew[el][trf]->pi_rf == frf) {
            _to._net->_driver[el][trf] = model ? std::optional<RcDriver>(model->driver) : std::nullopt;
            _to._net->_effective_cap[el][trf] = model ? std::optional<float>(model->capacitance) : std::nullopt;
            _to._net->_elmore_fallback[el][trf] = !model && _to._net->rct();
          }
        }
      }
    }
  }, _handle);
}

// Procedure: _fprop_delay
void Arc::_fprop_delay(bool delay_ready) {

  if (delay_ready || _has_state(LOOP_BREAKER) || _case_mask == 0) {
    return;
  }

  std::visit(
      Functors{// Case 1: Net arc
               [this](Net *net) {
                 FOR_EACH_EL_RF(el, rf) {
                   _set_delay(el, rf, rf, std::nullopt);
                   const float slew =
                       _from._slew[el][rf] ? float(*_from._slew[el][rf]) : 0.0f;
                   if (auto response = net->_response(el, rf, slew, _to)) {
                     _set_delay(el, rf, rf, response->delay);
                   }
                 }
               },
               // Case 2: Cell arc
               [this](TimingView tv) {
                 FOR_EACH_EL_RF_RF_IF(
                     el, frf, trf,
                     (tv[el] && _from._slew[el][frf] &&
                      (_case_mask & (1U << (2 * frf + trf))))) {
                   // Ideal-clock pins bypass slew fitting. Only transition
                   // validity is needed here: Timer masks each present
                   // ideal-edge delay to zero.
                   const float lc = 0;
                   auto si = *_from._slew[el][frf];
                   auto delay = tv[el]->delay(frf, trf, si, lc);
                   _set_delay(el, frf, trf, delay);
                 }
               }},
      _handle);
}

// Procedure: _fprop_at
void Arc::_fprop_at() {
  
  if(_has_state(LOOP_BREAKER) || _case_mask == 0) {
    return;
  }

  FOR_EACH_EL_RF_RF_IF(el, frf, trf,
                       _from._at[el][frf] && delay(el, frf, trf)) {
    _to._relax_at(this, el, frf, el, trf,
                  *delay(el, frf, trf) + *_from._at[el][frf]);
  }
}

// Procedure: _bprop_rat
void Arc::_bprop_rat() {
  
  if(_has_state(LOOP_BREAKER) || _case_mask == 0) {
    return;
  }

  std::visit(
      Functors{
          // Case 1: Net arc
          [this](Net *net) {
            FOR_EACH_EL_RF_IF(el, rf, _to._rat[el][rf] && delay(el, rf, rf)) {
              _from._relax_rat(this, el, rf, el, rf,
                               *_to._rat[el][rf] - *delay(el, rf, rf));
            }
          },
          // Case 2: Cell arc
          [this](TimingView tv) {
            FOR_EACH_EL_RF_RF_IF(
                el, frf, trf,
                (tv[el] && (_case_mask & (1U << (2 * frf + trf))))) {

              // propagation arc
              if (!tv[el]->is_constraint()) {
                if (!_to._rat[el][trf] || !delay(el, frf, trf)) {
                  continue;
                }
                _from._relax_rat(this, el, frf, el, trf,
                                 *_to._rat[el][trf] - *delay(el, frf, trf));
              }
              // constraint arc
              else {

                if (!tv[el]->is_transition_defined(frf, trf)) {
                  continue;
                }

                if (el == MIN) {
                  auto at = _from._at[MAX][frf];
                  auto slack = _to.slack(MIN, trf);
                  if (at && slack) {
                    _from._relax_rat(this, MAX, frf, MIN, trf, *at + *slack);
                  }
                } else {
                  auto at = _from._at[MIN][frf];
                  auto slack = _to.slack(MAX, trf);
                  if (at && slack) {
                    _from._relax_rat(this, MIN, frf, MAX, trf, *at - *slack);
                  }
                }
              }
            }
          }},
      _handle);
}

// Procedure: _remove_state
void Arc::_remove_state(int s) {
  if(s == 0) _state = 0;
  else {
    _state &= ~s;
  }
}

// Procedure: _insert_state
void Arc::_insert_state(int s) {
  _state |= s;
}

// Function: _has_state
bool Arc::_has_state(int s) const {
  return _state & s;
}


};  // end of namespace ot. -----------------------------------------------------------------------





