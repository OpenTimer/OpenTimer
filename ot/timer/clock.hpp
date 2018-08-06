#ifndef OT_TIMER_CLOCK_HPP_
#define OT_TIMER_CLOCK_HPP_

#include <ot/timer/pin.hpp>

namespace ot {

// Class: Clock
class Clock {

  friend class Timer;

  public:

    Clock(const std::string&, float);
    Clock(const std::string&, Pin&, float);

    inline const std::string& name() const;
    inline const std::string& comment() const;

    inline float period() const;
    inline float waveform(Tran) const;

  private:

    std::string _name;

    Pin* _source {nullptr};

    float _period {.0f};

    std::array<float, MAX_TRAN> _waveform;

    void _scale_time(float);
};

// Function: name
inline const std::string& Clock::name() const {
  return _name;
}

// Function: period
inline float Clock::period() const {
  return _period;
}

// Function: waveform
inline float Clock::waveform(Tran rf) const {
  return _waveform[rf];
}

    
};  // end of namespace ot ------------------------------------------------------------------------

#endif




