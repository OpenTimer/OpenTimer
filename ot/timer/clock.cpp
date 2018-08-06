#include <ot/timer/clock.hpp>

namespace ot {

// Constructor
Clock::Clock(const std::string& name, float period) : 
  _name     {name},
  _source   {nullptr},
  _period   {period},
  _waveform {0.0f, period / 2.0f} {
}

// Constructor
Clock::Clock(const std::string& name, Pin& source, float period) :
  _name     {name},
  _source   {&source},
  _period   {period},
  _waveform {0.0f, period / 2.0f} {
}

//// Constructor
//Clock::Clock(
//  const std::string& name, 
//  Pin& root, 
//  float period, 
//  std::array<float, MAX_TRAN> waveform
//) : 
//  _name     {name},
//  _root     {root},
//  _period   {period},
//  _waveform {waveform} {
//}

// Procedure: _scale_time
void Clock::_scale_time(float s) {

  _period *= s; 

  FOR_EACH_RF(rf) {
    _waveform[rf] *= s;
  }
}


};  // end of namespace ot. -----------------------------------------------------------------------
