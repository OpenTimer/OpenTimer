#include <ot/timer/pin.hpp>
#include <ot/timer/arc.hpp>
#include <ot/timer/net.hpp>
#include <ot/timer/test.hpp>

namespace ot {

// Constructor
PrimaryOutput::PrimaryOutput(Pin& pin) : _pin {pin} {
}

// Function: rat
std::optional<float> PrimaryOutput::rat(Split el, Tran rf) const {
  return _rat[el][rf];
}

// Function: slack
std::optional<float> PrimaryOutput::slack(Split el, Tran rf) const {
  if(_pin._at[el][rf] && _rat[el][rf]) {
    return el == MIN ? *_pin._at[el][rf] - *_rat[el][rf] : *_rat[el][rf] - *_pin._at[el][rf];
  }
  else {
    return std::nullopt;
  }
}

// Procedure: _scale_time
void PrimaryOutput::_scale_time(float s) {
  FOR_EACH_EL_RF_IF(el, rf, _rat[el][rf]) {
    _rat[el][rf] = _rat[el][rf].value() * s;
  }
}

// Procedure: _scale_capacitance
void PrimaryOutput::_scale_capacitance(float s) {
  FOR_EACH_EL_RF_IF(el, rf, _load[el][rf]) {
    _load[el][rf] *= s;
  }
}

// ------------------------------------------------------------------------------------------------

// Constructor
PrimaryInput::PrimaryInput(Pin& pin) : _pin {pin} {
}

// Procedure: _scale_time
void PrimaryInput::_scale_time(float s) {
  FOR_EACH_EL_RF(el, rf) {
    if(_slew[el][rf]) {
      _slew[el][rf] = _slew[el][rf].value() * s;
    }
    if(_at[el][rf]) {
      _at[el][rf] = _at[el][rf].value() * s;
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Constructor
Pin::At::At(Arc* a, Split el, Tran rf, float v) : 
  pi_arc {a}, 
  pi_el  {el}, 
  pi_rf  {rf}, 
  numeric  {v} {
}

// ------------------------------------------------------------------------------------------------

// Constructor
Pin::Slew::Slew(Arc* a, Split el, Tran rf, float v) : 
  pi_arc {a}, 
  pi_el  {el}, 
  pi_rf  {rf}, 
  numeric  {v} {
}

// ------------------------------------------------------------------------------------------------

// Constructor
Pin::Rat::Rat(Arc* a, Split el, Tran rf, float v) : 
  pi_arc  {a}, 
  pi_el   {el}, 
  pi_rf   {rf}, 
  numeric {v} {
}

// ------------------------------------------------------------------------------------------------

// Constructor
Pin::Pin(const std::string& name) : _name {name} {
}

// Procedure: _reset_slew
void Pin::_reset_slew() {
  FOR_EACH_EL_RF(el, rf) {
    _slew[el][rf].reset();
  }
}

// Procedure: _reset_at
void Pin::_reset_at() {
  FOR_EACH_EL_RF(el, rf) {
    _at[el][rf].reset();
  }
}

// Procedure: _reset_rat
void Pin::_reset_rat() {
  FOR_EACH_EL_RF(el, rf) {
    _rat[el][rf].reset();
  }
}

// Function: has_self_loop
bool Pin::has_self_loop() const {
  auto& arcs = num_fanins() < num_fanouts() ? _fanin : _fanout;
  return std::find_if(arcs.begin(), arcs.end(), [] (auto arc) {
    return arc->is_self_loop();
  }) != arcs.end();
}

// Function: is_input
bool Pin::is_input() const {
  return std::visit(Functors{
    [] (PrimaryInput*) {
      return true;
    },
    [] (PrimaryOutput*) {
      return false;
    },
    [] (CellpinView cp) {
      return cp[MIN]->direction == CellpinDirection::INPUT;
    }
  }, _handle);
}

// Function: is_output
bool Pin::is_output() const {
  return std::visit(Functors{
    [] (PrimaryInput*) {
      return false;
    },
    [] (PrimaryOutput*) {
      return true;
    },
    [] (CellpinView cp) {
      return cp[MIN]->direction == CellpinDirection::OUTPUT;
    }
  }, _handle);
}

// Function: is_rct_root
// Query if the pin is a rc root of a net.
bool Pin::is_rct_root() const {
  return std::visit(Functors{
    [] (PrimaryInput*) { 
      return true;
    },
    [] (PrimaryOutput*) {
      return false;
    },
    [] (CellpinView cp) {
      return cp[MIN]->direction == CellpinDirection::OUTPUT;
    }
  }, _handle);
}

// Function: is_datapath_source
// Return true if the node is attached to a datapath source.
bool Pin::is_datapath_source() const {

  // No fanin
  if(num_fanins() == 0) {
    return true;
  }
  // Clock cell pin
  else if(auto cp = cellpin(MIN)) {
    return (cp->is_clock && cp->is_clock.value() == true);
  }
  // Primary input
  else if(primary_input()) {
    return true;
  }
  // Otherwise
  else {
    return false;
  }
}

// Procedure: _insert_fanin
// Insert an arc into the fanin list of the pin.
void Pin::_insert_fanin(Arc& arc) {
  assert(&arc._to == this);
  arc._fanin_satellite = _fanin.insert(_fanin.end(), &arc);
}

// Procedure: _insert_fanout
void Pin::_insert_fanout(Arc& arc) {
  assert(&arc._from == this);
  arc._fanout_satellite = _fanout.insert(_fanout.end(), &arc);
}

// Procedure: _remove_fanout
void Pin::_remove_fanout(Arc& arc) {
  assert(arc._fanout_satellite && &arc._from == this);
  _fanout.erase(*(arc._fanout_satellite));
  arc._fanout_satellite.reset();
}

// Procedure: _remove_fanin
void Pin::_remove_fanin(Arc& arc) {
  assert(arc._fanin_satellite && &arc._to == this);
  _fanin.erase(*(arc._fanin_satellite));
  arc._fanin_satellite.reset();
}

// Function: _find_fanin
Arc* Pin::_find_fanin(Pin& from) {
  auto itr = std::find_if(_fanin.begin(), _fanin.end(), [&] (Arc* arc) {
    return &(arc->_from) == &from;
  });
  return itr == _fanin.end() ? nullptr : *itr;
}

// Function: _find_fanout
Arc* Pin::_find_fanout(Pin& to) {
  auto itr = std::find_if(_fanout.begin(), _fanout.end(), [&] (Arc* arc) {
    return &(arc->_to) == &to;
  });
  return itr == _fanout.end() ? nullptr : *itr;
}

// Function: at
std::optional<float> Pin::at(Split el, Tran rf) const {
  return _at[el][rf];
}

// Function: rat
std::optional<float> Pin::rat(Split el, Tran rf) const {
  return _rat[el][rf];
}

// Function: slew
std::optional<float> Pin::slew(Split el, Tran rf) const {
  return _slew[el][rf];
}

// Function: slack
std::optional<float> Pin::slack(Split el, Tran rf) const {
  if(_at[el][rf] && _rat[el][rf]) {
    return el == MIN ? *_at[el][rf] - *_rat[el][rf] : *_rat[el][rf] - *_at[el][rf];
  }
  else return std::nullopt;
}

// Function: _delta_at
std::optional<float> Pin::_delta_at(Split lel, Tran lrf, Split rel, Tran rrf) const {
  if(_at[lel][lrf] && _at[rel][rrf]) {
    return *_at[lel][lrf] - *_at[rel][rrf];
  }
  else return std::nullopt;
}

// Function: _delta_slew
std::optional<float> Pin::_delta_slew(Split lel, Tran lrf, Split rel, Tran rrf) const {
  if(_slew[lel][lrf] && _slew[rel][rrf]) {
    return *_slew[lel][lrf] - *_slew[rel][rrf];
  }
  else return std::nullopt;
}

// Function: _delta_rat
std::optional<float> Pin::_delta_rat(Split lel, Tran lrf, Split rel, Tran rrf) const {
  if(_rat[lel][lrf] && _rat[rel][rrf]) {
    return *_rat[lel][lrf] - *_rat[rel][rrf];
  }
  else return std::nullopt;
}

// Function: cap
// Query the pin capacitance. A pin belongs to three categories: 1) cellpin, 2) primary input,
// and 3) primary output. A cellot_pin.has only one type of capacitance. A primary input has no
// capacitance. The primary output has load capacitance.
float Pin::cap(Split el, Tran rf) const {

  return std::visit(Functors{
    [] (PrimaryInput*) {
      return 0.0f;
    },
    [&] (PrimaryOutput* po) {
      return po->_load[el][rf];
    },
    [&] (CellpinView cp) {
      if(rf == RISE && cp[el]->rise_capacitance) {
        return cp[el]->rise_capacitance.value();
      }
      else if(rf == FALL && cp[el]->fall_capacitance) {
        return cp[el]->fall_capacitance.value();
      }
      else {
        return cp[el]->capacitance ? cp[el]->capacitance.value() : 0.0f;
      }
    }
  }, _handle);
}

// Function: load
// The total capacitive load is defined as the sum of the input capacitance 
// of all the other devices sharing the trace.
// Note that the capacitance of the device driving the trace is not included.
//float Pin::load(Split el, Tran rf) const {
//  
//  // Case 1: The pin is a rctree root.
//  if(is_rct_root()) {
//    return _net ? _net->load(el, rf) : 0.0f;
//  }
//  
//  // Case 2: The pin is not a rctree root.
//  return cap(el, rf);
//}

// Procedure: _relax_slew
// Update the slew of the node
void Pin::_relax_slew(Arc* arc, Split fel, Tran frf, Split tel, Tran trf, float val) {

  switch(tel) {

    case MIN:
      if(!_slew[tel][trf] || val < *_slew[tel][trf]) {
        _slew[tel][trf].emplace(arc, fel, frf, val);
      }
    break;

    case MAX:
      if(!_slew[tel][trf] || val > *_slew[tel][trf]) {
        _slew[tel][trf].emplace(arc, fel, frf, val);
      }
    break;
  };
}

// Procedure: _relax_at
// Update the arrival time of the node from a given fanin node.
void Pin::_relax_at(Arc* arc, Split fel, Tran frf, Split tel, Tran trf, float val) {
  
  switch (tel) {
    case MIN:
      if(!_at[tel][trf] || val < *_at[tel][trf]) {
        _at[tel][trf].emplace(arc, fel, frf, val);
      }
    break;
    case MAX:
      if(!_at[tel][trf] || val > *_at[tel][trf]) {
        _at[tel][trf].emplace(arc, fel, frf, val);
      }
    break;
  }
}

// Procedure: _relax_rat
// Update the arrival time of the node
void Pin::_relax_rat(Arc* arc, Split fel, Tran frf, Split tel, Tran trf, float val) {

  switch(fel) {

    case MIN:
      if(!_rat[fel][frf] || val > *_rat[fel][frf]) {
        _rat[fel][frf].emplace(arc, tel, trf, val);
      }
    break;

    case MAX:
      if(!_rat[fel][frf] || val < *_rat[fel][frf]) {
        _rat[fel][frf].emplace(arc, tel, trf, val);
      }
    break;
  };
}

// Procedure: _remap_cellpin
void Pin::_remap_cellpin(Split el, const Cellpin* cpin) {

  (std::get<CellpinView>(_handle))[el] = cpin;

  if(_net) {
    _net->_rc_timing_updated = false;
  }
}

// Procedure: _remap_cellpin
void Pin::_remap_cellpin(Split el, const Cellpin& cpin) {
  
  (std::get<CellpinView>(_handle))[el] = &cpin;

  if(_net) {
    _net->_rc_timing_updated = false;
  }
}

// Procedure: _remove_state
void Pin::_remove_state(int s) {
  if(s == 0) _state = 0;
  else {
    _state &= ~s;
  }
}

// Procedure: _insert_state
void Pin::_insert_state(int s) {
  _state |= s;
}

// Function: _has_state
bool Pin::_has_state(int s) const {
  return _state & s;
}

// Function: _has_no_state
bool Pin::_has_no_state(int s) const {
  return !_has_state(s); 
}

};  // end of namespace ot. -----------------------------------------------------------------------








