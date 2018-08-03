#include <ot/timer/endpoint.hpp>
#include <ot/timer/timer.hpp>

namespace ot {

// Constructor
Endpoint::Endpoint(Split el, Tran rf, Test& test) : 
  _el  {el},
  _rf  {rf},
  _handle {test} {
  
  OT_LOGF_IF(!test.slack(el, rf), "test slack not defined");
}

// Constructor
Endpoint::Endpoint(Split el, Tran rf, PrimaryOutput& po) :
  _el {el},
  _rf {rf},
  _handle {po} {
  
  OT_LOGF_IF(!po.slack(el, rf), "PO slack not defined");
}

// Operator <
bool Endpoint::operator < (const Endpoint& rhs) const {
  return slack() < rhs.slack();
}

// Operator >
bool Endpoint::operator > (const Endpoint& rhs) const {
  return slack() > rhs.slack();
}

// Operator ==
bool Endpoint::operator == (const Endpoint& rhs) const {
  return slack() == rhs.slack();
}

// Function: is_po
bool Endpoint::is_po() const {
  return std::get_if<PrimaryOutputHandle>(&_handle) != nullptr;
}

// Function: is_test
bool Endpoint::is_test() const {
  return std::get_if<TestHandle>(&_handle) != nullptr;
}

// Function: _test
Test& Endpoint::_test() {
  return std::get<TestHandle>(_handle);
}

// Function: _po
PrimaryOutput& Endpoint::_po() {
  return std::get<PrimaryOutputHandle>(_handle);
}

// Function: slack
float Endpoint::slack() const {
  return std::visit([this] (auto&& handle) {
    return *(handle.get().slack(_el, _rf));
  }, _handle);
}

// ------------------------------------------------------------------------------------------------

/*// Function: empty
bool EndpointHeap::empty() const {
  return _array.empty();
}

// Function: top 
Endpoint* EndpointHeap::top() const {
  return empty() ? nullptr : _array[0];
}

// Function: size
size_t EndpointHeap::size() const {
  return _array.size();
}

// Procedure: clear 
void EndpointHeap::clear() {
  for(auto e : _array) {
    e->_satellite.reset();
  }
  _array.clear();
}

// Procedure: remove   
void EndpointHeap::remove(Endpoint& e) {

  // endpoint is not in the heap.
  if(!e._satellite) {
    return;
  }

  // Endpoint exists in the heap.
  auto last = _array.back();
  _array.pop_back();

  if(e._satellite && _cmp_slack(*last, *_array[(e._satellite.value()-1) >> 1])) {
    _bubble_up(e._satellite.value(), *last);
  }
  else {
    _bubble_down(e._satellite.value(), *last);
  }

  e._satellite.reset();
}

// Function: insert 
void EndpointHeap::insert(Endpoint& e) {

  // endpoint is in the heap
  if(e._satellite) {
    return;
  }

  _array.push_back(&e);
  _bubble_up(_array.size()-1, e);
}

// Function: pop 
Endpoint* EndpointHeap::pop() {
  if(_array.size()) {
    auto e = _array[0]; 
    auto last = _array.back();
    _array.pop_back();
    _bubble_down(0, *last);
    e->_satellite.reset();
    return e;
  }
  return nullptr;
}

// Function: _cmp_slack
bool EndpointHeap::_cmp_slack(const Endpoint& l, const Endpoint& r) const {
  float ls = l.slack() ? *(l.slack()) : std::numeric_limits<float>::max();
  float rs = r.slack() ? *(r.slack()) : std::numeric_limits<float>::max();
  return ls < rs;
}

// Procedure: _bubble_up
void EndpointHeap::_bubble_up(size_t idx, Endpoint& e) {
  while(idx) {
    size_t parent = (idx - 1) >> 1;
    if(!_cmp_slack(e, *_array[parent])) {
      break;
    }
    (_array[idx] = _array[parent])->_satellite = idx;
    idx = parent;
  }
  (_array[idx] = &e)->_satellite = idx;
}

// Procedure: _bubble_down 
void EndpointHeap::_bubble_down(size_t idx, Endpoint& e) {
  while(1) {
    size_t min_child = (idx + 1) << 1;
    if(min_child >= _array.size() || _cmp_slack(*_array[min_child-1], *_array[min_child])) {
      --min_child;
    }
    if(min_child >= _array.size() || !_cmp_slack(*_array[min_child], e)) break;
    (_array[idx] = _array[min_child])->_satellite = idx;
    idx = min_child;
  }
  (_array[idx] = &e)->_satellite = idx;
}
*/

// ------------------------------------------------------------------------------------------------

// Function: _worst_endpoints
std::vector<Endpoint> Timer::_worst_endpoints(size_t K, Split el, Tran rf) {
  _update_endpoints();
  auto beg = _endpoints[el][rf].begin();
  auto end = std::next(_endpoints[el][rf].begin(), std::min(K, _endpoints[el][rf].size()));
  return std::vector<Endpoint>(beg, end);
}

// Function: _worst_endpoints
std::vector<Endpoint> Timer::_worst_endpoints(size_t K) {

  _update_endpoints();

  std::vector<Endpoint> epts;
  std::array<std::array<size_t, MAX_TRAN>, MAX_SPLIT> i {0, 0, 0, 0};

  for(size_t k=0; k<K; ++k) {

    std::optional<Split> mel;
    std::optional<Tran> mrf;

    FOR_EACH_EL_RF_IF(el, rf, i[el][rf] < _endpoints[el][rf].size()) {
      if(!mel || _endpoints[el][rf][i[el][rf]] < _endpoints[*mel][*mrf][i[*mel][*mrf]]) {
        mel = el;
        mrf = rf;
      }
    }

    if(!mel) break;
    
    epts.push_back(_endpoints[*mel][*mrf][i[*mel][*mrf]]);
    ++i[*mel][*mrf];
  }

  return epts;
}

// Function: _worst_endpoints
std::vector<Endpoint> Timer::_worst_endpoints(size_t K, Split el) {

  _update_endpoints();

  std::vector<Endpoint> epts;
  std::array<size_t, MAX_TRAN> i {0, 0};

  for(size_t k=0; k<K; ++k) {

    std::optional<Tran> mrf;

    FOR_EACH_RF_IF(rf, i[rf] < _endpoints[el][rf].size()) {
      if(!mrf || _endpoints[el][rf][i[rf]] < _endpoints[el][*mrf][i[*mrf]]) {
        mrf = rf;
      }
    }

    if(!mrf) break;
    
    epts.push_back(_endpoints[el][*mrf][i[*mrf]]);
    ++i[*mrf];
  }

  return epts;
}

// Function: _worst_endpoints
std::vector<Endpoint> Timer::_worst_endpoints(size_t K, Tran rf) {

  _update_endpoints();

  std::vector<Endpoint> epts;
  std::array<size_t, MAX_SPLIT> i {0, 0};

  for(size_t k=0; k<K; ++k) {

    std::optional<Split> mel;

    FOR_EACH_EL_IF(el, i[el] < _endpoints[el][rf].size()) {
      if(!mel || _endpoints[el][rf][i[el]] < _endpoints[*mel][rf][i[*mel]]) {
        mel = el;
      }
    }

    if(!mel) break;
    
    epts.push_back(_endpoints[*mel][rf][i[*mel]]);
    ++i[*mel];
  }

  return epts;
}

//


};  // end of namespace ot. -----------------------------------------------------------------------
