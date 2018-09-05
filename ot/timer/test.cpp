#include <ot/timer/test.hpp>
#include <ot/timer/arc.hpp>
#include <ot/timer/pin.hpp>

namespace ot {

// ------------------------------------------------------------------------------------------------

// Constructor
Test::Test(Arc& arc) : _arc {arc} {
}

// Function: rat
std::optional<float> Test::rat(Split el, Tran rf) const {
  return _rat[el][rf];
}

// Function: constraint
std::optional<float> Test::constraint(Split el, Tran rf) const {
  return _constraint[el][rf];
}

// Function: cppr_credit
std::optional<float> Test::cppr_credit(Split el, Tran rf) const {
  return _cppr_credit[el][rf];
}

// Function: slack
std::optional<float> Test::slack(Split el, Tran rf) const {
  if(_arc._to._at[el][rf] && _rat[el][rf]) {
    return (
      el == MIN ? *_arc._to._at[el][rf] - *_rat[el][rf] : 
                  *_rat[el][rf] - *_arc._to._at[el][rf]
    ) + (
      _cppr_credit[el][rf] ? *_cppr_credit[el][rf] : 0.0f
    );
  }
  else return std::nullopt;
}

// Function: raw_slack
std::optional<float> Test::raw_slack(Split el, Tran rf) const {
  if(_arc._to._at[el][rf] && _rat[el][rf]) {
    return (
      el == MIN ? *_arc._to._at[el][rf] - *_rat[el][rf] : 
                  *_rat[el][rf] - *_arc._to._at[el][rf]
    );
  }
  else return std::nullopt;
}

// Function: arc
const Arc& Test::arc() const {
  return _arc;
}

// Function: constrained_pin
const Pin& Test::constrained_pin() const {
  return _arc._to;
}

// Function: related_pin
const Pin& Test::related_pin() const {
  return _arc._from;
}

// Function: _constrained_pin
Pin& Test::_constrained_pin() {
  return _arc._to;
}

// Function: _related_pin
Pin& Test::_related_pin() {
  return _arc._from;
}

// Procedure: _reset
void Test::_reset() {
  FOR_EACH_EL_RF(el, rf) {
    _rat[el][rf].reset();
    _cppr_credit[el][rf].reset();
    _constraint[el][rf].reset();
    _related_at[el][rf].reset();
  }
}

// Procedure: _fprop_rat
void Test::_fprop_rat(float period) {

  auto tv = _arc.timing_view();

  FOR_EACH_EL_RF_IF(el, rf, tv[el]) {

    // SLEW not defined at the constrained pin.
    if(!(_arc._to._slew[el][rf])) {
      continue;
    }

    auto fel = (el == MIN ? MAX : MIN);
    auto frf = tv[el]->is_rising_edge_triggered() ? RISE : FALL;
    
    if(frf == FALL && !tv[el]->is_falling_edge_triggered()) {
      OT_LOGE("clock transition not found for test ", _arc.name());
      continue;
    }

    // AT/SLEW not defined at the arc._from
    if(!(_arc._from._at[fel][frf]) || !(_arc._from._slew[fel][frf])) {
      continue;
    }
    
    if(el == MIN) {
      _related_at[el][rf] = *_arc._from._at[fel][frf];
    }
    else {
      _related_at[el][rf] = *_arc._from._at[fel][frf] + period;
    }

    _constraint[el][rf] = tv[el]->constraint(
      frf, 
      rf, 
      *_arc._from._slew[fel][frf],
      *_arc._to._slew[el][rf]
    );
    
    if(_constraint[el][rf] && _related_at[el][rf]) {
      if(el == MIN) {
        _rat[el][rf] = *_constraint[el][rf] + *_related_at[el][rf];
      }
      else {
        _rat[el][rf] = *_related_at[el][rf] - *_constraint[el][rf];
      }
    }
  }
}

};  // end of namespace ot. -----------------------------------------------------------------------










