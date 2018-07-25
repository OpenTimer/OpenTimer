#ifndef OT_SDC_SDC_HPP_
#define OT_SDC_SDC_HPP_

#include <ot/sdc/object.hpp>
#include <ot/sdc/mask.hpp>

namespace ot::sdc {

// SetInputDelay
struct SetInputDelay {
  inline static constexpr auto command = "set_input_delay";
  std::optional<std::byte> clock_fall;
  std::optional<std::byte> level_sensitive;
  std::optional<std::byte> add_delay;
  std::optional<std::byte> network_latency_included; 
  std::optional<std::byte> source_latency_included;
  std::optional<std::byte> min;
  std::optional<std::byte> max;
  std::optional<std::byte> rise;
  std::optional<std::byte> fall;
  std::optional<float> value;
  std::optional<Clock> clock;
  std::optional<Port> port;
};

std::ostream& operator << (std::ostream&, const SetInputDelay&);

// ------------------------------------------------------------------------------------------------

// SetInputTransition
// Set a fixed transition on input or inout ports
struct SetInputTransition {
  inline static constexpr auto command = "set_input_transition";
  std::optional<std::byte> min;
  std::optional<std::byte> max;
  std::optional<std::byte> rise;
  std::optional<std::byte> fall;
  std::optional<std::byte> clock_fall;
  std::optional<float> value;
  std::optional<Clock> clock;
  std::optional<Port> port;
};

std::ostream& operator << (std::ostream&, const SetInputTransition&);

// ------------------------------------------------------------------------------------------------

// SetOutputDelay
struct SetOutputDelay {
  inline static constexpr auto command = "set_output_delay";
  std::optional<Clock> clock;
  std::optional<std::byte> clock_fall;
  std::optional<std::byte> level_sensitive;
  std::optional<std::byte> rise;
  std::optional<std::byte> fall;
  std::optional<std::byte> max;
  std::optional<std::byte> min;
  std::optional<std::byte> add_delay;
  std::optional<std::byte> network_latency_included; 
  std::optional<std::byte> source_latency_included;
  std::optional<float> value;
  std::optional<Port> port;
};

std::ostream& operator << (std::ostream&, const SetOutputDelay&);

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
  std::optional<Port> port;
};

std::ostream& operator << (std::ostream&, const SetLoad&);

// ------------------------------------------------------------------------------------------------

// CreateClock
struct CreateClock {
  inline static constexpr auto command = "create_clock";
  std::optional<float> period;
  std::optional<std::byte> add;
  std::string name;
  std::string comment;
  std::optional<std::array<float, MAX_TRAN>> waveform;
  std::optional<Port> port;
};

std::ostream& operator << (std::ostream&, const CreateClock&);

// ------------------------------------------------------------------------------------------------

using Command = std::variant<
  SetInputDelay,
  SetInputTransition,
  SetOutputDelay,
  SetLoad,
  CreateClock
>;

SetInputDelay      extract_set_input_delay     (token_iter_t&, token_iter_t);
SetInputTransition extract_set_input_transition(token_iter_t&, token_iter_t);
SetOutputDelay     extract_set_output_delay    (token_iter_t&, token_iter_t);
SetLoad            extract_set_load            (token_iter_t&, token_iter_t);
CreateClock        extract_create_clock        (token_iter_t&, token_iter_t);

// ------------------------------------------------------------------------------------------------

// Class: SDC
struct SDC {

  std::vector<Command> commands;

  void read(const std::string&);
  void read(const std::filesystem::path&);
};

};  // end of namespace ot::sdc. ------------------------------------------------------------------


#endif
