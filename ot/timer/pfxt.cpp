#include <ot/timer/pfxt.hpp>
#include <ot/timer/sfxt.hpp>
#include <ot/timer/arc.hpp>
#include <ot/timer/timer.hpp>

namespace ot {
  
// Constructor
PfxtNode::PfxtNode(float s, size_t f, size_t t, Arc* a, PfxtNode* p) :
  slack  {s},
  from   {f},
  to     {t},
  arc    {a},
  parent {p} {
}

// ------------------------------------------------------------------------------------------------

// Constructor
PfxtCache::PfxtCache(const SfxtCache& sfxt) : _sfxt {sfxt} {
}

// Move constructor
PfxtCache::PfxtCache(PfxtCache&& pfxt) : 
  _sfxt  {pfxt._sfxt},
  _nodes {std::move(pfxt._nodes)},
  _paths {std::move(pfxt._paths)},
  _heap  {std::move(pfxt._heap)} {
}

// Procedure: insert
PfxtNode& PfxtCache::_insert(float s, size_t f, size_t t, Arc* a, PfxtNode* p) {
  auto& pfx = _nodes.emplace_back(std::make_unique<PfxtNode>(s, f, t, a, p));
  _heap.push(pfx.get());
  return *pfx;
}

// ------------------------------------------------------------------------------------------------

// Function: _pfxt_cacheS
// Construct a prefix tree from a given suffix tree.
PfxtCache Timer::_pfxt_cache(const SfxtCache& sfxt) const {

  PfxtCache pfxt(sfxt);

  assert(sfxt.slack());

  // Generate the path prefix from each startpoint. 
  for(const auto& [k, v] : sfxt._srcs) {
    if(!v) {
      continue;
    }
    else if(auto s = *sfxt.__dist[k] + *v; s < 0.0f) {
      pfxt._insert(s, sfxt._S, k, nullptr, nullptr);
    }
  }

  return pfxt;
}

// Procedure: _spur
// Spur the path and expands the search space. The procedure iteratively scan the present
// critical path and performs spur operation along the path to generate other candidates.
void Timer::_spur(PfxtCache& pfxt, size_t K) {

  for(size_t k=0; k<K && !pfxt._heap.empty(); ++k) {
    
    // create a new path
    PfxtNode* pfx = pfxt._heap.top();
    pfxt._heap.pop();
    pfxt._paths.push_back(pfx);

    // spur from the path
    _spur(pfxt, *pfx);
  }
}

// Procedure: _spur
void Timer::_spur(PfxtCache& pfxt, PfxtNode& pfx) {
  
  auto el = pfxt._sfxt._el;
  auto T  = pfxt._sfxt._T;
  auto u  = pfx.to;

  while(u != T) {

    assert(pfxt._sfxt.__link[u]);

    auto [upin, urf] = _decode_pin(u);

    for(auto arc : upin->_fanout) {
      
      // skip if the edge belongs to the suffix tree
      if(*pfxt._sfxt.__link[u] == arc->_idx) {
        continue;
      }

      FOR_EACH_RF_IF(vrf, arc->_delay[el][urf][vrf]) {
        auto v = _encode_pin(arc->_to, vrf);

        // skip if the edge goes outside the sfxt
        if(!pfxt._sfxt.__dist[v]) {
          continue;
        }

        auto w = (el == EARLY) ? *arc->_delay[el][urf][vrf] : -(*arc->_delay[el][urf][vrf]);
        auto s = *pfxt._sfxt.__dist[v] + w - *pfxt._sfxt.__dist[u] + pfx.slack;

        if(s < 0.0f) {
          pfxt._insert(s, u, v, arc, &pfx);
        }
      }
    }
    u = *pfxt._sfxt.__tree[u];
  }
}


};  // end of namespace ot. -----------------------------------------------------------------------


