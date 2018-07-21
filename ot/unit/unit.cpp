#include <ot/unit/unit.hpp>

namespace ot {

// Function: make_time_unit
std::optional<TimeUnit> make_time_unit(const std::string& str) {

  if(std::smatch pieces; std::regex_match(str, pieces, unit_regex)) {
  
    if(pieces.size() != 4 || pieces[3] != "s") {
      return std::nullopt;
    }
    else if(auto v=std::stof(pieces[1].str()); pieces[2].str() == "p") {
      return picoseconds(v);
    }
    else if(pieces[2].str() == "n") {
      return nanoseconds(v);
    }
    else if(pieces[2].str() == "u") {
      return microseconds(v);
    }
    else return std::nullopt;
  }
  else return std::nullopt;
}

// Function: make_voltage_unit
std::optional<VoltageUnit> make_voltage_unit(const std::string& str) {

  if(std::smatch pieces; std::regex_match(str, pieces, unit_regex)) {

    if(pieces.size() != 4 || pieces[3] != "V") {
      return std::nullopt; 
    }
    else if(auto v = std::stof(pieces[1].str()); pieces[2].str() == "") {
      return volts(v);
    }
    else if(pieces[2].str() == "m") {
      return millivolts(v);
    }
    else {
      return std::nullopt;
    }
  }
  else {
    return std::nullopt;
  }
}

// Function: make_current_unit
std::optional<CurrentUnit> make_current_unit(const std::string& str) {

  if(std::smatch pieces; std::regex_match(str, pieces, unit_regex)) {

    if(pieces.size() != 4 || pieces[3] != "A") {
      return std::nullopt; 
    }
    else if(auto v = std::stof(pieces[1].str()); pieces[2].str() == "") {
      return amps(v);
    }
    else if(pieces[2].str() == "m") {
      return milliamps(v);
    }
    else if(pieces[2].str() == "u") {
      return microamps(v);
    }
    else if(pieces[2].str() == "k") {
      return kiloamps(v);
    }
    else {
      return std::nullopt;
    }
  }
  else {
    return std::nullopt;
  }
}

// Function: make_power_unit
std::optional<PowerUnit> make_power_unit(const std::string& str) {

  if(std::smatch pieces; std::regex_match(str, pieces, unit_regex)) {

    if(pieces.size() != 4 || pieces[3].str() != "W") {
      return std::nullopt; 
    }
    else if(auto v = std::stof(pieces[1].str()); pieces[2].str() == "m") {
      return milliwatts(v);
    }
    else if(pieces[2].str() == "u") {
      return microwatts(v);
    }
    else if(pieces[2].str() == "n") {
      return nanowatts(v);
    }
    else if(pieces[2].str() == "p") {
      return picowatts(v);
    }
    else {
      return std::nullopt;
    }
  }
  else {
    return std::nullopt;
  }
}

// Function: make_capacitance_unit
std::optional<CapacitanceUnit> make_capacitance_unit(const std::string& str) {
  
  if(std::smatch pieces; std::regex_match(str, pieces, unit_regex)) {

    if(pieces.size() != 4 || (pieces[3].str() != "f" && pieces[3].str() != "F")) {
      return std::nullopt; 
    }
    else if(auto v = std::stof(pieces[1].str()); pieces[2].str() == "f") {
      return femtofarads(v);
    }
    else if(pieces[2].str() == "p") {
      return picofarads(v);
    }
    else {
      return std::nullopt;
    }
  }
  else {
    return std::nullopt;
  }

}

// Function: make_resistance_unit
std::optional<ResistanceUnit> make_resistance_unit(const std::string& str) {

  if(std::smatch pieces; std::regex_match(str, pieces, unit_regex)) {

    if(pieces.size() != 4 || (pieces[3].str() != "ohm" && pieces[3].str() != "Ohm")) {
      return std::nullopt; 
    }
    else if(auto v = std::stof(pieces[1].str()); pieces[2].str() == "") {
      return ohms(v);
    }
    else if(pieces[2].str() == "k") {
      return kiloohms(v);
    }
    else {
      return std::nullopt;
    }
  }
  else {
    return std::nullopt;
  }
}

// ------------------------------------------------------------------------------------------------

// Function: divide_time_unit
float divide_time_unit(const TimeUnit& num, const TimeUnit& den) {
  return std::visit(Functors{[&] (const auto& n) {
    return std::visit(Functors{[&] (const auto& d) {
      return n / d;
    }}, den);
  }}, num);
}

// Function: divide_voltage_unit
float divide_voltage_unit(const VoltageUnit& num, const VoltageUnit& den) {
  return std::visit(Functors{[&] (const auto& n) {
    return std::visit(Functors{[&] (const auto& d) {
      return n / d;
    }}, den);
  }}, num);
}

// Function: divide_current_unit
float divide_current_unit(const CurrentUnit& num, const CurrentUnit& den) {
  return std::visit(Functors{[&] (const auto& n) {
    return std::visit(Functors{[&] (const auto& d) {
      return n / d;
    }}, den);
  }}, num);
}

// Function: divide_power_unit
float divide_power_unit(const PowerUnit& num, const PowerUnit& den) {
  return std::visit(Functors{[&] (const auto& n) {
    return std::visit(Functors{[&] (const auto& d) {
      return n / d;
    }}, den);
  }}, num);
}

// Function: divide_capacitance_unit
float divide_capacitance_unit(
  const CapacitanceUnit& num, 
  const CapacitanceUnit& den
) {
  return std::visit(Functors{[&] (const auto& n) {
    return std::visit(Functors{[&] (const auto& d) {
      return n / d;
    }}, den);
  }}, num);
}

// Function: divide_resistance_unit
float divide_resistance_unit(
  const ResistanceUnit& num, 
  const ResistanceUnit& den
) {
  return std::visit(Functors{[&] (const auto& n) {
    return std::visit(Functors{[&] (const auto& d) {
      return n / d;
    }}, den);
  }}, num);
}


// ------------------------------------------------------------------------------------------------

// Function: dump_time_unit
std::string dump_time_unit(const TimeUnit& unit) {

  std::ostringstream oss;

  std::visit(Functors{
    [&] (const picoseconds&  d) { oss << d.count() << "ps"; },
    [&] (const nanoseconds&  d) { oss << d.count() << "ns"; },
    [&] (const microseconds& d) { oss << d.count() << "us"; }
  }, unit);

  return oss.str();
}

// Function: dump_voltage_unit
std::string dump_voltage_unit(const VoltageUnit& unit) {

  std::ostringstream oss;

  std::visit(Functors{
    [&] (const volts& d)      { oss << d.count() << "V"; },
    [&] (const millivolts& d) { oss << d.count() << "mV"; }
  }, unit); 

  return oss.str();
}

// Function: dump_current_unit
std::string dump_current_unit(const CurrentUnit& unit) {
  
  std::ostringstream oss;

  std::visit(Functors{
    [&] (const kiloamps& d)  { oss << d.count() << "kA"; },
    [&] (const amps& d)      { oss << d.count() << "A";  },
    [&] (const milliamps& d) { oss << d.count() << "mA"; },
    [&] (const microamps& d) { oss << d.count() << "uA"; }
  }, unit);

  return oss.str();
}

// Function: dump_power_unit
std::string dump_power_unit(const PowerUnit& unit) {

  std::ostringstream oss;

  std::visit(Functors{
    [&] (const picowatts& d)  { oss << d.count() << "pW"; }, 
    [&] (const nanowatts& d)  { oss << d.count() << "nW"; },
    [&] (const microwatts& d) { oss << d.count() << "uW"; },
    [&] (const milliwatts& d) { oss << d.count() << "mW"; } 
  }, unit);

  return oss.str();
}

// Function: dump_capacitance_unit
std::string dump_capacitance_unit(const CapacitanceUnit& unit) {
  
  std::ostringstream oss;

  std::visit(Functors{
    [&] (const picofarads& d)  { oss << d.count() << "pf"; },    
    [&] (const femtofarads& d) { oss << d.count() << "ff"; }
  }, unit);

  return oss.str();
}

// Function: dump_resistance_unit
std::string dump_resistance_unit(const ResistanceUnit& unit) {

  std::ostringstream oss;

  std::visit(Functors{
    [&] (const ohms& d)     { oss << d.count() << "ohm";  },
    [&] (const kiloohms& d) { oss << d.count() << "kohm"; }
  }, unit);

  return oss.str();
}

};  // end of namespace ot. -----------------------------------------------------------------------





