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
  return _from._name + "->" + _to._name;
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
void Arc::_reset_delay() {
  FOR_EACH_EL_RF_RF(el, frf, trf) {
    _delay[el][frf][trf].reset();
  }
}

// Procedure: _fprop_slew
void Arc::_fprop_slew() {

  if(_has_state(LOOP_BREAKER)) {
    return;
  }

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
    [this] (TimingView tv) {
      FOR_EACH_EL_RF_RF_IF(el, frf, trf, (tv[el] && _from._slew[el][frf])) {
        auto lc = (_to._net) ? _to._net->_load(el, trf) : 0.0f;
        if(auto so = tv[el]->slew(frf, trf, *_from._slew[el][frf], lc); so) {
          _to._relax_slew(this, el, frf, el, trf, *so);
        }
      }
    }
  }, _handle);
}

// Procedure: _fprop_delay
void Arc::_fprop_delay() {
  
  if(_has_state(LOOP_BREAKER)) {
    return;
  }

  std::visit(Functors{
    // Case 1: Net arc
    [this] (Net* net) {
      FOR_EACH_EL_RF(el, rf) {
        _delay[el][rf][rf] = net->_delay(el, rf, _to);
      }
    },
    // Case 2: Cell arc
    [this] (TimingView tv) {
      FOR_EACH_EL_RF_RF_IF(el, frf, trf, (tv[el] && _from._slew[el][frf])) {
        auto lc = (_to._net) ? _to._net->_load(el, trf) : 0.0f;
        auto si = *_from._slew[el][frf];
        _delay[el][frf][trf] = tv[el]->delay(frf, trf, si, lc);
      }
    }
  }, _handle);
}

// Procedure: _fprop_at
void Arc::_fprop_at() {
  
  if(_has_state(LOOP_BREAKER)) {
    return;
  }

  FOR_EACH_EL_RF_RF_IF(el, frf, trf, _from._at[el][frf] && _delay[el][frf][trf]) {
    _to._relax_at(this, el, frf, el, trf, *_delay[el][frf][trf] + *_from._at[el][frf]);
  }
}

// Procedure: _bprop_rat
void Arc::_bprop_rat() {
  
  if(_has_state(LOOP_BREAKER)) {
    return;
  }

  std::visit(Functors{
    // Case 1: Net arc
    [this] (Net* net) {
      FOR_EACH_EL_RF_IF(el, rf, _to._rat[el][rf] && _delay[el][rf][rf]) {
        _from._relax_rat(this, el, rf, el, rf, *_to._rat[el][rf] - *_delay[el][rf][rf]);
      }
    },
    // Case 2: Cell arc
    [this] (TimingView tv) {

      FOR_EACH_EL_RF_RF_IF(el, frf, trf, tv[el]) {
        
        // propagation arc
        if(!tv[el]->is_constraint()) {
          if(!_to._rat[el][trf] || !_delay[el][frf][trf]) {
            continue;
          }
          _from._relax_rat(this, el, frf, el, trf, *_to._rat[el][trf] - *_delay[el][frf][trf]);
        }
        // constraint arc
        else {
          
          if(!tv[el]->is_transition_defined(frf, trf)) {
            continue;
          }

          if(el == MIN) {
            auto at = _from._at[MAX][frf];
            auto slack = _to.slack(MIN, trf);
            if(at && slack) {
              _from._relax_rat(this, MAX, frf, MIN, trf, *at + *slack);
            }
          }
          else {
            auto at = _from._at[MIN][frf];
            auto slack = _to.slack(MAX, trf);
            if(at && slack) {
              _from._relax_rat(this, MIN, frf, MAX, trf, *at - *slack);
            }
          }
        }
      }
    }
  }, _handle);
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





