#include <ot/timer/timer.hpp>

namespace ot {

// Constructor
Point::Point(const Pin& p, Tran t, float v) :
  pin        {p},
  transition {t},
  at         {v} {
}

// ------------------------------------------------------------------------------------------------

// Constructor
Path::Path(float slk, const Endpoint* ept) :
  slack    {slk},
  endpoint {ept} {
}

// ------------------------------------------------------------------------------------------------

// Operator <<
std::ostream& operator << (std::ostream& os, const Path& path) {

  if(path.empty()) {
    os << "empty path\n";
  }

  auto split = path.endpoint->split();
  
  // Print the head
  os << "Endpoint     :   " << path.back().pin.name()  << '\n';
  os << "Startpoint   :   " << path.front().pin.name() << '\n';
  os << "Analysis type:   " << to_string(split) << '\n';
  os << "Required Time:   " << (split == EARLY ? path.back().at - path.slack :
                                                 path.back().at + path.slack) 
                            << '\n';
  os << "Arrival Time :   " << path.back().at << '\n';
  os << "Slack        :   " << path.slack << '\n';
  
  // find the maximum pin name
  auto max_plen = std::max_element(path.begin(), path.end(), 
    [] (const Point& lp, const Point& rp) {
      return lp.pin.name().size() < rp.pin.name().size();
    }
  )->pin.name().size();

  os << std::setfill('-') << std::setw(31 + max_plen) << '\n'
     << std::setfill(' ') << std::setw(10) << "Arrival"  
                          << std::setw(12) << "Delay"
                          << std::setw(6)  << "Dir" 
                          << std::setw(2  + max_plen)  << "Pin"   << '\n'
     << std::setfill('-') << std::setw(31 + max_plen) << '\n';
  
  // trace
  os << std::setfill(' ') << std::fixed << std::setprecision(3);
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
    os << std::setw(4) << to_string(p.transition) << "  ";

    // pin name
    os << std::setw(max_plen) << p.pin.name() << '\n';
    
    // cursor
    pi_at = p.at;
  }
  
  os << std::setfill('-') << std::setw(31 + max_plen) << '\n';
  
  return os;
}

// ------------------------------------------------------------------------------------------------

// Functoin: _extract
// Extract the path in ascending order.
std::vector<Path> PathHeap::extract() {
  std::sort_heap(_paths.begin(), _paths.end(), _comp);
  std::vector<Path> P;
  P.reserve(_paths.size());
  std::transform(_paths.begin(), _paths.end(), std::back_inserter(P), [] (auto& ptr) {
    return std::move(*ptr);
  });
  _paths.clear();
  return P;
}

// Procedure: push
void PathHeap::push(std::unique_ptr<Path> path) {
  _paths.push_back(std::move(path));
  std::push_heap(_paths.begin(), _paths.end(), _comp);
}

// Procedure: pop
void PathHeap::pop() {
  if(_paths.empty()) {
    return;
  }
  std::pop_heap(_paths.begin(), _paths.end(), _comp);
  _paths.pop_back();
}

// Function: top
Path* PathHeap::top() const {
  return _paths.empty() ? nullptr : _paths.front().get();
}

// Procedure: fit
void PathHeap::fit(size_t K) {
  while(_paths.size() > K) {
    pop();
  }
}

// Procedure: heapify
void PathHeap::heapify() {
  std::make_heap(_paths.begin(), _paths.end(), _comp);   
}

// Procedure: merge_and_fit
void PathHeap::merge_and_fit(PathHeap&& rhs, size_t K) {

  if(_paths.capacity() < rhs._paths.capacity()) {
    _paths.swap(rhs._paths);
  }

  std::sort_heap(_paths.begin(), _paths.end(), _comp);
  std::sort_heap(rhs._paths.begin(), rhs._paths.end(), _comp);

  auto mid = _paths.insert(
    _paths.end(), 
    std::make_move_iterator(rhs._paths.begin()),
    std::make_move_iterator(rhs._paths.end())
  ); 

  rhs._paths.clear();

  std::inplace_merge(_paths.begin(), mid, _paths.end(), _comp);
  
  if(_paths.size() > K) {
    _paths.resize(K);
  }
  
  heapify(); 
}

