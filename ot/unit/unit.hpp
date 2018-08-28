#ifndef OT_UNIT_UNIT_HPP_
#define OT_UNIT_UNIT_HPP_

#include <ot/headerdef.hpp>
#include <ot/traits.hpp>

namespace ot {

using ohm_t    = units::impedance::ohm_t;       // resistance unit (Ohm)
using farad_t  = units::capacitance::farad_t;   // capacitance unit (F)
using second_t = units::time::second_t;         // time unit (s)
using volt_t   = units::voltage::volt_t;        // voltage unit (V)
using ampere_t = units::current::ampere_t;      // current unit (A)
using watt_t   = units::power::watt_t;          // power unit (W)

std::optional<second_t> make_time_unit(std::string_view);
std::optional<ohm_t> make_resistance_unit(std::string_view);
std::optional<volt_t> make_voltage_unit(std::string_view);
std::optional<ampere_t> make_current_unit(std::string_view); 
std::optional<watt_t> make_power_unit(std::string_view);
std::optional<farad_t> make_capacitance_unit(std::string_view);



};  // end of namespace ot ------------------------------------------------------------------------


#endif



