#include <ot/timer/endpoint.hpp>
#include <ot/timer/timer.hpp>

namespace ot {

// Constructor
Endpoint::Endpoint(Split el, Tran rf, Test& test) : 
  _el  {el},
  _rf  {rf},
  _handle {&test} {
  
  OT_LOGF_IF(!test.slack(el, rf), "test slack not defined");
}

// Constructor
Endpoint::Endpoint(Split el, Tran rf, PrimaryOutput& po) :
  _el {el},
  _rf {rf},
  _handle {&po} {
  
  OT_LOGF_IF(!po.slack(el, rf), "PO slack not defined");
}

// Function: slack
float Endpoint::slack() const {
  return std::visit([this] (auto&& handle) {
    return *(handle->slack(_el, _rf));
  }, _handle);
}

// ------------------------------------------------------------------------------------------------

// Function: _worst_endpoints
std::vector<Endpoint*> Timer::_worst_endpoints(size_t K, Split el, Tran rf) {
  _update_endpoints();
  auto beg = _endpoints[el][rf].begin();
  auto end = std::next(_endpoints[el][rf].begin(), std::min(K, _endpoints[el][rf].size()));
  std::vector<Endpoint*> epts;
  std::transform(beg, end, std::back_inserter(epts), [] (Endpoint& ept) {
    return &ept;
  });
  return epts;
}

// Function: _worst_endpoints
std::vector<Endpoint*> Timer::_worst_endpoints(size_t K) {

  _update_endpoints();

  std::vector<Endpoint*> epts;
  std::array<std::array<size_t, MAX_TRAN>, MAX_SPLIT> i {{{0, 0}, {0, 0}}};

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
    
    epts.push_back(&_endpoints[*mel][*mrf][i[*mel][*mrf]]);
    ++i[*mel][*mrf];
  }

  return epts;
}

// Function: _worst_endpoints
std::vector<Endpoint*> Timer::_worst_endpoints(size_t K, Split el) {

  _update_endpoints();

  std::vector<Endpoint*> epts;
  std::array<size_t, MAX_TRAN> i {0, 0};

  for(size_t k=0; k<K; ++k) {

    std::optional<Tran> mrf;

    FOR_EACH_RF_IF(rf, i[rf] < _endpoints[el][rf].size()) {
      if(!mrf || _endpoints[el][rf][i[rf]] < _endpoints[el][*mrf][i[*mrf]]) {
        mrf = rf;
      }
    }

    if(!mrf) break;
    
    epts.push_back(&_endpoints[el][*mrf][i[*mrf]]);
    ++i[*mrf];
  }

  return epts;
}

// Function: _worst_endpoints
std::vector<Endpoint*> Timer::_worst_endpoints(size_t K, Tran rf) {

  _update_endpoints();

  std::vector<Endpoint*> epts;
  std::array<size_t, MAX_SPLIT> i {0, 0};

  for(size_t k=0; k<K; ++k) {

    std::optional<Split> mel;

    FOR_EACH_EL_IF(el, i[el] < _endpoints[el][rf].size()) {
      if(!mel || _endpoints[el][rf][i[el]] < _endpoints[*mel][rf][i[*mel]]) {
        mel = el;
      }
    }

    if(!mel) break;
    
    epts.push_back(&_endpoints[*mel][rf][i[*mel]]);
    ++i[*mel];
  }

  return epts;
}

// TODO (Guannan)
// Function: _worst_endpoints
std::vector<Endpoint*> Timer::_worst_endpoints(const PathGuide& guide) {

  _update_endpoints();
  
  std::vector<Endpoint*> epts;

  return epts;
}



};  // end of namespace ot. -----------------------------------------------------------------------


