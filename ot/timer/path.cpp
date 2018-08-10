#include <ot/timer/path.hpp>
#include <ot/timer/pin.hpp>
#include <ot/timer/timer.hpp>

namespace ot {

// Constructor
Point::Point(const std::string& name, Tran rf, float in_at) :
  pin   {name},
  tran  {rf},
  at    {in_at} {
}

// ------------------------------------------------------------------------------------------------

// Constructor
Path::Path(Split el, float slk) :
  split {el},
  slack {slk} {
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
  if(!path.split) os << "n/a"; else os << to_string(*path.split);
  os << '\n';

  os << "Required Time:   ";
  if(path.empty() || !path.split || !path.slack) {
    os << "n/a"; 
  } 
  else {
    os << (*path.split == EARLY ? path.back().at - *path.slack :
                                  path.back().at + *path.slack );
  }
  os << '\n';

  os << "Arrival Time :   ";
  if(path.empty()) os << "n/a"; else os << path.back().at;
  os << '\n';

  os << "Slack        :   ";
  if(!path.slack) os << "n/a"; else os << *path.slack;
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

// Functoin: _extract
// Extract the path in ascending order.
std::vector<Path> PathHeap::_extract() {
  std::sort_heap(_paths.begin(), _paths.end(), _comp);
  std::vector<Path> P;
  P.reserve(_paths.size());
  std::transform(_paths.begin(), _paths.end(), std::back_inserter(P), [] (auto& ptr) {
    return std::move(*ptr);
  });
  return P;
}

// Procedure: _insert
void PathHeap::_insert(std::unique_ptr<Path> path) {
  _paths.push_back(std::move(path));
  std::push_heap(_paths.begin(), _paths.end(), _comp);
}

// Procedure: _pop
void PathHeap::_pop() {
  if(_paths.empty()) {
    return;
  }
  std::pop_heap(_paths.begin(), _paths.end(), _comp);
  _paths.pop_back();
}

// Function: _top
Path* PathHeap::_top() const {
  return _paths.empty() ? nullptr : _paths.front().get();
}

// ------------------------------------------------------------------------------------------------

// Function: worst_paths 
// Report the top-k worst_paths
std::vector<Path> Timer::worst_paths(size_t K) {
  std::scoped_lock lock(_mutex);
  return _worst_paths(_worst_endpoints(K), K);
}

// Function: worst_paths
std::vector<Path> Timer::worst_paths(size_t K, Split el) {
  std::scoped_lock lock(_mutex);
  return _worst_paths(_worst_endpoints(K, el), K);
}

// Function: worst_paths
std::vector<Path> Timer::worst_paths(size_t K, Tran rf) {
  std::scoped_lock lock(_mutex);
  return _worst_paths(_worst_endpoints(K, rf), K);
}

// Function: worst_paths
std::vector<Path> Timer::worst_paths(size_t K, Split el, Tran rf) {
  std::scoped_lock lock(_mutex);
  return _worst_paths(_worst_endpoints(K, el, rf), K);
}

// Function: _worst_paths
// Report the top-k worst_paths
std::vector<Path> Timer::_worst_paths(std::vector<Endpoint*>&& epts, size_t K) {

  assert(epts.size() <= K);
  
  // No need to report anything.
  if(K == 0 || epts.empty()) {
    return {};
  }

  std::vector<Path> paths;

  // No need to generate prefix tree
  if(K == 1) {
    auto& path = paths.emplace_back(epts[0]->split(), epts[0]->slack());
    auto sfxt = _sfxt_cache(*epts[0]);
    assert(std::fabs(*sfxt.slack() - *path.slack) < 1e-3);
    _recover_prefix(path, sfxt, *sfxt.__tree[sfxt._S]);
    _recover_suffix(path, sfxt, *sfxt.__tree[sfxt._S]);
  }
  // Generate the prefix tree
  else {
    OT_LOGF("unsupported yet");

    //auto heap = std::make_shared<UniqueGuard<PathHeap>>();

    //auto [task, fu] = _taskflow.emplace([heap] () {
    //  return heap->get()->_extract(); 
    //});

    //for(size_t i=0; i<epts.size(); ++i) {
    //  _taskflow.silent_emplace([heap=heap.get(), ept=epts[i], K] () {
    //  }).precede(task);
    //}
    //
    //future = std::move(fu);
    _taskflow.wait_for_all();
  }
    


  return paths;
}

// Procedure: _recover_prefix
void Timer::_recover_prefix(Path& path, const SfxtCache& sfxt, size_t idx) const {
  
  auto el = sfxt._el;
  auto [v, rf] = _decode_pin(idx);

  assert(v->_at[el][rf]);
  
  path.emplace_front(v->_name, rf, *v->_at[el][rf]);

  if(auto arc = v->_at[el][rf]->pi_arc; arc) {
    _recover_prefix(path, sfxt, _encode_pin(arc->_from, v->_at[el][rf]->pi_rf));
  }
}

// Procedure: _recover_suffix
void Timer::_recover_suffix(Path& path, const SfxtCache& sfxt, size_t u) const {
  
  assert(!path.empty());
  
  if(!sfxt.__link[u]) return;

  auto a = _idx2arc[*sfxt.__link[u]];
  auto v = *sfxt.__tree[u];
  auto [pin, rf] = _decode_pin(v);
  auto at = path.back().at + *a->_delay[sfxt._el][path.back().tran][rf]; 

  path.emplace_back(pin->_name, rf, at);

  _recover_suffix(path, sfxt, v);
}


};  // end of namespace ot. -----------------------------------------------------------------------






