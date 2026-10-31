#ifndef OT_LIBERTY_CELLPIN_HPP_
#define OT_LIBERTY_CELLPIN_HPP_

#include <ot/liberty/timing.hpp>
#include <ot/liberty/power.hpp>

namespace ot {

// Struct: CellpinDirection
enum class CellpinDirection {
  INPUT,
  OUTPUT,
  INOUT,
  INTERNAL
};

inline const std::unordered_map<std::string_view, CellpinDirection> cellpin_directions {
  {"input",    CellpinDirection::INPUT},
  {"output",   CellpinDirection::OUTPUT},
  {"inout",    CellpinDirection::INOUT},
  {"internal", CellpinDirection::INTERNAL}
};

// Library-scoped waveform conventions survive cell-library merging.
struct SlewThresholds {
  std::array<float, 2> lower {0.2f, 0.2f};
  std::array<float, 2> upper {0.8f, 0.8f};
  std::array<float, 2> input {0.5f, 0.5f};
  std::array<float, 2> output {0.5f, 0.5f};
  float derate {1.0f};
  float span(Tran rf) const { return upper[rf] - lower[rf]; }
};

// Class: Cellpin
struct Cellpin {

  std::string name;
  std::string original_pin;
  std::optional<std::string> function;
  bool three_state {false};
  SlewThresholds thresholds;

  std::optional<CellpinDirection> direction;
  std::optional<float> capacitance;      // Pin capacitance
  std::optional<float> max_capacitance;  // Max pin capacitance (output pin).
  std::optional<float> min_capacitance;  // Min pin capacitance (output pin).
  std::optional<float> max_transition;   // Max transition.
  std::optional<float> min_transition;   // Min transition.
  std::optional<float> fall_capacitance; 
  std::optional<float> rise_capacitance;
  std::array<std::optional<float>, 2> rise_capacitance_range, fall_capacitance_range;
  std::optional<float> fanout_load;
  std::optional<float> max_fanout;
  std::optional<float> min_fanout;
  std::optional<bool> is_clock;          // Is clock pin.

  std::vector<Timing> timings;
  std::vector<InternalPower> internal_power; // Input and/or output pin related

  const Timing* isomorphic_timing(const Timing&) const;

  void scale_time(float);
  void scale_capacitance(float);
};

std::ostream& operator << (std::ostream&, const Cellpin&);

// ------------------------------------------------------------------------------------------------

using CellpinView = std::array<const Cellpin*, MAX_SPLIT>;

};  // end of namespace ot ------------------------------------------------------------------------


#endif
