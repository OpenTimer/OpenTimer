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

// Function: slack
std::optional<float> Test::slack(Split el, Tran rf) const {
  if(_arc._to._at[el][rf] && _rat[el][rf]) {
    return (
      el == EARLY ? *_arc._to._at[el][rf] - *_rat[el][rf] : 
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
      el == EARLY ? *_arc._to._at[el][rf] - *_rat[el][rf] : 
                    *_rat[el][rf] - *_arc._to._at[el][rf]
    );
  }
  else return std::nullopt;
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
  }
}

// Procedure: _fprop_rat
void Test::_fprop_rat(float period) {

  auto tv = _arc.timing_view();

  // Clear rat and cppr
  FOR_EACH_EL_RF_IF(el, rf, tv[el]) {

    // SLEW not defined at the constrained pin.
    if(!(_arc._to._slew[el][rf])) {
      continue;
    }

    auto fel = (el == EARLY ? LATE : EARLY);
    auto rrf = tv[el]->is_rising_edge_triggered() ? RISE : FALL;
    
    // AT/SLEW not defined at the arc._from
    if(!(_arc._from._at[fel][rrf]) || !(_arc._from._slew[fel][rrf])) {
      continue;
    }
    
    float from_at = *(_arc._from._at[fel][rrf]);
    float from_slew = *(_arc._from._slew[fel][rrf]);
    float to_slew = *(_arc._to._slew[el][rf]);

    if(_constraint[el][rf] = tv[el]->constraint(rrf, rf, from_slew, to_slew)) {
      if(el == EARLY) {
        _rat[el][rf] = *_constraint[el][rf] + from_at;
      }
      else {
        _rat[el][rf] = from_at + period - *_constraint[el][rf];
      }
    }
  }
}

};  // end of namespace ot. -----------------------------------------------------------------------










