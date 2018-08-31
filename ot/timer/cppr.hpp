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

// ----------------------------------------------------------------------------

// Struct: CpprCredit 
struct CpprCredit {
  Pin& pin;
  Tran rf;
  float credit;
  inline CpprCredit(Pin&, Tran, float);
  inline operator float() const;
};

// Constructor
CpprCredit::CpprCredit(Pin& pin, Tran rf, float c) :
  pin    {pin},
  rf     {rf},
  credit {c} {
}

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
