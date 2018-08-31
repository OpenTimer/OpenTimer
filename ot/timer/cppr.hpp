#ifndef OT_TIMER_CPPR_HPP_
#define OT_TIMER_CPPR_HPP_

#include <ot/headerdef.hpp>

namespace ot {

// Forward declaration
class Pin;

// ----------------------------------------------------------------------------

// Struct: CpprAnalysis
struct CpprAnalysis {
  // TODO
};

// Struct: CpprCredit 
struct CpprCredit {
  Tran rf;
  Pin& pin;
  float credit;
  inline operator float() const;
};

// operator
inline CpprCredit::operator float() const {
  return credit;
}

// ----------------------------------------------------------------------------

// CpprCache: The internal thread-local storage for cppr.
class CpprCache {

  friend class Timer;

  public:

    CpprCache(size_t);
    CpprCache(const CpprCache&) = delete;
    CpprCache(CpprCache&&);
    ~CpprCache();

    CpprCache& operator = (const CpprCache&) = delete;
    CpprCache& operator = (CpprCache&&) = delete;

  private:

    inline thread_local static std::vector<std::optional<size_t>> __capp;
    
    size_t _capb;   // beg
    size_t _cape;   // end

    std::unordered_set<size_t> _pins;
};

};  // end of namespace ot. -----------------------------------------------------------------------


#endif
