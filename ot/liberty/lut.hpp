#ifndef OT_LIBERTY_LUT_HPP_
#define OT_LIBERTY_LUT_HPP_

#include <ot/headerdef.hpp>
#include <ot/static/logger.hpp>
#include <ot/utility/utility.hpp>
#include <ot/exception.hpp>

namespace ot {

// Enum: LutVar
enum class LutVar {
  TOTAL_OUTPUT_NET_CAPACITANCE = 0,
  INPUT_NET_TRANSITION,
  CONSTRAINED_PIN_TRANSITION,
  RELATED_PIN_TRANSITION,
  INPUT_TRANSITION_TIME,
};

// Function: is_time_lut_var
bool is_time_lut_var(LutVar);
bool is_capacitance_lut_var(LutVar);

// Function: to_string
std::string to_string(LutVar);

// Struct: LutTemplate
struct LutTemplate {

  std::string name;

  std::optional<LutVar> variable1;
  std::optional<LutVar> variable2;

  std::vector<float> indices1;
  std::vector<float> indices2;
};

std::ostream& operator << (std::ostream& os, const LutTemplate&);

// ------------------------------------------------------------------------------------------------

// Struct: TimingLut
struct TimingLut {
  
  std::string name;

  std::vector<float> indices1;
  std::vector<float> indices2;
  std::vector<float> table;

  const LutTemplate* lut_template {nullptr};
  
  float operator() (float, float) const;

  bool is_scalar() const;
  bool empty() const;
  
  void scale_time(float);
  void scale_capacitance(float);
};

std::ostream& operator << (std::ostream& os, const TimingLut&);

};  // end of namespace ot ------------------------------------------------------------------------

#endif
