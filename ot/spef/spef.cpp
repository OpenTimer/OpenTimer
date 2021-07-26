#include <ot/spef/spef.hpp>

/*namespace ot::spef {

// Function: is_keyword
bool is_keyword(const std::string& str) {
  return keywords.find(str) != keywords.end();
}

// Function: to_string
std::string to_string(ConnectionType t) {
  switch(t) {
    case ConnectionType::INTERNAL:
      return "*I";
    break;

    case ConnectionType::EXTERNAL:
      return "*P";
    break;

    default:
      assert(false); 
    break;
  }
}

// Function: to_string
std::string to_string(ConnectionDirection d) {
  switch(d) {
    case ConnectionDirection::INPUT:
      return "I";
    break;

    case ConnectionDirection::OUTPUT:
      return "O";
    break;

    case ConnectionDirection::INOUT:
      return "B";
    break;

    default:
      assert(false);
    break;
  }
}

// Function: unmap
// TODO
void unmap(const std::unordered_map<std::string, std::string>& map, std::string& name) {
  
  if(map.empty()) {
    return;
  }

}

// ------------------------------------------------------------------------------------------------

// Constructor
Connection::Connection(const std::string& n, ConnectionType t, ConnectionDirection d) :
  name {n},
  type {t},
  direction {d}  {
}

// ------------------------------------------------------------------------------------------------

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
    
  os << "*D_NET " << net.name << ' ' << net.lcap << '\n';
  
  os << "*CONN\n";
  for(const auto& c : net.connections) {
    os << to_string(c.type) << ' ' 
       << c.name << ' ' 
       << to_string(c.direction) << '\n';
  }
  
  auto cap_counter {0};
  os << "*CAP\n";
  for(const auto& [key, cap] : net.caps) {
    os << ++cap_counter << ' ' << key << ' ' << cap << '\n';
  }

  auto res_counter {0};
  os << "*RES\n";
  for(const auto& [n1, n2, res] : net.ress) {
    os << ++res_counter << ' ' << n1 << ' ' << n2 << ' ' << res << '\n';
  }
  os << "*END\n";

  return os;
}

// ------------------------------------------------------------------------------------------------

// Operator <<
std::ostream& operator << (std::ostream& os, const Spef& spef) {

  // header

  // Name map section
  if(!spef.name_map.empty())  {
    os << "*NAME_MAP\n";
    for(const auto& [k, v] : spef.name_map) {
      os << k << ' ' << v << '\n';
    }
    os << '\n';
  }

  // Internal section
  for(size_t i=0; i<spef.nets.size(); ++i) {
    if(i) os << '\n';
    os << spef.nets[i];
  }
  return os;
}


// Procedure: read_spef
// *T_UNIT 1 PS
// *C_UNIT 1 FF
// *R_UNIT 1 KOHM
// *L_UNIT 1 UH
void Spef::read(const std::filesystem::path& path) {
  
  OT_LOGE_RIF(
    path.empty() || !std::filesystem::exists(path),
    "spef ", path, " doesn't exist"
  );

  OT_LOGI("loading spef ", path, " ...");

  auto tokens = tokenize(path);

  for(size_t i=0; i<tokens.size(); ++i) {
    
    // divider
    if(tokens[i] == "*DIVIDER") {
      if(i+1 >= tokens.size() || tokens[i+1].size() != 1) {
        OT_LOGF("syntax error in *DIVIDER section");
      }
      divider = tokens[i+1][0];
      i += 1;
    }
    // delimiter
    else if(tokens[i] == "*DELIMITER") {
      if(i+1 >= tokens.size() || tokens[i+1].size() != 1) {
        OT_LOGF("syntax error in *DELIMITER section");
      }

      delimiter = tokens[i+1][0];
      i += 1;
    }
    // time unit section
    else if(tokens[i] == "*T_UNIT") {
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
    // name mapping section
    else if(tokens[i] == "*NAME_MAP") {
      while(i+2 < tokens.size()) {
        if(is_keyword(tokens[i+1]) || is_keyword(tokens[i+2])) {
          break;
        }
        name_map.try_emplace(std::move(tokens[i+1]), std::move(tokens[i+2]));
        i += 2;
      }
    }
    // Net section
    else if(tokens[i] == "*D_NET") {

      if(i+2 >= tokens.size()) {
        OT_LOGF("syntax error in *D_NET section");
      }

      Net net {std::move(tokens[i+1]), std::stof(tokens[i+2])}; 

      i += 2;

      while(++i < tokens.size()) {
        // *CONN section
        // *P external_connection direction {conn_attr} | 
        // *I internal_connection direction {conn_attr}
        if(tokens[i] == "*CONN") {
          auto j = i;
          while(++i < tokens.size()) {

            if(tokens[i] != "*P" && tokens[i] != "*I") {
              i = j;
              break;
            }
            
            if(i+2 >= tokens.size()) {
              OT_LOGF("syntax error in *CONN section");
            }
            
            auto t = (tokens[i] == "*P") ? ConnectionType::EXTERNAL : 
                                           ConnectionType::INTERNAL ;

            if(auto& k = tokens[i+2]; k != "I" && k != "O" && k != "B") {
              OT_LOGF("syntax error in parsing direction ", k);
            }
            
            auto d = (tokens[i+2] == "I") ? ConnectionDirection::INPUT  :
                    ((tokens[i+2] == "O") ? ConnectionDirection::OUTPUT : 
                                            ConnectionDirection::INOUT);
            
            net.connections.emplace_back(tokens[i+1], t, d);

            i += 2;

            // tyr to read attribute if any
            if(i+1 < tokens.size()) {
              while(tokens[i+1] == "*C" || tokens[i+1] == "*L" || tokens[i+1] == "*D") {
                // coordinate
                if(tokens[i+1] == "*C") {
                  if(i+3 >= tokens.size()) {
                    OT_LOGF("syntax error in parsing attribute *C");
                  }
                  // TODO
                  i += 3;
                }
                // load capacitance
                else if(tokens[i+1] == "*L") {
                  if(i+2 >= tokens.size()) {
                    OT_LOGF("syntax error in parsing attribute *L");
                  }
                  // TODO
                  i += 2;
                }
                // driving cell
                else {
                  if(i+2 >= tokens.size()) {
                    OT_LOGF("syntax error in parsing attribute *D");
                  }
                  // TODO
                  i += 2;
                }
              }
            }

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
          OT_LOGF("unexpected token ", tokens[i], " in *D_NET section");
        }
      }
    }
    else {
      //OT_LOGW("unexpected token ", tokens[i]);
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

};  // end of namespace ot. ----------------------------------------------------------------------- */



