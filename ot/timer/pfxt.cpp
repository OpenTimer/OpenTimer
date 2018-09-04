#include <ot/timer/timer.hpp>

namespace ot {
  
// Constructor
PfxtNode::PfxtNode(float s, size_t f, size_t t, const Arc* a, const PfxtNode* p) :
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
  _comp  {pfxt._comp},
  _paths {std::move(pfxt._paths)},
  _nodes {std::move(pfxt._nodes)} {
}

// Procedure: _push
void PfxtCache::_push(float s, size_t f, size_t t, const Arc* a, const PfxtNode* p) {
  _nodes.emplace_back(std::make_unique<PfxtNode>(s, f, t, a, p));
  std::push_heap(_nodes.begin(), _nodes.end(), _comp);
}

// Procedure: _pop
// Pop a path from the min-heap to the path vector. Here we need to keep the pointer
// ownership since the later path peeling process need access to the prefix tree node.
PfxtNode* PfxtCache::_pop() {
  if(_nodes.empty()) {
    return nullptr;
  }
  std::pop_heap(_nodes.begin(), _nodes.end(), _comp);
  _paths.push_back(std::move(_nodes.back()));
  _nodes.pop_back();
  return _paths.back().get();
}

// Function: _top
PfxtNode* PfxtCache::_top() const {
  return _nodes.empty() ? nullptr : _nodes.front().get();
}

// ------------------------------------------------------------------------------------------------

// Function: _pfxt_cache
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
      pfxt._push(s, sfxt._S, k, nullptr, nullptr);
    }
  }

  return pfxt;
}

// Procedure: _spur
// Spur the path and expands the search space. The procedure iteratively scan the present
// critical path and performs spur operation along the path to generate other candidates.
void Timer::_spur(Endpoint& ept, size_t K, PathHeap& heap) const {

  auto sfxt = _sfxt_cache(ept);
  auto pfxt = _pfxt_cache(sfxt);

  for(size_t k=0; k<K; ++k) {

    auto node = pfxt._pop();
    
    // no more path to generate
    if(node == nullptr) {
      break;
    }

    // If the maximum among the minimum is smaller than the current minimum,
    // there is no need to do more.
    if(heap.num_paths() >= K && heap.top()->slack <= node->slack) {
      break;
    }
    
    // push the path to the heap and maintain the top-k
    auto path = std::make_unique<Path>(node->slack, &ept);
    _recover_datapath(*path, sfxt, node, sfxt._T);

    heap.push(std::move(path));
    heap.fit(K);

    // expand the search space
    _spur(pfxt, *node);
  }
}

// Procedure: _spur
void Timer::_spur(PfxtCache& pfxt, const PfxtNode& pfx) const {
  
  auto el = pfxt._sfxt._el;
  auto u  = pfx.to;

  while(u != pfxt._sfxt._T) {

    assert(pfxt._sfxt.__link[u]);

    auto [upin, urf] = _decode_pin(u);

    for(auto arc : upin->_fanout) {
        
      FOR_EACH_RF_IF(vrf, arc->_delay[el][urf][vrf]) {

        // skip if the edge goes outside the sfxt
        auto v = _encode_pin(arc->_to, vrf);
        if(!pfxt._sfxt.__dist[v]) {
          continue;
        }

        // skip if the edge belongs to the suffix tree
        if(_encode_arc(*arc, urf, vrf) == *pfxt._sfxt.__link[u]) {
          continue;
        }

        auto w = (el == MIN) ? *arc->_delay[el][urf][vrf] : -(*arc->_delay[el][urf][vrf]);
        auto s = *pfxt._sfxt.__dist[v] + w - *pfxt._sfxt.__dist[u] + pfx.slack;

        if(s < 0.0f) {
          pfxt._push(s, u, v, arc, &pfx);
        }
      }
    }
    u = *pfxt._sfxt.__tree[u];
  }
}


};  // end of namespace ot. -----------------------------------------------------------------------


