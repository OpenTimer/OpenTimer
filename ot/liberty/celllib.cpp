#include <ot/liberty/celllib.hpp>

namespace ot {

// Function: to_string
std::string to_string(DelayModel m) {
  switch(m) {
    case DelayModel::GENERIC_CMOS:
      return "generic_cmos";
    break;

    case DelayModel::TABLE_LOOKUP:
      return "table_lookup";
    break;

    case DelayModel::CMOS2:
      return "cmos2";
    break;

    case DelayModel::PIECEWISE_CMOS:
      return "piecewise_cmos";
    break;

    case DelayModel::DCM:
      return "dcm";
    break;

    case DelayModel::POLYNOMIAL:
      return "polynomial";
    break;

    default:
      return "undefined";
    break;
  }
}

// Function: lut_template
const LutTemplate* Celllib::lut_template(const std::string& name) const {
  if(auto itr = lut_templates.find(name); itr == lut_templates.end()) {
    return nullptr;
  }
  else {
    return &(itr->second);
  }
}

// Function: lut_template
LutTemplate* Celllib::lut_template(const std::string& name) {
  if(auto itr = lut_templates.find(name); itr == lut_templates.end()) {
    return nullptr;
  }
  else {
    return &(itr->second);
  }
}

// Function: cell
const Cell* Celllib::cell(const std::string& name) const {
  if(auto itr = cells.find(name); itr == cells.end()) {
    return nullptr;
  }
  else {
    return &(itr->second);
  }
}

// Function: cell
Cell* Celllib::cell(const std::string& name) {
  if(auto itr = cells.find(name); itr == cells.end()) {
    return nullptr;
  }
  else {
    return &(itr->second);
  }
}

// Function: _extract_lut_template
LutTemplate Celllib::_extract_lut_template(token_iterator& itr, const token_iterator end) {

  LutTemplate lt;

  if(itr=on_next_parentheses(
    itr, 
    end, 
    [&] (auto& name) mutable { lt.name = std::move(name); }); itr == end) {
    OT_LOGF("can't find lut template name");
  }
  
  // Extract the lut template group
  if(itr = std::find(itr, end, "{"); itr == end) {
    OT_LOGF("can't find lut template group brace '{'");
  }

  //std::cout << lt.name << std::endl;

  int stack = 1;
  
  while(stack && ++itr != end) {
    
    // variable 1
    if(*itr == "variable_1") {                       // Read the variable.

      if(++itr == end) {
        OT_LOGF("variable_1 error in lut template ", lt.name);
      }

      if(auto vitr = lut_vars.find(*itr); vitr != lut_vars.end()) {
        lt.variable1 = vitr->second;
      }
      else {
        OT_LOGW("unexpected lut template variable ", *itr);
      }
    }
    // variable 2
    else if(*itr == "variable_2") { 

      if(++itr == end) {
        OT_LOGF("variable_2 error in lut template ", lt.name);
      }

      if(auto vitr = lut_vars.find(*itr); vitr != lut_vars.end()) {
        lt.variable2 = vitr->second;
      }
      else {
        OT_LOGW("unexpected lut template variable ", *itr);
      }
    }
    // index_1
    else if(*itr == "index_1") {
      itr = on_next_parentheses(itr, end, [&] (auto& str) { lt.indices1.push_back(std::stof(str)); });
    }
    // index_2
    else if(*itr == "index_2") {
      itr = on_next_parentheses(itr, end, [&] (auto& str) { lt.indices2.push_back(std::stof(str)); });
    }
    else if(*itr == "}") {
      stack--;
    }
    else if(*itr == "{") {
      stack++;
    }
    else {
    }
  }
  
  if(stack != 0 || *itr != "}") {
    OT_LOGF("can't find lut template group brace '}'");
  }

  return lt;
}

// Function: _extract_lut
Lut Celllib::_extract_lut(token_iterator& itr, const token_iterator end) {
  
  Lut lut;
  
  if(itr=on_next_parentheses(
    itr, 
    end, 
    [&] (auto& name) mutable { lut.name = std::move(name); }); itr == end) {
    OT_LOGF("can't find lut template name");
  }
  
  // Set up the template
  lut.lut_template = lut_template(lut.name);
  
  // Extract the lut group
  if(itr = std::find(itr, end, "{"); itr == end) {
    OT_LOGF("group brace '{' error in lut ", lut.name);
  }

  int stack = 1;

  size_t size1 = 1;
  size_t size2 = 1;

  while(stack && ++itr != end) {
    
    if(*itr == "index_1") { 
      itr = on_next_parentheses(itr, end, [&] (auto& v) mutable {
        lut.indices1.push_back(std::stof(v));
      });

      if(lut.indices1.size() == 0) {
        OT_LOGF("syntax error in ", lut.name, " index_1");
      }

      size1 = lut.indices1.size();
    }
    else if(*itr == "index_2") {
      itr = on_next_parentheses(itr, end, [&] (auto& v) mutable {
        lut.indices2.push_back(std::stof(v));
      });

      if(lut.indices2.size() == 0) {
        OT_LOGF("syntax error in ", lut.name, " index_2");
      }
      
      size2 = lut.indices2.size();
    }
    else if(*itr == "values") {

      if(lut.indices1.empty()) {
        if(size1 != 1) {
          OT_LOGF("empty indices1 in non-scalar lut ", lut.name);
        }
        lut.indices1.resize(size1);
      }

      if(lut.indices2.empty()){
        if(size2 != 1) {
          OT_LOGF("empty indices2 in non-scalar lut ", lut.name);
        }
        lut.indices2.resize(size2);
      }

      lut.table.resize(size1*size2);

      int id {0};
      itr = on_next_parentheses(itr, end, [&] (auto& v) mutable {
        lut.table[id++] = std::stof(v);
      });
    }
    else if(*itr == "}") {
      stack--;
    }
    else if(*itr == "{") {
      stack++;
    }
    else {
    }
  }

  if(stack != 0 || *itr != "}") {
    OT_LOGF("group brace '}' error in lut ", lut.name);
  }

  return lut;
}

// Function: _extract_timing
Timing Celllib::_extract_timing(token_iterator& itr, const token_iterator end) {

  Timing timing;

  // Extract the lut template group
  if(itr = std::find(itr, end, "{"); itr == end) {
    OT_LOGF("can't find group brace '{' in timing");
  }
  
  int stack = 1;
  
  while(stack && ++itr != end) {
    
    if (*itr == "cell_fall") {        
      timing.cell_fall = _extract_lut(itr, end);
    } 
    else if (*itr == "cell_rise") {                  // Rise delay.
      timing.cell_rise = _extract_lut(itr, end);
    } 
    else if (*itr == "fall_transition") {            // Fall slew.
      timing.fall_transition = _extract_lut(itr, end);
    } 
    else if (*itr == "rise_transition") {            // Rise slew.
      timing.rise_transition = _extract_lut(itr, end);
    } 
    else if (*itr == "rise_constraint") {            // FF rise constraint.
      timing.rise_constraint = _extract_lut(itr, end);
    }
    else if(*itr == "fall_constraint") {             // FF fall constraint.
      timing.fall_constraint = _extract_lut(itr, end);
    }
    else if(*itr == "timing_sense") {               // Read the timing sense.

      OT_LOGF_IF(++itr == end, "syntex error in timing_sense");

      if(*itr == "negative_unate") {
        timing.sense = TimingSense::NEGATIVE_UNATE;       // Negative unate.
      }
      else if(*itr == "positive_unate") {            // Positive unate.
        timing.sense = TimingSense::POSITIVE_UNATE;
      }
      else if(*itr == "non_unate") {                 // Non unate.
        timing.sense = TimingSense::NON_UNATE;
      }
      else {
        OT_LOGF("unexpected timing sense ", *itr);
      }
    } 
    else if(*itr == "timing_type") {
      
      if(++itr == end) {
        OT_LOGF("syntax error in timing_type");
      }

      if(auto titr = timing_types.find(*itr); titr != timing_types.end()) {
        timing.type = titr->second;
      }
      else {
        OT_LOGW("unexpected timing type ", std::quoted(*itr));
      }
    }
    else if (*itr == "related_pin") {
      
      if(++itr == end) {
        OT_LOGF("syntax error in related_pin");
      }

      timing.related_pin = std::move(*itr);
    }
    else if(*itr == "}") {
      stack--;
    }
    else if(*itr == "{") {
      stack++;
    }
    else {
    }
  }

  if(stack != 0 || *itr != "}") {
    OT_LOGF("can't find group brace '}' in timing");
  }

  return timing;
}

// Functoin: _extract_cellpin
Cellpin Celllib::_extract_cellpin(token_iterator& itr, const token_iterator end) {

  Cellpin cellpin;
  
  if(itr=on_next_parentheses(
    itr, 
    end, 
    [&] (auto& name) mutable { cellpin.name = std::move(name); }); itr == end) {
    OT_LOGF("can't find cellpin name");
  }
  
  // Extract the lut template group
  if(itr = std::find(itr, end, "{"); itr == end) {
    OT_LOGF("can't find group brace '{' in cellpin ", cellpin.name);
  }

  //std::cout << "  -->" << cellpin.name << std::endl;

  int stack = 1;
  
  while(stack && ++itr != end) {
    
    if(*itr == "direction") {

      if(++itr == end) {
        OT_LOGF("can't get the direction in cellpin ", cellpin.name);
      }

      if(auto ditr = cellpin_directions.find(*itr); ditr != cellpin_directions.end()) {
        cellpin.direction = ditr->second;
      }
      else {
        OT_LOGW("unexpected cellpin direction ", *itr);
      }
    }
    else if(*itr == "capacitance") {
      OT_LOGF_IF(++itr == end, "can't get the capacitance in cellpin ", cellpin.name);
      cellpin.capacitance = std::stof(*itr);
    }
    else if(*itr == "max_capacitance") {
      OT_LOGF_IF(++itr == end, "can't get the max_capacitance in cellpin ", cellpin.name);
      cellpin.max_capacitance = std::stof(*itr);
    }
    else if(*itr == "min_capacitance") {  
      OT_LOGF_IF(++itr == end, "can't get the min_capacitance in cellpin ", cellpin.name);
      cellpin.min_capacitance = std::stof(*itr);
    }
    else if(*itr == "max_transition") {        
      OT_LOGF(++itr == end, "can't get the max_transition in cellpin ", cellpin.name);
      cellpin.max_transition = std::stof(*itr);
    }
    else if(*itr == "min_transition") {          
      OT_LOGF_IF(++itr == end, "can't get the min_transition in cellpin ", cellpin.name);
      cellpin.min_transition = std::stof(*itr);
    }
    else if(*itr == "fall_capacitance") {
      OT_LOGF_IF(++itr == end, "can't get fall_capacitance in cellpin ", cellpin.name);
      cellpin.fall_capacitance = std::stof(*itr);
    }
    else if(*itr == "rise_capacitance") {
      OT_LOGF_IF(++itr == end, "can't get rise_capacitance in cellpin ", cellpin.name);
      cellpin.rise_capacitance = std::stof(*itr);
    }
    else if(*itr == "clock") {  
      OT_LOGF_IF(++itr == end, "can't get the clock status in cellpin ", cellpin.name);
      cellpin.is_clock = (*itr == "true") ? true : false;
    }
    else if(*itr == "original_pin") {
      OT_LOGF_IF(++itr ==end, "can't get the original pin in cellpin ", cellpin.name);
      cellpin.original_pin = std::move(*itr);
    }
    else if(*itr == "timing") {
      cellpin.timings.push_back(_extract_timing(itr, end));
    }
    else if(*itr == "}") {
      stack--;
    }
    else if(*itr == "{") {
      stack++;
    }
    else {
    }
  }
  
  if(stack != 0 || *itr != "}") {
    OT_LOGF("can't find group brace '}' in cellpin ", cellpin.name);
  }

  return cellpin;
}

// Function: _extract_cell
Cell Celllib::_extract_cell(token_iterator& itr, const token_iterator end) {
  
  Cell cell;
  
  if(itr=on_next_parentheses(
    itr, 
    end, 
    [&] (auto& name) mutable { cell.name = std::move(name); }); itr==end) {
    OT_LOGF("can't find cell name");
  }

  // Extract the lut template group
  if(itr = std::find(itr, end, "{"); itr == end) {
    OT_LOGF("can't find group brace '{' in cell ", cell.name);
  }

  //std::cout << cell.name << std::endl;

  int stack = 1;
  
  while(stack && ++itr != end) {
    
    if(*itr == "cell_leakage_power") {               // Read the leakage power.
      OT_LOGF_IF(++itr == end, "can't get leakage power in cell ", cell.name);
      cell.leakage_power = std::stof(*itr);
    }
    else if(*itr == "cell_footprint") {              // Read the footprint.
      OT_LOGF_IF(++itr == end, "can't get footprint in cell ", cell.name);
      cell.cell_footprint = std::move(*itr);
    }
    else if(*itr == "area") {                        // Read the area.
      OT_LOGF_IF(++itr == end, "can't get area in cell ", cell.name);
      cell.area = std::stof(*itr);
    }
    else if(*itr == "pin") {                         // Read the cell pin group.
      auto pin = _extract_cellpin(itr, end);
      cell.cellpins[pin.name] = std::move(pin);
    }
    else if(*itr == "}") {
      stack--;
    }
    else if(*itr == "{") {
      stack++;
    }
    else {
      //OT_LOGW("unexpected token ", *itr);
    }
  }

  if(stack != 0 || *itr != "}") {
    OT_LOGF("can't find group brace '}' in cell ", cell.name);
  }

  return cell;
}

// Procedure: read
void Celllib::read(const std::filesystem::path& path) {
  
  OT_LOGE_RIF(
    path.empty() || !std::filesystem::exists(path),
    "celllib ", path, " doesn't exist"
  );

  OT_LOGI("loading celllib ", path, " ...");

  static std::string_view delimiters = "(),:;/#[]{}*\"\\";
  static std::string_view exceptions = "(){}";

  auto tokens = tokenize(path, delimiters, exceptions);

  // Set up the iterator
  auto itr = tokens.begin();
  auto end = tokens.end();

  // Read the library name.
  if(itr = std::find(itr, end, "library"); itr == end) {
    OT_LOGF("can't find keyword ", std::quoted("library"));
  }

  if(itr = on_next_parentheses(
    itr, 
    end, 
    [&] (auto& str) mutable { name = std::move(str); }); itr == end) {
    OT_LOGF("can't find library name");
  }

  // Extract the library group
  if(itr = std::find(itr, tokens.end(), "{"); itr == tokens.end()) {
    OT_LOGF("can't find library group symbol '{'");
  }

  int stack = 1;
  
  while(stack && ++itr != end) {
    
    if(*itr == "lu_table_template") {
      auto lut = _extract_lut_template(itr, end);
      lut_templates[lut.name] = std::move(lut);
    }
    else if(*itr == "delay_model") {
      OT_LOGF_IF(++itr == end, "syntax error in delay_model");
      if(auto ditr = delay_models.find(*itr); ditr != delay_models.end()) {
        delay_model = ditr->second;
      }
      else {
        OT_LOGW("unexpected delay model ", *itr);
      }
    }
    else if(*itr == "default_cell_leakage_power") {
      OT_LOGF_IF(++itr == end, "syntax error in default_cell_leakage_power");
      default_cell_leakage_power = std::stof(*itr);
    }
    else if(*itr == "default_inout_pin_cap") {
      OT_LOGF_IF(++itr == end, "syntax error in default_inout_pin_cap");
      default_inout_pin_cap = std::stof(*itr);
    }
    else if(*itr == "default_input_pin_cap") {
      OT_LOGF_IF(++itr == end, "syntax error in default_input_pin_cap");
      default_input_pin_cap = std::stof(*itr);
    }
    else if(*itr == "default_output_pin_cap") {
      OT_LOGF_IF(++itr == end, "syntax error in default_output_pin_cap");
      default_output_pin_cap = std::stof(*itr);
    }
    else if(*itr == "default_fanout_load") {
      OT_LOGF_IF(++itr == end, "syntax error in default_fanout_load");
      default_fanout_load = std::stof(*itr);
    }
    else if(*itr == "default_max_fanout") {
      OT_LOGF_IF(++itr == end, "syntax error in default_max_fanout");
      default_max_fanout = std::stof(*itr);
    }
    else if(*itr == "default_max_transition") {
      OT_LOGF_IF(++itr == end, "syntax error in default_max_transition");
      default_max_transition = std::stof(*itr);
    }
    // TODO: Unit field.
    else if(*itr == "time_unit") {
      OT_LOGF_IF(++itr == end, "time_unit syntax error");
    }
    else if(*itr == "voltage_unit") {
      OT_LOGF_IF(++itr == end, "voltage_unit syntax error");
    }
    else if(*itr == "current_unit") {
      OT_LOGF_IF(++itr == end, "current_unit syntax error");
    }
    else if(*itr == "pulling_resistance_unit") {
      OT_LOGF_IF(++itr == end, "pulling_resistance_unit syntax error");
    }
    else if(*itr == "leakage_power_unit") {
      OT_LOGF_IF(++itr == end, "leakage_power_unit syntax error");
    }
    else if(*itr == "capacitive_load_unit") {
      std::string unit;
      if(itr = on_next_parentheses(
        itr, 
        end, 
        [&] (auto& str) mutable { unit += str; }); itr == end) {
        OT_LOGF("capacitive_load_unit syntax error");
      }
    }
    else if(*itr == "cell") { 
      auto cell = _extract_cell(itr, end);
      cells[cell.name] = std::move(cell); 
    }
    else if(*itr == "}") {
      stack--;
    }
    else if(*itr == "{") {
      stack++;
    }
    else {
    }
  }

  if(stack != 0 || *itr != "}") {
    OT_LOGF("can't find library group brace '}'");
  }

}

//// Procedure: to_time_unit
//// Convert the numerics to the new unit
//void Celllib::to_time_unit(const TimeUnit& unit) {
//  
//  float s = (time_unit) ? divide_time_unit(*time_unit, unit) : 1.0f;
//
//  if(time_unit = unit; std::fabs(s - 1.0f) < 1e-6) {
//    return;
//  }
//
//  for(auto& c : cells) {
//    c.second.scale_time(s);
//  }
//}
//
//// Procedure: to_capacitance_unit
//void Celllib::to_capacitance_unit(const CapacitanceUnit& unit) {
//  
//  float s = (capacitance_unit) ? divide_capacitance_unit(*capacitance_unit, unit) : 1.0f;
//
//  if(capacitance_unit = unit; std::fabs(s - 1.0f) < 1e-6) {
//    return;
//  }
//
//  for(auto& c : cells) {
//    c.second.scale_capacitance(s);
//  }
//}
//
//// Procedure: to_voltage_unit
//void Celllib::to_voltage_unit(const VoltageUnit& unit) {
//
//  float s = (voltage_unit) ? divide_voltage_unit(*voltage_unit, unit) : 1.0f;
//
//  if(voltage_unit = unit; std::fabs(s - 1.0f) < 1e-6) {
//    return;
//  }
//
//  // TODO
//}
//
//// Procedure: to_current_unit
//void Celllib::to_current_unit(const CurrentUnit& unit) {
//
//  float s = (current_unit) ? divide_current_unit(*current_unit, unit) : 1.0f;
//
//  if(current_unit = unit; std::fabs(s - 1.0f) < 1e-6) {
//    return;
//  }
//
//  // TODO
//}
//
//// Procedure: to_resistance_unit
//void Celllib::to_resistance_unit(const ResistanceUnit& unit) {
//
//  float s = (resistance_unit) ? divide_resistance_unit(*resistance_unit, unit) : 1.0f;
//
//  if(resistance_unit = unit; std::fabs(s - 1.0f) < 1e-6) {
//    return;
//  }
//
//  // TODO
//}
//
//// Procedure: to_power_unit
//void Celllib::to_power_unit(const PowerUnit& unit) {
//
//  float s = (power_unit) ? divide_power_unit(*power_unit, unit) : 1.0f;
//
//  if(power_unit = unit; std::fabs(s - 1.0f) < 1e-6) {
//    return;
//  }
//
//  // TODO
//}

// Operator: <<
std::ostream& operator << (std::ostream& os, const Celllib& c) {

  // Write the comment.
  os << "/* Generated by OpenTimer " << " */\n";
  
  // Write library name.
  os << "library (\"" << c.name << "\") {\n";

  // Delay modeA
  if(c.delay_model) {
    os << "delay_model : " << to_string(*(c.delay_model)) << ";\n";
  }

  // Write the lut templates
  for(const auto& kvp : c.lut_templates) {
    os << kvp.second << '\n';
  }

  // Write all cells.
  for(const auto& kvp : c.cells) {
    os << kvp.second << '\n';
  }

  // Write library ending group symbol.
  os << "}\n";

  return os;
}



};  // namespace ot. ------------------------------------------------------------------------------





