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

// ------------------------------------------------------------------------------------------------

// Class: PfxtCache
class PfxtCache {

  friend class Timer;
  
  // Min-heap comparator
  struct PfxtNodeComparator {
    bool operator () (std::unique_ptr<PfxtNode>& a, std::unique_ptr<PfxtNode>& b) const {
      return a->slack > b->slack;
    }
  };

  public:
    
    inline size_t num_nodes() const;
    inline size_t num_paths() const;

    PfxtCache(const SfxtCache&);

    PfxtCache(const PfxtCache&) = delete;
    PfxtCache(PfxtCache&&);

    PfxtCache& operator = (const PfxtCache&) = delete;
    PfxtCache& operator = (PfxtCache&&) = delete;

  private:

    const SfxtCache& _sfxt;

    PfxtNodeComparator _comp;

    std::vector<std::unique_ptr<PfxtNode>> _nodes;
    std::vector<std::unique_ptr<PfxtNode>> _paths;

    void _push(float, size_t, size_t, Arc*, PfxtNode*);
    void _pop();

    PfxtNode* _top() const;
};

// Function: num_nodes
inline size_t PfxtCache::num_nodes() const {
  return _nodes.size();
}

// Function: num_paths
inline size_t PfxtCache::num_paths() const {
  return _paths.size();
}

};  // end of namespace ot. ----------------------------------------------------------------------

#endif




