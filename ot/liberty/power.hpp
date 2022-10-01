#ifndef OT_LIBERTY_POWER_HPP_
#define OT_LIBERTY_POWER_HPP_

#include <ot/liberty/lut.hpp>

namespace ot {

// Struct: InternalPower
struct InternalPower {

  std::string related_pin;

  std::optional<Lut> rise_power;
  std::optional<Lut> fall_power;

  void scale_time(float s);
  void scale_capacitance(float s);

  std::optional<float> power(Tran, Tran, float, float) const;
};

std::ostream& operator << (std::ostream&, const InternalPower&);

};  // end of namespace ot. -----------------------------------------------------------------------


#endif