// Function: dump
std::string PathHeap::dump() const {
  std::ostringstream oss;
  oss << "# Paths: " << _paths.size() << '\n';
  for(size_t i=0; i<_paths.size(); ++i) {
    oss << "slack[" << i << "]: " << _paths[i]->slack << '\n'; 
  }
  return oss.str();
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

  // No need to generate prefix tree
  if(K == 1) {
    std::vector<Path> paths;
    paths.emplace_back(epts[0]->slack(), epts[0]);
    auto sfxt = _sfxt_cache(*epts[0]);
    assert(std::fabs(*sfxt.slack() - paths[0].slack) < 1e-3);
    _recover_datapath(paths[0], sfxt);
    return paths;
  }
  
  // Generate the prefix tree
  PathHeap heap;

  _taskflow.transform_reduce(epts.begin(), epts.end(), heap,
    [&] (PathHeap l, PathHeap r) {
      l.merge_and_fit(std::move(r), K);
      return l;
    },
    [&] (PathHeap heap, Endpoint* ept) {
      _spur(*ept, K, heap);
      return heap;
    },
    [&] (Endpoint* ept) {
      PathHeap heap;
      _spur(*ept, K, heap);
      return heap;
    }
  );

  _taskflow.wait_for_all();

  return heap.extract();
}

// Procedure: _recover_prefix
// Recover the worst path prefix at a given pin.
void Timer::_recover_prefix(Path& path, const SfxtCache& sfxt, size_t idx) const {
  
  auto el = sfxt._el;
  auto [v, rf] = _decode_pin(idx);

  assert(v->_at[el][rf]);
  
  path.emplace_front(*v, rf, *v->_at[el][rf]);

  if(auto arc = v->_at[el][rf]->pi_arc; arc) {
    _recover_prefix(path, sfxt, _encode_pin(arc->_from, v->_at[el][rf]->pi_rf));
  }
}

// Procedure: _recover_datapath
// Recover the worst data path from a given suffix tree.
void Timer::_recover_datapath(Path& path, const SfxtCache& sfxt) const {
  
  if(!sfxt.__tree[sfxt._S]) {
    return;
  }

  auto u = *sfxt.__tree[sfxt._S];
  auto [upin, urf] = _decode_pin(u);

  // data path source
  assert(upin->_at[sfxt._el][urf]);
  path.emplace_back(*upin, urf, *upin->_at[sfxt._el][urf]);
  
  // recursive
  while(u != sfxt._T) {
    assert(sfxt.__link[u]);
    auto [arc, frf, trf] = _decode_arc(*sfxt.__link[u]);
    u = *sfxt.__tree[u];
    std::tie(upin, urf) = _decode_pin(u);
    assert(path.back().transition == frf && urf == trf);
    auto at = path.back().at + *arc->_delay[sfxt._el][frf][trf];
    path.emplace_back(*upin, urf, at);
  }
}

// Procedure: _recover_datapath
// recover the data path from a given prefix tree node w.r.t. a suffix tree
void Timer::_recover_datapath(
  Path& path, const SfxtCache& sfxt, const PfxtNode* node, size_t v
) const {

  if(node == nullptr) {
    return;
  }

  _recover_datapath(path, sfxt, node->parent, node->from);

  auto u = node->to;
  auto [upin, urf] = _decode_pin(u);
  
  // data path source
  if(node->from == sfxt._S) {
    assert(upin->_at[sfxt._el][urf]);
    path.emplace_back(*upin, urf, *upin->_at[sfxt._el][urf]);
  }
  // internal deviation
  else {
    assert(!path.empty());
    auto at = path.back().at + *node->arc->_delay[sfxt._el][path.back().transition][urf];
    path.emplace_back(*upin, urf, at);
  }

  while(u != v) {
    assert(sfxt.__link[u]);
    auto [arc, frf, trf] = _decode_arc(*sfxt.__link[u]);
    u = *sfxt.__tree[u];   
    std::tie(upin, urf) = _decode_pin(u);
    assert(path.back().transition == frf && urf == trf);
    auto at = path.back().at + *arc->_delay[sfxt._el][frf][trf]; 
    path.emplace_back(*upin, urf, at);
  }

}

};  // end of namespace ot. -----------------------------------------------------------------------






