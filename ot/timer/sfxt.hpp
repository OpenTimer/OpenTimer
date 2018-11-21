#ifndef OT_TIMER_SFXT_HPP_
#define OT_TIMER_SFXT_HPP_

#include <ot/headerdef.hpp>
#include <ot/static/logger.hpp>

namespace ot {

// Class: SfxtCache
// The internal thread-local storage to construct suffix tree for 
// path-based timing analysis. The suffix tree is a shortest path tree 
// generated from an endpoint.
class SfxtCache {

  friend class Timer;

  public:

    SfxtCache(Split, size_t, size_t);
    SfxtCache(const SfxtCache&) = delete;
    SfxtCache(SfxtCache&&);
    ~SfxtCache();

    SfxtCache& operator = (const SfxtCache&) = delete;
    SfxtCache& operator = (SfxtCache&&) = delete;

    inline std::optional<float> slack() const;
    inline Split split() const;
    inline size_t root() const;

  private:
    
    Split _el;
    size_t _S;    // super source
    size_t _T;    // root

    std::vector<size_t> _pins;
    
    std::unordered_map<size_t, std::optional<float>> _srcs;
    
    inline thread_local static std::vector<size_t> __pins;
    inline thread_local static std::vector<std::optional<float>>  __dist;
    inline thread_local static std::vector<std::optional<size_t>> __tree;
    inline thread_local static std::vector<std::optional<size_t>> __link;
    inline thread_local static std::vector<std::optional<bool>>   __spfa;

    inline bool _relax(size_t, size_t, std::optional<size_t>, float);
};

// Function: root
inline size_t SfxtCache::root() const {
  return _T;
}

// Function: slack
inline std::optional<float> SfxtCache::slack() const {
  return __dist[_S];
}

// Function: split
inline Split SfxtCache::split() const {
  return _el;
}

// Function: _relax        
inline bool SfxtCache::_relax(size_t u, size_t v, std::optional<size_t> e, float d) {
  if(!__dist[u] || *__dist[v] + d < *__dist[u]) {
    __dist[u] = *__dist[v] + d;
    __tree[u] = v;
    __link[u] = e;
    return true;
  }
  return false;
}

};  // end of namespace ot. -----------------------------------------------------------------------

#endif
