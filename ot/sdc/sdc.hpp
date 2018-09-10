#ifndef OT_SDC_SDC_HPP_
#define OT_SDC_SDC_HPP_

#include <ot/sdc/object.hpp>
#include <ot/sdc/mask.hpp>

namespace ot::sdc {

// Function: home
std::filesystem::path home();

// SetInputDelay
struct SetInputDelay {

  inline static constexpr auto command = "set_input_delay";

  std::string clock;
  std::optional<std::byte> clock_fall;
  std::optional<std::byte> level_sensitive;
  std::optional<std::byte> add_delay;
  std::optional<std::byte> network_latency_included; 
  std::optional<std::byte> source_latency_included;
  std::optional<std::byte> min;
  std::optional<std::byte> max;
  std::optional<std::byte> rise;
  std::optional<std::byte> fall;
  std::optional<float> delay_value;
  std::optional<Object> port_pin_list;

  SetInputDelay() = default;
  SetInputDelay(const Json&);
};

// ------------------------------------------------------------------------------------------------

// SetInputTransition
// Set a fixed transition on input or inout ports
struct SetInputTransition {

  inline static constexpr auto command = "set_input_transition";

  std::string clock;
  std::optional<std::byte> min;
  std::optional<std::byte> max;
  std::optional<std::byte> rise;
  std::optional<std::byte> fall;
  std::optional<std::byte> clock_fall;
  std::optional<float> transition;
  std::optional<Object> port_list;

  SetInputTransition() = default;
  SetInputTransition(const Json&);
};

// ------------------------------------------------------------------------------------------------

// SetOutputDelay
struct SetOutputDelay {

  inline static constexpr auto command = "set_output_delay";

  std::string clock;
  std::optional<std::byte> clock_fall;
  std::optional<std::byte> level_sensitive;
  std::optional<std::byte> rise;
  std::optional<std::byte> fall;
  std::optional<std::byte> max;
  std::optional<std::byte> min;
  std::optional<std::byte> add_delay;
  std::optional<std::byte> network_latency_included; 
  std::optional<std::byte> source_latency_included;
  std::optional<float> delay_value;
  std::optional<Object> port_pin_list;

  SetOutputDelay() = default;
  SetOutputDelay(const Json&);
};

// ------------------------------------------------------------------------------------------------

// SetLoad
struct SetLoad {

  inline static constexpr auto command = "set_load";

  std::optional<std::byte> min;
  std::optional<std::byte> max;
  std::optional<std::byte> subtract_pin_load;
  std::optional<std::byte> pin_load;
  std::optional<std::byte> wire_load;
  std::optional<float> value;
  std::optional<Object> objects;

  SetLoad() = default;
  SetLoad(const Json&);
};

// ------------------------------------------------------------------------------------------------

// CreateClock
struct CreateClock {

  inline static constexpr auto command = "create_clock";

  std::optional<float> period;
  std::optional<std::byte> add;
  std::string name;
  std::string comment;
  std::optional<std::array<float, MAX_TRAN>> waveform;
  std::optional<Object> port_pin_list;
  
  CreateClock() = default;
  CreateClock(const Json&);
};

// ------------------------------------------------------------------------------------------------

// Class: SetClockUncertainty 
struct SetClockUncertainty {

  inline static constexpr auto command = "set_clock_uncertainty";

  std::optional<float> uncertainty;
  std::optional<Object> object_list;

  SetClockUncertainty() = default;
  SetClockUncertainty(const Json&);
};

// ------------------------------------------------------------------------------------------------

using Command = std::variant<
  SetInputDelay,
  SetInputTransition,
  SetOutputDelay,
  SetLoad,
  CreateClock
>;

// ------------------------------------------------------------------------------------------------

// Class: SDC
struct SDC {
  std::vector<Command> commands;
  void read(const std::filesystem::path&);
};

};  // end of namespace ot::sdc. ------------------------------------------------------------------


#endif
