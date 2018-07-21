#include <ot/spef/spef.hpp>

namespace ot::spef {

// Procedure: scale_capacitance
void Net::scale_capacitance(float s) {
  lcap *= s;
  for(auto& c : caps) {
    std::get<1>(c) *= s;
  }
}

// Procedure: scale_resistance
void Net::scale_resistance(float s) {
  for(auto& r : ress) {
    std::get<2>(r) *= s;
  }
}

// Operator <<
std::ostream& operator << (std::ostream& os, const Net& net) {
    
  os << net.name << ' ' << net.lcap << '\n';
  
  os << "*CONN\n";
  for(const auto& p : net.pins) {
    os << p << '\n';
  }
  
  os << "*CAP\n";
  for(const auto& [key, cap] : net.caps) {
    os << key << " " << cap << '\n';
  }

  os << "*RES\n";
  for(const auto& [n1, n2, res] : net.ress) {
    os << n1 << ' ' << n2 << ' ' << res << '\n';
  }
  os << '\n';

  return os;
}

// ------------------------------------------------------------------------------------------------

// Operator <<
std::ostream& operator << (std::ostream& os, const Spef& spef) {
  for(const auto& net : spef.nets) {
    os << net;
  }
  return os;
}


// Procedure: read_spef
// *T_UNIT 1 PS
// *C_UNIT 1 FF
// *R_UNIT 1 KOHM
// *L_UNIT 1 UH
void Spef::read(const std::filesystem::path& path) {

  auto tokens = tokenize(path);

  for(size_t i=0; i<tokens.size(); ++i) {
    
    // time unit section
    if(tokens[i] == "*T_UNIT") {
      if(i+2 >= tokens.size()) {
        OT_LOGF("syntax error in *T_UNIT section");
      }
      time_unit = make_time_unit(tokens[i+1] + to_lower(tokens[i+2]));
      i += 2;
    }
    // capacitance unit
    else if(tokens[i] == "*C_UNIT") {
      if(i+2 >= tokens.size()) {
        OT_LOGF("syntax error in *C_UNIT section");
      }
      capacitance_unit = make_capacitance_unit(tokens[i+1] + to_lower(tokens[i+2]));
    }
    // resistance unit
    else if(tokens[i] == "*R_UNIT") {
      if(i+2 >= tokens.size()) {
        OT_LOGF("syntax error in *R_UNIT section");
      }
      resistance_unit = make_resistance_unit(tokens[i+1] + to_lower(tokens[i+2]));
    }
    // Net section
    else if(tokens[i] == "*D_NET") {

      if(i+2 >= tokens.size()) {
        OT_LOGF("syntax error in *NET section");
      }

      Net net {std::move(tokens[i+1]), std::stof(tokens[i+2])}; 

      i += 2;

      while(++i < tokens.size()) {
        // *CONN section
        if(tokens[i] == "*CONN") {
          auto j = i;
          while(++i < tokens.size()) {
            if(tokens[i][1] != 'P' && tokens[i][1] != 'I') {
              i = j;
              break;
            }
            
            if(i+2 >= tokens.size()) {
              OT_LOGF("syntax error in *CONN section");
            }

            net.pins.push_back(std::move(tokens[++i]));
            ++i;  // skip the pin direction
            j = i;
          }
        }
        // *CAP section.
        else if(tokens[i] == "*CAP") {
          auto j = i;
          while(++i < tokens.size()) {
            if(!::isdigit(tokens[i][0])) {
              i = j;
              break;
            }

            if(i+2 >= tokens.size()) {
              OT_LOGF("syntax error in *CAP section");
            }

            net.caps.emplace_back(std::forward_as_tuple(
              std::move(tokens[i+1]), 
              std::stof(tokens[i+2])
            ));
            i += 2;
            j = i;
          }
        }
        // *RES section.
        else if(tokens[i] == "*RES") {
          auto j = i;
          while(++i < tokens.size()) {
            if(!::isdigit(tokens[i][0])) {
              i = j;
              break;
            }

            if(i+3 >= tokens.size()) {
              OT_LOGF("syntax error in *RES section");
            }

            net.ress.emplace_back(std::forward_as_tuple(
              std::move(tokens[i+1]),
              std::move(tokens[i+2]),
              std::stof(tokens[i+3])
            ));
            i += 3;
            j = i;
          }
        }
        // *END section.
        else if(tokens[i] == "*END") {
          nets.push_back(std::move(net));
          break;
        }
        else {
          OT_LOGF("unexpected token ", tokens[i], " in *NET section");
        }
      }
    }
  }
}
  
// Procedure: to_capacitance_unit
void Spef::to_capacitance_unit(const CapacitanceUnit& unit) {
   
  float s = (capacitance_unit) ? divide_capacitance_unit(*capacitance_unit, unit) : 1.0f;

  if(capacitance_unit = unit; std::fabs(s - 1.0f) < 1e-6) {
    return;
  }

  for(auto& n : nets) {
    n.scale_capacitance(s);
  }
}

// Procedure: to_resistance_unit
void Spef::to_resistance_unit(const ResistanceUnit& unit) {
   
  float s = (resistance_unit) ? divide_resistance_unit(*resistance_unit, unit) : 1.0f;

  if(resistance_unit = unit; std::fabs(s - 1.0f) < 1e-6) {
    return;
  }

  for(auto& n : nets) {
    n.scale_resistance(s);
  }
}

};  // end of namespace ot. -----------------------------------------------------------------------



