#include <ot/liberty/cellpin.hpp>

namespace ot {

// Function: to_string
std::string to_string(CellpinDirection d) {  
  switch(d) {
    case CellpinDirection::INPUT:
      return "input";
    break;

    case CellpinDirection::OUTPUT:
      return "output";
    break;

    case CellpinDirection::INOUT:
      return "inout";
    break;

    case CellpinDirection::INTERNAL:
      return "internal";
    break;
    
    default:
      return "undefined";
    break;
  }
}

// Procedure: scale_time
void Cellpin::scale_time(float s) {
  
  if(max_transition) {
    max_transition = max_transition.value() * s;
  }

  if(min_transition) {
    min_transition = min_transition.value() * s;
  }

  for(auto& timing : timings) {
    timing.scale_time(s);
  }
}

// Procedure: scale_capacitance
void Cellpin::scale_capacitance(float s) {
  
  if(capacitance) {
    capacitance = capacitance.value() * s;
  }

  if(min_capacitance) {
    min_capacitance = min_capacitance.value() * s;
  }
  
  if(max_capacitance) {
    max_capacitance = max_capacitance.value() * s;
  }

  if(fall_capacitance) {
    fall_capacitance = fall_capacitance.value() * s;
  }

  if(rise_capacitance) {
    rise_capacitance = rise_capacitance.value() * s;
  }

  for(auto& timing : timings) {
    timing.scale_capacitance(s);
  }
}

// Function: isomorphic_timing
const Timing* Cellpin::isomorphic_timing(const Timing& rhs) const {
  for(const auto& timing : timings) {
    if(rhs.isomorphic(timing)) {
      return &timing;
    }
  }
  return nullptr;
}

// Operator
std::ostream& operator << (std::ostream& os, const Cellpin& p) {

  // Write the cellpin name.
  os << "  pin (\"" << p.name << "\") {\n";
    
  // Write the pin direction.
  if(p.direction) {
    os << "    direction : " << to_string(*p.direction) << ";\n";
  }
  
  // Write the pin capacitance.
  if(p.capacitance) {
    os << "    capacitance : " << (*p.capacitance) << ";\n";
  }

  // Write the clock flag.
  if(p.is_clock) {
    os << "    clock : " << (*(p.is_clock) ? "true" : "false") << ";\n";
  }
  
  if(p.max_capacitance) {
    os << "    max_capacitance : " << *(p.max_capacitance) << ";\n";
  }
  
  if(p.min_capacitance) {
    os << "    min_capacitance : " << *(p.min_capacitance) << ";\n";
  }

  if(p.rise_capacitance) {
    os << "    rise_capacitance : " << *p.rise_capacitance << ";\n";
  }
  
  if(p.fall_capacitance) {
    os << "    fall_capacitance : " << *p.fall_capacitance << ";\n";
  }

  if(p.max_transition) {
    os << "    max_transition : " << *(p.max_transition) << ";\n";
  }

  if(p.min_transition) {
    os << "    min_transition : " << *(p.min_transition) << ";\n";
  }
  
  if(p.fanout_load) {
    os << "    fanout_load : " << *p.fanout_load << ";\n";
  }

  if(p.max_fanout) {
    os << "    max_fanout : " << *p.max_fanout << ";\n";
  }
  
  if(p.min_fanout) {
    os << "    min_fanout : " << *p.min_fanout << ";\n";
  }

  // Write the timing.
  for(const auto& timing : p.timings) {
    os << timing;
  }

  // Write the ending group symbol.
  os << "  }\n";

  return os;
}


};  // end of namespace ot ------------------------------------------------------------------------


