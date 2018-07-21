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
Arc::Arc(Pin& from, Pin& to, SplitView<Timing> t) : 
  _from   {from},
  _to     {to},
  _handle {t} {
}

// Function: is_net_arc
bool Arc::is_net_arc() const {
  return std::get_if<Net*>(&_handle) != nullptr;
}

// Function: is_cell_arc
bool Arc::is_cell_arc() const {
  return std::get_if<SplitView<Timing>>(&_handle) != nullptr;
}

// Function: is_tseg
bool Arc::is_tseg() const {
  if(auto ptr = std::get_if<SplitView<Timing>>(&_handle); ptr) {
    return ptr->get(EARLY).is_constraint();
  }
  else return false;
}

// Function: is_pseg
bool Arc::is_pseg() const {
  if(auto ptr = std::get_if<SplitView<Timing>>(&_handle); ptr) {
    return !(ptr->get(EARLY).is_constraint());
  }
  else return false;
}

// Function: _timing
SplitView<Timing> Arc::_timing() {
  return std::get<SplitView<Timing>>(_handle);
}

// Procedure: _remap_timing
void Arc::_remap_timing(Split el, const Timing& timing) {
  std::get<SplitView<Timing>>(_handle).set(el, timing);
}

// Procedure: _reset_delay
void Arc::_reset_delay() {
  FOR_EACH_EL_RF_RF(el, frf, trf) {
    _delay[el][frf][trf].reset();
  }
}

// Procedure: _fprop_slew
void Arc::_fprop_slew() {

  std::visit(Functors{
    // Case 1: Net arc
    [this] (Net* net) {
      FOR_EACH_EL_RF(el, rf) {
        if(_from._slew[el][rf]) {
          if(auto so = net->_slew(el, rf, *(_from._slew[el][rf]), _to); so) {
            _to._relax_slew(this, el, rf, el, rf, *so);
          }
        }
      }
    },
    // Case 2: Cell arc
    [this] (SplitView<Timing> timing) {
      FOR_EACH_EL_RF_RF_IF(el, frf, trf, _from._slew[el][frf]) {
        auto lc = (_to._net) ? _to._net->_load(el, trf) : 0.0f;
        if(auto so = timing.get(el).slew(frf, trf, *_from._slew[el][frf], lc); so) {
          _to._relax_slew(this, el, frf, el, trf, *so);
        }
      }
    }
  }, _handle);
}

// Procedure: _fprop_delay
void Arc::_fprop_delay() {

  std::visit(Functors{
    // Case 1: Net arc
    [this] (Net* net) {
      FOR_EACH_EL_RF(el, rf) {
        _delay[el][rf][rf] = net->_delay(el, rf, _to);
      }
    },
    // Case 2: Cell arc
    [this] (SplitView<Timing> timing) {
      FOR_EACH_EL_RF_RF_IF(el, frf, trf, _from._slew[el][frf]) {
        auto lc = (_to._net) ? _to._net->_load(el, trf) : 0.0f;
        auto si = *_from._slew[el][frf];
        _delay[el][frf][trf] = timing.get(el).delay(frf, trf, si, lc);
      }
    }
  }, _handle);
}

// Procedure: _fprop_at
void Arc::_fprop_at() {
  FOR_EACH_EL_RF_RF_IF(el, frf, trf, _from._at[el][frf] && _delay[el][frf][trf]) {
    _to._relax_at(this, el, frf, el, trf, *_delay[el][frf][trf] + *_from._at[el][frf]);
  }
}

// Procedure: _bprop_rat
void Arc::_bprop_rat() {

  std::visit(Functors{
    // Case 1: Net arc
    [this] (Net* net) {
      FOR_EACH_EL_RF_IF(el, rf, _to._rat[el][rf] && _delay[el][rf][rf]) {
        _from._relax_rat(this, el, rf, el, rf, *_to._rat[el][rf] - *_delay[el][rf][rf]);
      }
    },
    // Case 2: Cell arc
    [this] (SplitView<Timing> timing) {

      if(!timing.get(EARLY).is_constraint()) {
        FOR_EACH_EL_RF_RF_IF(el, frf, trf, _to._rat[el][trf] && _delay[el][frf][trf]) {
          _from._relax_rat(this, el, frf, el, trf, *_to._rat[el][trf] - *_delay[el][frf][trf]);
        }
      }
      else {

        // For the clock pins of the flip-flop, the required arrival time 
        // is derived from the test slack. 
        // For the early mode, the slack at the clock pin is the setup or late test slack.
        // For the late split, the slack at the clock pin is the hold or early test slack. 
        // From the corresponding test slack and arrival time, the clock required arrival time 
        // can be derived, and appropriately propagated.

        FOR_EACH_EL_RF_RF(el, ck_rf, d_rf) {

          if(!timing.get(el).is_transition_defined(ck_rf, d_rf)) {
            continue;
          }

          if(el == EARLY) {
            auto at = _from._at[LATE][ck_rf];
            auto slack = _to.slack(EARLY, d_rf);
            if(at && slack) {
              _from._relax_rat(this, LATE, ck_rf, EARLY, d_rf, *at + *slack);
            }
          }
          else {
            auto at = _from._at[EARLY][ck_rf];
            auto slack = _to.slack(LATE, d_rf);
            if(at && slack) {
              _from._relax_rat(this, EARLY, ck_rf, LATE, d_rf, *at - *slack);
            }
          }
        }
      }
    }
  }, _handle);
}


};  // end of namespace ot. -----------------------------------------------------------------------





