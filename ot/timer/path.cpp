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
    os << std::setfill(' ');
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
std::vector<Path> Timer::worst_paths(size_t K) {
  std::scoped_lock lock(_mutex);
  return _worst_paths(_worst_endpoints(K), K);
}

// Function: worst_paths
std::vector<Path> Timer::worst_paths(Split el, size_t K) {
  std::scoped_lock lock(_mutex);
  return _worst_paths(_worst_endpoints(el, K), K);
}

// Function: worst_paths
std::vector<Path> Timer::worst_paths(Tran rf, size_t K) {
  std::scoped_lock lock(_mutex);
  return _worst_paths(_worst_endpoints(rf, K), K);
}

// Function: worst_paths
std::vector<Path> Timer::worst_paths(Split el, Tran rf, size_t K) {
  std::scoped_lock lock(_mutex);
  return _worst_paths(_worst_endpoints(el, rf, K), K);
}

// Function: _worst_paths
// Report the top-k worst_paths
std::vector<Path> Timer::_worst_paths(const std::vector<Endpoint>& wepts, size_t K) {
  
  // No need to report anything.
  if(K == 0 || wepts.empty()) {
    return {};
  }
  else { 
    std::vector<Path> paths;

    // No need to generate prefix tree
    if(K == 1) {
      paths.push_back(_worst_path(wepts[0]));
    }
    else {
      OT_LOGW("unsupported yet");
    }
    return paths;
  }
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

// Function: _worst_path
Path Timer::_worst_path(const SfxtCache& sfxt) const {

  assert(sfxt.slack());

  Path path;

  // extract the prefix
  _recover_prefix(path, sfxt, *sfxt.__tree[sfxt._S]);

  // extract the suffix
  _recover_suffix(path, sfxt, *sfxt.__tree[sfxt._S]);

  return path;
}

// Function: _worst_path
Path Timer::_worst_path(const Endpoint& ept) const {
  //OT_LOGD("edpt: ", ept.is_test(), " ", ept.is_po(), " ", ept.slack());
  auto sfxt = _sfxt_cache(ept);
  //OT_LOGD("sfxt: ", *sfxt.slack());
  return _worst_path(sfxt);
}


};  // end of namespace ot. -----------------------------------------------------------------------






