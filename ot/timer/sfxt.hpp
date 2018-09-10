#ifndef OT_TIMER_SFXT_HPP_
#define OT_TIMER_SFXT_HPP_

#include <ot/headerdef.hpp>

namespace ot {

// Struct: SfxtMirror
// Mirror of a suffix tree, i.e., a copy of the tree exported from SfxtCache.
struct SfxtMirror {
    
  Split el;
  size_t S;    // super source
  size_t T;    // root

  std::unordered_map<size_t, std::optional<float>>  dist;
  std::unordered_map<size_t, std::optional<size_t>> tree;
  std::unordered_map<size_t, std::optional<size_t>> link;
  std::unordered_map<size_t, std::optional<float>>  srcs;
};

// ------------------------------------------------------------------------------------------------

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

    //SfxtMirror mirrorize() const;

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

    bool _relax(size_t, size_t, std::optional<size_t>, float);
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

};  // end of namespace ot. -----------------------------------------------------------------------

#endif
