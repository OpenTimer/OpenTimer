#include <ot/timer/path.hpp>
#include <ot/timer/pin.hpp>
#include <ot/timer/timer.hpp>

namespace ot {

// Constructor
Point::Point(const std::string& name, Split el, Tran rf, float in_at, float in_rat) :
  pin   {name},
  split {el},
  tran  {rf},
  at    {in_at},
  rat   {in_rat} {
}

// ------------------------------------------------------------------------------------------------

// Operator <<
std::ostream& operator << (std::ostream& os, const Path& path) {
  
  // Print the head
  os << "Endpoint     :   ";
  if(path.empty()) os << "n/a"; else os << path.back().pin;
  os << '\n';

  os << "Startpoint   :   ";
  if(path.empty()) os << "n/a"; else os << path.front().pin;
  os << '\n';

  os << "Path type    :   ";
  if(path.empty()) os << "n/a"; else os << to_string(path.back().split);
  os << '\n';

  os << "Required Time:   ";
  if(path.empty()) os << "n/a"; else os << path.back().rat;
  os << '\n';

  os << "Arrival Time :   ";
  if(path.empty()) os << "n/a"; else os << path.back().at;
  os << '\n';

  os << "Slack        :   ";
  if(path.empty()) os << "n/a"; else os << path.back().slack();
  os << '\n';
  
  // Print the body
  if(!path.empty()) {

    // find the maximum pin name
    auto max_plen = std::max_element(path.begin(), path.end(), 
      [] (const Point& lp, const Point& rp) {
        return lp.pin.size() < rp.pin.size();
      }
    )->pin.size();

    os << std::setfill('-') << std::setw(31 + max_plen) << '\n'
       << std::setfill(' ') << std::setw(10) << "Arrival"  
                            << std::setw(12) << "Delay"
                            << std::setw(6)  << "Dir" 
                            << std::setw(2  + max_plen)  << "Pin"   << '\n'
       << std::setfill('-') << std::setw(31 + max_plen) << '\n';
    
    // trace
    os << std::setfill(' ') << std::setprecision(3);
    std::optional<float> pi_at;
    for(const auto& p : path) {
      // arrival time
      os << std::setw(10) << p.at << "  ";
      
      // delay
      os << std::setw(10);
      if(pi_at) os << p.at - *pi_at;
      else os << "n/a";
      os << "  ";

      // transition
      os << std::setw(4) << to_string(p.tran) << "  ";

      // pin name
      os << std::setw(max_plen) << p.pin << '\n';
      
      // cursor
      pi_at = p.at;
    }
    
    os << std::setfill('-') << std::setw(31 + max_plen) << '\n';
  }
  
  return os;
}

// ------------------------------------------------------------------------------------------------

// Function: worst_paths 
// Report the top-k worst_paths
std::future<Paths> Timer::worst_paths(size_t K) {
  std::scoped_lock lock(_mutex);
  return _worst_paths(_worst_endpoints(K), K);
}

// Function: worst_paths
std::future<Paths> Timer::worst_paths(size_t K, Split el) {
  std::scoped_lock lock(_mutex);
  return _worst_paths(_worst_endpoints(K, el), K);
}

// Function: worst_paths
std::future<Paths> Timer::worst_paths(size_t K, Tran rf) {
  std::scoped_lock lock(_mutex);
  return _worst_paths(_worst_endpoints(K, rf), K);
}

// Function: worst_paths
std::future<Paths> Timer::worst_paths(size_t K, Split el, Tran rf) {
  std::scoped_lock lock(_mutex);
  return _worst_paths(_worst_endpoints(K, el, rf), K);
}

// Function: _worst_paths
// Report the top-k worst_paths
std::future<Paths> Timer::_worst_paths(const std::vector<Endpoint>& wepts, size_t K) {

  assert(wepts.size() <= K);
  
  // No need to report anything.
  if(K == 0 || wepts.empty()) {
    return std::async(std::launch::deferred, [] () -> Paths { return {}; });
  }

  std::future<Paths> future;

  // No need to generate prefix tree
  if(K == 1) {
    future = _taskflow.emplace([this, ept=wepts[0]] () -> Paths {
      Paths paths(1);
      auto sfxt = _sfxt_cache(ept);
      _recover_prefix(paths[0], sfxt, *sfxt.__tree[sfxt._S]);
      _recover_suffix(paths[0], sfxt, *sfxt.__tree[sfxt._S]);
      return paths;
    }).second;
  }
  // Generate the prefix tree
  else {
    OT_LOGF("unsupported yet");
  }
    
  _taskflow.silent_dispatch();

  return future;
}

// Procedure: _recover_prefix
void Timer::_recover_prefix(Path& path, const SfxtCache& sfxt, size_t idx) const {
  
  auto el = sfxt._el;
  auto [v, rf] = _decode_pin(idx);

  assert(v->_at[el][rf]);

  if(auto arc = v->_at[el][rf]->pi_arc; arc) {
    _recover_prefix(path, sfxt, _encode_pin(arc->_from, v->_at[el][rf]->pi_rf));
  }
    
  path.emplace_back(
    v->_name, 
    el, 
    rf, 
    *v->_at[el][rf],
    el == EARLY ? *v->_at[el][rf] - *sfxt.slack() : *sfxt.slack() + *v->_at[el][rf]
  );
}

// Procedure: _recover_suffix
void Timer::_recover_suffix(Path& path, const SfxtCache& sfxt, size_t u) const {
  
  assert(!path.empty());
  
  if(!sfxt.__link[u]) return;

  auto a = _idx2arc[*sfxt.__link[u]];
  auto v = *sfxt.__tree[u];
  auto [pin, rf] = _decode_pin(v);
  auto at = path.back().at + *a->_delay[sfxt._el][path.back().tran][rf]; 

  path.emplace_back(
    pin->_name, 
    sfxt._el, 
    rf, 
    at,
    sfxt._el == EARLY ? at - *sfxt.slack() : *sfxt.slack() + at
  );

  _recover_suffix(path, sfxt, v);
}


};  // end of namespace ot. -----------------------------------------------------------------------






