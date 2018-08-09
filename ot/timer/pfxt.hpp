#ifndef OT_TIMER_PFXT_HPP_
#define OT_TIMER_PFXT_HPP_

#include <ot/headerdef.hpp>

namespace ot {

// Forward declaration
class SfxtCache;
class Arc;

// ------------------------------------------------------------------------------------------------

// Class: PfxtNode
struct PfxtNode {
  
  float slack;
  size_t from;
  size_t to;

  Arc* arc {nullptr};
  PfxtNode* parent {nullptr};

  PfxtNode(float, size_t, size_t, Arc*, PfxtNode*);
};

// Class: PfxtNodeComparator
struct PfxtNodeComparator {
  constexpr bool operator () (const PfxtNode* a, const PfxtNode* b) const {
    return a->slack > b->slack;
  }
};

// ------------------------------------------------------------------------------------------------

// Class: PfxtCache
class PfxtCache {

  friend class Timer;

  public:
    
    inline size_t num_nodes() const;
    inline size_t num_paths() const;
    inline size_t num_cands() const;

    PfxtCache(const SfxtCache&);

    PfxtCache(const PfxtCache&) = delete;
    PfxtCache(PfxtCache&&);

    PfxtCache& operator = (const PfxtCache&) = delete;
    PfxtCache& operator = (PfxtCache&&) = delete;

  private:

    const SfxtCache& _sfxt;

    std::vector<std::unique_ptr<PfxtNode>> _nodes;
    std::vector<PfxtNode*> _paths;
    std::priority_queue<PfxtNode*, std::vector<PfxtNode*>, PfxtNodeComparator> _heap;

    PfxtNode& _insert(float, size_t, size_t, Arc*, PfxtNode*);
};

// Function: num_nodes
inline size_t PfxtCache::num_nodes() const {
  return _nodes.size();
}

// Function: num_paths
inline size_t PfxtCache::num_paths() const {
  return _paths.size();
}

// Function: num_cands
inline size_t PfxtCache::num_cands() const {
  return _heap.size();
}

};  // end of namespace ot. ----------------------------------------------------------------------

#endif




