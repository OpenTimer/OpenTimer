#include <ot/timer/sfxt.hpp>
#include <ot/timer/timer.hpp>

namespace ot {

// Constructor
SfxtCache::SfxtCache(Split el, size_t S, size_t T) : _el {el}, _S {S}, _T {T} {
  resize_to_fit(std::max(S, T) + 1, __tree, __link, __dist, __spfa);
  _pins.insert(S);
  _pins.insert(T);
  // debug
  //for(const auto& i : __tree) assert(!i);
  //for(const auto& i : __dist) assert(!i);
  //for(const auto& i : __spfa) assert(!i);
}

// Move constructor
SfxtCache::SfxtCache(SfxtCache&& rhs) : 
  _pins {std::move(rhs._pins)},
  _srcs {std::move(rhs._srcs)} {
}

// Destructor
SfxtCache::~SfxtCache() {
  for(auto p : _pins) {
    __dist[p].reset();
    __tree[p].reset();
    __link[p].reset();
    __spfa[p].reset();
  }
}
        
// Function: _relax        
bool SfxtCache::_relax(size_t u, size_t v, std::optional<size_t> e, float d) {
  if(!__dist[u] || *__dist[v] + d < *__dist[u]) {
    __dist[u] = *__dist[v] + d;
    __tree[u] = v;
    __link[u] = e;
    return true;
  }
  return false;
}

// ------------------------------------------------------------------------------------------------

// Procedure: _spfa
// Perform shortest path fast algorithm (SPFA) to build up the suffix tree.
void Timer::_spfa(SfxtCache& sfxt, std::queue<size_t>& queue) const {

  auto el = sfxt._el;

  while(!queue.empty()) {

    auto v = queue.front();
    queue.pop();
    sfxt.__spfa[v] = false;
    sfxt._pins.insert(v);

    auto [pin, vrf] = _decode_pin(v);

    // Stop at the data source
    if(pin->is_datapath_source()) {
      sfxt._srcs.insert(v);
      continue;
    }

    // Relax on fanin
    for(auto arc : pin->_fanin) {
      FOR_EACH_RF_IF(urf, arc->_delay[el][urf][vrf]) {
        auto u = _encode_pin(arc->_from, urf);
        auto d = (el == EARLY) ? *arc->_delay[el][urf][vrf] : -(*arc->_delay[el][urf][vrf]);
        if(sfxt._relax(u, v, arc->_idx, d)) {
          if(!sfxt.__spfa[u] || *sfxt.__spfa[u] == false) {
            queue.push(u);
            sfxt.__spfa[u] = true;
          }
        }
      }
    }
  }

}

// Function: _sfxt_cache
SfxtCache Timer::_sfxt_cache(const PrimaryOutput& po, Split el, Tran rf) const {
  
  assert(po._rat[el][rf]);

  // create a cache 
  auto S = _idx2pin.size() << 1;
  auto v = _encode_pin(po._pin, rf);

  SfxtCache sfxt(el, S, v);

  assert(!sfxt.__dist[v]);

  // start at the root
  std::queue<size_t> queue;
  queue.push(v);
  sfxt.__dist[v] = (el == EARLY) ? -(*po._rat[el][rf]) : *po._rat[el][rf];
  sfxt.__spfa[v] == true;

  // build the tree
  _spfa(sfxt, queue);
  
  // relax sources
  for(auto s : sfxt._srcs) {
    if(auto offset = _sfxt_offset(sfxt, s); offset) {
      sfxt._relax(S, s, std::nullopt, *offset);
    }
  }

  return sfxt;
}

// Function: _sfxt_cache
SfxtCache Timer::_sfxt_cache(const Test& test, Split el, Tran rf) const {

  assert(test._rat[el][rf]);

  // create a cache
  auto S = _idx2pin.size() << 1;
  auto v = _encode_pin(test._arc._to, rf);
  SfxtCache sfxt(el, S, v);

  // Start at the D pin and perform SPFA all the way to the sources of data paths.
  assert(!sfxt.__dist[v]);
  std::queue<size_t> queue;
  queue.push(v);
  sfxt.__dist[v] = (el == EARLY) ? -(*test._rat[el][rf]) : *test._rat[el][rf];
  sfxt.__spfa[v] = true;
  
  // Build the tree
  _spfa(sfxt, queue);

  // relaxation from the sources
  if(_cppr_analysis) {
    auto cppr = _cppr_cache(test, el, rf);
    for(auto s : sfxt._srcs) {
      auto [pin, srf] = _decode_pin(s);
      if(auto offset = _cppr_offset(cppr, *pin, el, srf); offset) {
        sfxt._relax(S, s, std::nullopt, *offset);
      }
    }
  }
  else {
    for(auto s : sfxt._srcs) {
      if(auto offset = _sfxt_offset(sfxt, s); offset) {
        sfxt._relax(S, s, std::nullopt, *offset);
      }
    }
  }

  return sfxt;
}

// Function: _sfxt_cache
SfxtCache Timer::_sfxt_cache(const Endpoint& ept) const {
  return std::visit([this, &ept] (auto&& handle) {
    return _sfxt_cache(handle, ept._el, ept._rf);
  }, ept._handle);
}

// Function: _sfxt_offset
std::optional<float> Timer::_sfxt_offset(const SfxtCache& sfxt, size_t v) const {

  auto [pin, rf] = _decode_pin(v);
  
  if(auto at = pin->_at[sfxt._el][rf]; at) {
    return sfxt._el == EARLY ? *at : -*at;
  }
  else {
    return std::nullopt;
  }
}

};  // end of namespace ot. -----------------------------------------------------------------------








