#ifndef OT_TIMER_SFXT_HPP_
#define OT_TIMER_SFXT_HPP_

#include <ot/headerdef.hpp>

namespace ot {

// SfxtCache: The internal thread-local storage to construct suffix tree
// for path-based timing analysis.
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

  private:

    inline thread_local static std::vector<std::optional<float>>  __dist;
    inline thread_local static std::vector<std::optional<size_t>> __tree;
    inline thread_local static std::vector<std::optional<size_t>> __link;
    inline thread_local static std::vector<std::optional<bool>>   __spfa;

    Split _el;
    size_t _S;
    size_t _T;

    std::unordered_set<size_t> _pins;
    std::unordered_set<size_t> _srcs;

    bool _relax(size_t, size_t, std::optional<size_t>, float);
};

// Function: slack
inline std::optional<float> SfxtCache::slack() const {
  return __dist[_S];
}

};  // end of namespace ot. -----------------------------------------------------------------------

#endif
