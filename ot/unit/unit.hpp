#ifndef OT_UNIT_UNIT_HPP_
#define OT_UNIT_UNIT_HPP_

#include <ot/headerdef.hpp>
#include <ot/traits.hpp>

namespace ot {

// ------------------------------------------------------------------------------------------------
  
using picoseconds  = std::chrono::duration<float, std::milli>;
using nanoseconds  = std::chrono::duration<float>;
using microseconds = std::chrono::duration<float, std::kilo>;

//using femtoseconds = std::chrono::duration<float, std::femto>;
//using picoseconds  = std::chrono::duration<float, std::pico>;
//using nanoseconds  = std::chrono::duration<float, std::nano>;
//using microseconds = std::chrono::duration<float, std::micro>;
//using milliseconds = std::chrono::duration<float, std::milli>;
//using seconds      = std::chrono::duration<float>;
//using kiloseconds  = std::chrono::duration<float, std::kilo>;
//using megaseconds  = std::chrono::duration<float, std::mega>;
//using gigaseconds  = std::chrono::duration<float, std::giga>;

using millivolts   = std::chrono::duration<float, std::milli>;
using volts        = std::chrono::duration<float>;

using microamps    = std::chrono::duration<float, std::micro>;
using milliamps    = std::chrono::duration<float, std::milli>;
using amps         = std::chrono::duration<float>;
using kiloamps     = std::chrono::duration<float, std::kilo>;

using picowatts    = std::chrono::duration<float, std::milli>;
using nanowatts    = std::chrono::duration<float>;
using microwatts   = std::chrono::duration<float, std::kilo>;
using milliwatts   = std::chrono::duration<float, std::mega>;

using femtofarads  = std::chrono::duration<float, std::milli>;
using picofarads   = std::chrono::duration<float>;

using ohms         = std::chrono::duration<float>;
using kiloohms     = std::chrono::duration<float, std::kilo>;

// ------------------------------------------------------------------------------------------------

inline const std::regex unit_regex(
  "([\\+-]?\\d*\\.?\\d+)\\s*([fpnumkM]?)\\s*([sfVAW]|Ohm)", 
  std::regex::icase
);

// Function: unit_cast
template <typename D, typename R, typename P>
constexpr D unit_cast(const std::chrono::duration<R, P>& rhs) {
  return std::chrono::duration_cast<D>(rhs);
}

// ------------------------------------------------------------------------------------------------

// Alias units
using TimeUnit = std::variant<picoseconds, nanoseconds, microseconds>;
using VoltageUnit = std::variant<millivolts, volts>;
using CurrentUnit = std::variant<microamps, milliamps, amps, kiloamps>;
using PowerUnit = std::variant<picowatts, nanowatts, microwatts, milliwatts>;
using CapacitanceUnit = std::variant<picofarads, femtofarads>;
using ResistanceUnit = std::variant<ohms, kiloohms>;

// Dump units
std::string dump_time_unit(const TimeUnit&);
std::string dump_voltage_unit(const VoltageUnit&);
std::string dump_current_unit(const CurrentUnit&);
std::string dump_power_unit(const PowerUnit&);
std::string dump_capacitance_unit(const CapacitanceUnit&);
std::string dump_resistance_unit(const ResistanceUnit&);

// Make units
std::optional<TimeUnit> make_time_unit(const std::string&);
std::optional<VoltageUnit> make_voltage_unit(const std::string&);
std::optional<CurrentUnit> make_current_unit(const std::string&);
std::optional<PowerUnit> make_power_unit(const std::string&);
std::optional<CapacitanceUnit> make_capacitance_unit(const std::string&);
std::optional<ResistanceUnit> make_resistance_unit(const std::string&);

// Unit operator
float divide_time_unit(const TimeUnit&, const TimeUnit&);
float divide_voltage_unit(const VoltageUnit&, const VoltageUnit&);
float divide_current_unit(const CurrentUnit&, const CurrentUnit&);
float divide_power_unit(const PowerUnit&, const PowerUnit&);
float divide_capacitance_unit(const CapacitanceUnit&, const CapacitanceUnit&);
float divide_resistance_unit(const ResistanceUnit&, const ResistanceUnit&);

};  // end of namespace ot ------------------------------------------------------------------------


#endif
