#ifndef OT_TAU15_TAU15_HPP_
#define OT_TAU15_TAU15_HPP_

#include <ot/headerdef.hpp>
#include <ot/static/logger.hpp>

namespace ot::tau15 {

// Struct: Clock
struct Clock {
  std::string pin;
  float period;
};

// Struct: AT
struct AT {
  std::string pin;
  std::array<std::array<float, ot::MAX_TRAN>, ot::MAX_SPLIT> value;
};

// Struct: Slew
struct Slew {
  std::string pin;
  std::array<std::array<float, ot::MAX_TRAN>, ot::MAX_SPLIT> value;
};

// Struct: RAT
struct RAT {
  std::string pin;
  std::array<std::array<float, ot::MAX_TRAN>, ot::MAX_SPLIT> value;
};

// Struct: Load
struct Load {
  std::string pin;
  float value;
};

using Assertion = std::variant<
  Clock,
  AT,
  RAT,
  Slew,
  Load
>;

// Struct: Timing
struct Timing {
  std::vector<Assertion> assertions;
  void read(const std::filesystem::path&);
};


};  // end of namespace ot ------------------------------------------------------------------------

#endif




