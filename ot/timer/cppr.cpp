#include <ot/timer/cppr.hpp>
#include <ot/timer/timer.hpp>

namespace ot {

// Constructor
CpprCache::CpprCache(size_t N) {
  resize_to_fit(N, __capp);
  // debug
  //for(const auto& i : __capp) assert(!i);
}

// Move constructor
CpprCache::CpprCache(CpprCache&& rhs) : 
  _capb {rhs._capb},
  _cape {rhs._cape},
  _pins {std::move(rhs._pins)} {
}

// Destructor
CpprCache::~CpprCache() {
  for(auto p : _pins) {
    __capp[p].reset();
  }
}

// ------------------------------------------------------------------------------------------------

// Function: _cppr_cache
// Obtain a CPPR cache for a given test.
CpprCache Timer::_cppr_cache(const Test& test, Split el, Tran rf) const {

  // Create a cppr handle.
  auto cppr = CpprCache(_idx2pin.size() << 1);

  // Find the timing
  auto tv  = test._arc.timing_view();
  assert(tv[el]);

  // Find the capture path
  auto v   = &(test._arc._from);
  auto vel = (el == MIN) ? MAX : MIN;
  auto vrf = tv[el]->is_rising_edge_triggered() ? RISE : FALL;

  cppr._cape = _encode_pin(*v, vrf);

  while(v && v->_at[vel][vrf]) {

    auto vid = _encode_pin(*v, vrf);    
    cppr._pins.insert(vid);

    if(auto arc = v->_at[vel][vrf]->pi_arc; arc) {
      // Cacth the data to local to avoid messing up swap.
      auto u = &(arc->_from);
      auto uel = v->_at[vel][vrf]->pi_el;
      auto urf = v->_at[vel][vrf]->pi_rf;

      // Record the path parent.
      cppr.__capp[vid] = _encode_pin(*u, urf);
      
      // Move the pointer
      vel = uel;
      vrf = urf;
      v   = u;
    }
    else {
      cppr._capb = vid;
      cppr.__capp[vid] = vid;
      break;
    }
  }

  return cppr;
}

// Function: _cppr_credit
std::optional<float> Timer::_cppr_credit(const Test& test, Split el, Tran rf) const {

  assert(_cppr_analysis);

  // Create a suffix tree
  auto sfxt = _sfxt_cache(test, el, rf);

  // compute the cppr credit
  if(sfxt.slack()) {
    auto tat = *test._arc._to._at[el][rf];
    auto rat = (el == MIN) ? tat - *sfxt.slack() : *sfxt.slack() + tat;
    return rat - *test._rat[el][rf];
  }
  else {
    return std::nullopt;
  }
}

// Procedure: _cppr_credit
std::optional<float> Timer::_cppr_credit(const CpprCache& cppr, Pin& pin, Split el, Tran rf) const {

  assert(_cppr_analysis);

  // back-trace to find the common point. 
  auto v = &pin;
  auto vel = el;
  auto vrf = rf;

  while(v && v->_at[vel][vrf]) {

    auto vid = _encode_pin(*v, vrf);    
    
    // Find a converging point.
    if(cppr.__capp[vid]) {
      
      assert(vel == el);

      auto dv = v->_delta_at(MAX, vrf, MIN, vrf);

      // Return the credit for the early (hold) test.  
      if(el == MIN) {
        return dv;
      }
      // Return the credit for the late (setup) test.
      else {
        auto [r, rrf] = _decode_pin(cppr._capb);
        auto dr = r->_delta_at(MAX, rrf, MIN, rrf);
        if(dv && dr) {
          return *dv - *dr;
        }
        else {
          return std::nullopt;
        }
      }
    }
    
    // Go up to the parent.
    if(auto arc = v->_at[vel][vrf]->pi_arc; arc) {
      // Cacahe the local data to avoid swap error.
      auto u = &(arc->_from);
      auto uel = v->_at[vel][vrf]->pi_el;
      auto urf = v->_at[vel][vrf]->pi_rf;
      // Move the pointer
      vel = uel;
      vrf = urf;
      v   = u;
    }
    else break;
  }

  return std::nullopt;
}

// Function: _cppr_offset
std::optional<float> Timer::_cppr_offset(const CpprCache& cppr, Pin& pin, Split el, Tran rf) const {

  assert(_cppr_analysis);

  if(auto at = pin._at[el][rf]; !at) {
    return std::nullopt;
  }
  else {
    if(auto credit = _cppr_credit(cppr, pin, el, rf); credit) {
      return (el == MIN) ? *at + *credit : -(*at) + *credit; 
    }
    else {
      return (el == MIN) ? *at : -(*at);
    }
  }
}


};  // end of namespace ot. -----------------------------------------------------------------------









