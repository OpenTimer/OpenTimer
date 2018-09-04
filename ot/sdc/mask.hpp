#ifndef OT_SDC_MASK_HPP_
#define OT_SDC_MASK_HPP_

#include <ot/headerdef.hpp>

namespace ot::sdc {

// Struct: TimingMask
struct TimingMask {
  inline static constexpr auto min_mask  = 0x01;
  inline static constexpr auto max_mask  = 0x02;
  inline static constexpr auto rise_mask = 0x04;
  inline static constexpr auto fall_mask = 0x08;

  constexpr TimingMask(
    std::optional<std::byte> min, 
    std::optional<std::byte> max,
    std::optional<std::byte> rise, 
    std::optional<std::byte> fall
  ) {

    if(min) mask |= min_mask;
    if(max) mask |= max_mask;
    if(!min && !max) mask |= (min_mask | max_mask);

    if(rise) mask |= rise_mask;
    if(fall) mask |= fall_mask;
    if(!rise && !fall) mask |= (rise_mask | fall_mask);
  }

  int mask {0};

  constexpr bool operator | (Split el) const {
    return (el == MIN) ? mask & min_mask : mask & max_mask;
  }

  constexpr bool operator | (Tran rf) const {
    return (rf == RISE) ? mask & rise_mask : mask & fall_mask;
  }
};

};  // end of namespace ot::sdc. ------------------------------------------------------------------

#endif
