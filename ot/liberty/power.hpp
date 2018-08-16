#ifndef OT_LIBERTY_POWER_HPP_
#define OT_LIBERTY_POWER_HPP_

#include <ot/liberty/lut.hpp>

namespace ot {

// Struct: InternalPower
struct InternalPower {

  std::string related_pin;
  
  std::optional<Lut> rise_power;
  std::optional<Lut> fall_power;

};

};  // end of namespace ot. -----------------------------------------------------------------------


#endif
