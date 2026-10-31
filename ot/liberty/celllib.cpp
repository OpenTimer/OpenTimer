#include <ot/liberty/celllib.hpp>

namespace ot {

// Procedure: _uncomment
void Celllib::_uncomment(std::vector<char>& buffer) {
  
  auto fsize = buffer.size() > 0 ? buffer.size() - 1 : 0;

  // Mart out the comment
  for(size_t i=0; i<fsize; ++i) {

    // Block comment
    if(buffer[i] == '/' && buffer[i+1] == '*') {
      buffer[i] = buffer[i+1] = ' ';
      for(i=i+2; i<fsize; buffer[i++]=' ') {
        if(buffer[i] == '*' && buffer[i+1] == '/') {
          buffer[i] = buffer[i+1] = ' ';
          i = i+1;
          break;
        }
      }
    }
    
    // Line comment
    if(buffer[i] == '/' && buffer[i+1] == '/') {
      buffer[i] = buffer[i+1] = ' ';
      for(i=i+2; i<fsize; ++i) {
        if(buffer[i] == '\n' || buffer[i] == '\r') {
          break;
        }
        else buffer[i] = ' ';
      }
    }
    
    // Pond comment
    if(buffer[i] == '#') {
      buffer[i] = ' ';
      for(i=i+1; i<fsize; ++i) {
        if(buffer[i] == '\n' || buffer[i] == '\r') {
          break;
        }
        else buffer[i] = ' ';
      }
    }
  }
}

// Procedure: _tokenize
void Celllib::_tokenize(const std::vector<char>& buf, std::vector<std::string_view>& tokens) {

  static std::string_view dels = "(),:;/#[]{}*\"\\";

  // get the position
  const char* beg = buf.data();
  const char* end = buf.data() + buf.size();

  // Parse the token.
  const char *token {nullptr};
  size_t len {0};
  int bracket_depth = 0;

  tokens.clear();

  for(const char* itr = beg; itr != end && *itr != 0; ++itr) {
    
    // extract the entire quoted string as a token
    if(*itr == '[') ++bracket_depth;
    bool is_del = (dels.find(*itr) != std::string_view::npos)
                  && *itr != '[' && *itr != ']'
                  && !(*itr == ':' && bracket_depth);
    if(*itr == ']') --bracket_depth;

    if(std::isspace(*itr) || is_del) {
      if(len > 0) {                            // Add the current token.
        tokens.push_back({token, len});
        token = nullptr;
        len = 0;
      }
      // group delimiter is liberty token
      if(*itr == '(' || *itr == ')' || *itr == '{' || *itr == '}') {
        tokens.push_back({itr, 1});
      }
      // Keep every related pin in a grouped Liberty timing arc.
      else if(*itr == '"' && !tokens.empty() && (tokens.back() == "related_pin" || tokens.back() == "function" || tokens.back() == "when")) {
        const char* first = ++itr;
        while(itr != end && *itr != '"' && *itr != 0) ++itr;
        OT_LOGF_IF(itr == end || *itr != '"', "unterminated related_pin");
        tokens.push_back({first, static_cast<size_t>(itr - first)});
      }
      // Other quoted values retain the existing tokenizer behavior.
      // extract the entire quoted string (this is buggy now...)
      //else if(*itr == '"') {
      //  for(++itr; itr != end && *itr != '"'; ++itr, ++len) ;
      //  if(len > 0) {
      //    tokens.push_back({itr-len, len});
      //    len = 0;
      //  }
      //}
    } 
    else {
      if(len == 0) {
        token = itr;
      }
      ++len;
    }
  }

  if(len > 0) {
    tokens.push_back({token, len});
  } 
}

// ------------------------------------------------------------------------------------------------

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

// Function: _extract_operating_conditions
std::optional<float> Celllib::_extract_operating_conditions(token_iterator& itr, const token_iterator end) {

  std::optional<float> voltage;
  std::string operating_condition_name;

  if(itr=on_next_parentheses(
    itr, 
    end, 
    [&] (auto& name) mutable { operating_condition_name = name; }); itr == end) {
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
    if(*itr == "voltage") {                       // Read the variable.

      if(++itr == end) {
        OT_LOGF("volate error in operating_conditions template ", operating_condition_name);
      }

      voltage = std::strtof(std::string(*itr).c_str(), nullptr);
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
  
  if(stack != 0 || itr == end || *itr != "}") {
    OT_LOGF("can't find operating_conditions template group brace '}'");
  }

  return voltage;
}

// Function: _extract_lut_template
LutTemplate Celllib::_extract_lut_template(token_iterator& itr, const token_iterator end) {

  LutTemplate lt;

  if(itr=on_next_parentheses(
    itr, 
    end, 
    [&] (auto& name) mutable { lt.name = name; }); itr == end) {
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
      itr = on_next_parentheses(itr, end, [&] (auto& str) { 
        lt.indices1.push_back(std::strtof(str.data(), nullptr)); 
      });
    }
    // index_2
    else if(*itr == "index_2") {
      itr = on_next_parentheses(itr, end, [&] (auto& str) { 
        lt.indices2.push_back(std::strtof(str.data(), nullptr)); 
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
  
  if(stack != 0 || itr == end || *itr != "}") {
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
    [&] (auto& name) mutable { lut.name = name; }); itr == end) {
    OT_LOGF("can't find lut template name");
  }
  
  // Set up the template
  if(const auto* source = lut_template(lut.name)) {
    // A table owns its library-scoped axes. Library copies and merges must
    // not retain pointers into a temporary or a same-named template.
    lut.lut_template = std::make_shared<LutTemplate>(*source);
  }
  
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
        lut.indices1.push_back(std::strtof(v.data(), nullptr));
      });

      if(lut.indices1.size() == 0) {
        OT_LOGF("syntax error in ", lut.name, " index_1");
      }

      size1 = lut.indices1.size();
    }
    else if(*itr == "index_2") {
      itr = on_next_parentheses(itr, end, [&] (auto& v) mutable {
        lut.indices2.push_back(std::strtof(v.data(), nullptr));
      });

      if(lut.indices2.size() == 0) {
        OT_LOGF("syntax error in ", lut.name, " index_2");
      }
      
      size2 = lut.indices2.size();
    }
    else if(*itr == "values") {

      // Liberty permits axes to be inherited from the named template.
      if(lut.indices1.empty() && lut.lut_template) {
        lut.indices1 = lut.lut_template->indices1;
        size1 = std::max(size_t{1}, lut.indices1.size());
      }
      if(lut.indices2.empty() && lut.lut_template) {
        lut.indices2 = lut.lut_template->indices2;
        size2 = std::max(size_t{1}, lut.indices2.size());
      }
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

      size_t id {0};
      itr = on_next_parentheses(itr, end, [&] (auto& v) mutable {
        OT_LOGF_IF(id >= lut.table.size(), "too many values in lut ", lut.name);
        lut.table[id++] = std::strtof(v.data(), nullptr);
      });
      OT_LOGF_IF(id != lut.table.size(), "too few values in lut ", lut.name);
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

  if(stack != 0 || itr == end || *itr != "}") {
    OT_LOGF("group brace '}' error in lut ", lut.name);
  }

  return lut;
}

// Function: _extract_internal_power
InternalPower Celllib::_extract_internal_power(token_iterator& itr, const token_iterator end) {

  InternalPower power;

  // Extract the lut template group
  if(itr = std::find(itr, end, "{"); itr == end) {
    OT_LOGF("can't find group brace '{' in timing");
  }

  int stack = 1;

  while(stack && ++itr != end) {

    if (*itr == "rise_power") {
      power.rise_power = _extract_lut(itr, end);
    }
    else if (*itr == "fall_power") {                  // Rise delay.
      power.fall_power = _extract_lut(itr, end);
    }
    else if (*itr == "related_pin") {

      if(++itr == end) {
        OT_LOGF("syntax error in related_pin");
      }

      power.related_pin = *itr;
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

  if(stack != 0 || itr == end || *itr != "}") {
    OT_LOGF("can't find group brace '}' in internal_power");
  }

  return power;
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
    else if(*itr == "when") {
      OT_LOGF_IF(++itr == end, "missing timing condition");
      timing.when = std::string(*itr);
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
        OT_LOGW("unexpected timing type ", *itr);
      }
    }
    else if (*itr == "related_pin") {
      
      if(++itr == end) {
        OT_LOGF("syntax error in related_pin");
      }

      timing.related_pin = *itr;
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

  if(stack != 0 || itr == end || *itr != "}") {
    OT_LOGF("can't find group brace '}' in timing");
  }

  return timing;
}

// Functoin: _extract_cellpin
Cellpin Celllib::_extract_cellpin(token_iterator& itr, const token_iterator end,
                                const Cellpin* defaults, std::vector<Cellpin>* children) {

  Cellpin cellpin = defaults ? *defaults : Cellpin{};
  std::vector<token_iterator> nested_pins;
  
  if(itr=on_next_parentheses(
    itr, 
    end, 
    [&] (auto& name) mutable { cellpin.name = name; }); itr == end) {
    OT_LOGF("can't find cellpin name");
  }
  
  // Extract the lut template group
  if(itr = std::find(itr, end, "{"); itr == end) {
    OT_LOGF("can't find group brace '{' in cellpin ", cellpin.name);
  }

  //std::cout << "  -->" << cellpin.name << std::endl;

  int stack = 1;
  
  while(stack && ++itr != end) {
    
    if(*itr == "pin") {
      nested_pins.push_back(itr);
      itr = std::find(itr, end, "{");
      OT_LOGF_IF(itr == end, "missing nested pin group");
      int nesting = 1;
      while(nesting && ++itr != end) {
        if(*itr == "{") ++nesting;
        if(*itr == "}") --nesting;
      }
      OT_LOGF_IF(itr == end, "unterminated nested pin group");
    }
    else if(*itr == "direction") {

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
      cellpin.capacitance = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "max_capacitance") {
      OT_LOGF_IF(++itr == end, "can't get the max_capacitance in cellpin ", cellpin.name);
      cellpin.max_capacitance = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "min_capacitance") {  
      OT_LOGF_IF(++itr == end, "can't get the min_capacitance in cellpin ", cellpin.name);
      cellpin.min_capacitance = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "max_transition") {        
      OT_LOGF_IF(++itr == end, "can't get the max_transition in cellpin ", cellpin.name);
      cellpin.max_transition = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "min_transition") {          
      OT_LOGF_IF(++itr == end, "can't get the min_transition in cellpin ", cellpin.name);
      cellpin.min_transition = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "fall_capacitance") {
      OT_LOGF_IF(++itr == end, "can't get fall_capacitance in cellpin ", cellpin.name);
      cellpin.fall_capacitance = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "rise_capacitance") {
      OT_LOGF_IF(++itr == end, "can't get rise_capacitance in cellpin ", cellpin.name);
      cellpin.rise_capacitance = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "rise_capacitance_range" || *itr == "fall_capacitance_range") {
      auto& range = *itr == "rise_capacitance_range" ? cellpin.rise_capacitance_range : cellpin.fall_capacitance_range;
      size_t count = 0;
      itr = on_next_parentheses(itr, end, [&] (auto& value) {
        OT_LOGF_IF(count >= 2, "too many capacitance range values");
        range[count++] = std::strtof(value.data(), nullptr);
      });
      OT_LOGF_IF(count != 2 || !(*range[MIN] >= 0 && *range[MAX] >= *range[MIN]), "invalid capacitance range");
    }
    else if(*itr == "fanout_load") {
      OT_LOGF_IF(++itr == end, "can't get fanout_load in cellpin ", cellpin.name);
      cellpin.fanout_load = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "max_fanout") {
      OT_LOGF_IF(++itr == end, "can't get max_fanout in cellpin ", cellpin.name);
      cellpin.max_fanout = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "min_fanout") {
      OT_LOGF_IF(++itr == end, "can't get min_fanout in cellpin ", cellpin.name);
      cellpin.min_fanout = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "clock") {  
      OT_LOGF_IF(++itr == end, "can't get the clock status in cellpin ", cellpin.name);
      cellpin.is_clock = (*itr == "true") ? true : false;
    }
    else if(*itr == "function") {
      OT_LOGF_IF(++itr == end, "missing pin function");
      cellpin.function = std::string(*itr);
    }
    else if(*itr == "three_state") {
      cellpin.three_state = true;
    }
    else if(*itr == "original_pin") {
      OT_LOGF_IF(++itr == end, "can't get the original pin in cellpin ", cellpin.name);
      cellpin.original_pin = *itr;
    }
    else if(*itr == "internal_power") {
      cellpin.internal_power.emplace_back(_extract_internal_power(itr, end));
      }
    else if(*itr == "timing") {
      auto timing = _extract_timing(itr, end);
      const std::string related = timing.related_pin;
      size_t start = related.find_first_not_of(" \t\r\n");
      while(start != std::string::npos) {
        const auto stop = related.find_first_of(" \t\r\n", start);
        timing.related_pin = related.substr(start, stop - start);
        cellpin.timings.emplace_back(timing);
        start = stop == std::string::npos ? stop : related.find_first_not_of(" \t\r\n", stop);
      }
      // No related pin at all (absent or ""): keep the arc as parsed, never drop it.
      if(related.find_first_not_of(" \t\r\n") == std::string::npos) {
        cellpin.timings.emplace_back(std::move(timing));
      }
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
  
  if(stack != 0 || itr == end || *itr != "}") {
    OT_LOGF("can't find group brace '}' in cellpin ", cellpin.name);
  }

  if(children) {
    for(auto nested : nested_pins) {
      children->push_back(_extract_cellpin(nested, end, &cellpin));
    }
  }
  return cellpin;
}

// Expand Liberty pin ranges without losing bus-level attributes and arcs.
static void insert_bus_pin(Cell& cell, Cellpin pin) {
  const auto left = pin.name.find('[');
  const auto colon = pin.name.find(':', left);
  const auto right = pin.name.find(']', left);
  if(left == std::string::npos || colon == std::string::npos || right == std::string::npos) {
    cell.cellpins[pin.name] = std::move(pin);
    return;
  }
  const int first = std::stoi(pin.name.substr(left + 1, colon - left - 1));
  const int last = std::stoi(pin.name.substr(colon + 1, right - colon - 1));
  const auto base = pin.name.substr(0, left);
  for(int bit = std::min(first, last); bit <= std::max(first, last); ++bit) {
    pin.name = base + "[" + std::to_string(bit) + "]";
    cell.cellpins[pin.name] = pin;
  }
}

void Celllib::_extract_bus(token_iterator& itr, const token_iterator end, Cell& cell) {
  auto start = itr;
  std::vector<Cellpin> children;
  auto bus = _extract_cellpin(itr, end, nullptr, &children);
  if(!children.empty()) {
    for(auto& pin : children) insert_bus_pin(cell, std::move(pin));
    return;
  }
  auto type = std::find(start, itr, "bus_type");
  OT_LOGF_IF(type == itr || ++type == itr, "missing bus_type for ", bus.name);
  auto range = _bus_ranges.find(std::string{*type});
  OT_LOGF_IF(range == _bus_ranges.end(), "unknown bus_type ", *type);
  bus.name += "[" + std::to_string(range->second.first) + ":"
                  + std::to_string(range->second.second) + "]";
  insert_bus_pin(cell, std::move(bus));
}

// Function: _extract_cell
Cell Celllib::_extract_cell(token_iterator& itr, const token_iterator end) {
  
  Cell cell;
  
  if(itr=on_next_parentheses(
    itr, 
    end, 
    [&] (auto& name) mutable { cell.name = name; }); itr==end) {
    OT_LOGF("can't find cell name");
  }

  // Extract the lut template group
  if(itr = std::find(itr, end, "{"); itr == end) {
    OT_LOGF("can't find group brace '{' in cell ", cell.name);
  }

  int stack = 1;
  
  while(stack && ++itr != end) {
    
    if(*itr == "cell_leakage_power") {               // Read the leakage power.
      OT_LOGF_IF(++itr == end, "can't get leakage power in cell ", cell.name);
      cell.leakage_power = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "cell_footprint") {              // Read the footprint.
      OT_LOGF_IF(++itr == end, "can't get footprint in cell ", cell.name);
      cell.cell_footprint = *itr;
    }
    else if(*itr == "area") {                        // Read the area.
      OT_LOGF_IF(++itr == end, "can't get area in cell ", cell.name);
      cell.area = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "pin") {                         // Read the cell pin group.
      auto pin = _extract_cellpin(itr, end);
      insert_bus_pin(cell, std::move(pin));
    }
    else if(*itr == "bus") {
      _extract_bus(itr, end, cell);
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

  if(stack != 0 || itr == end || *itr != "}") {
    OT_LOGF("can't find group brace '}' in cell ", cell.name);
  }

  return cell;
}

// Procedure: read
std::vector<Celllib> Celllib::read_all(const std::filesystem::path& path) {
  
  std::ifstream ifs(path, std::ios::ate);
  
  // return on failure
  OT_LOGF_IF(!ifs, "failed to open celllib ", path);

  size_t fsize = ifs.tellg();
  ifs.seekg(0, std::ios::beg);
  std::vector<char> buffer(fsize + 1);
  ifs.read(buffer.data(), fsize);
  buffer[fsize] = 0;

  // get tokens
  std::vector<std::string_view> tokens;
  tokens.reserve(buffer.size() / sizeof(std::string));

  Celllib parser;
  parser._uncomment(buffer);
  parser._tokenize(buffer, tokens);

  // Set up the iterator
  auto itr = tokens.begin();
  auto end = tokens.end();
  std::vector<Celllib> libraries;
  while((itr = std::find(itr, end, "library")) != end) {
    libraries.emplace_back();
    libraries.back()._read_group(itr, end);
    if(itr != end) ++itr;
  }
  OT_LOGF_IF(libraries.empty(), "no library group in ", path);
  return libraries;
}

void Celllib::read(const std::filesystem::path& path) {
  auto libraries = read_all(path);
  *this = std::move(libraries.front());
}

void Celllib::_read_group(token_iterator& itr, const token_iterator end) {

  // Read the library name.
  if(itr = std::find(itr, end, "library"); itr == end) {
    OT_LOGF("can't find keyword ", std::quoted("library"));
  }

  if(itr = on_next_parentheses(
    itr, 
    end, 
    [&] (auto& str) mutable { name = str; }); itr == end) {
    OT_LOGF("can't find library name");
  }

  // Extract the library group
  if(itr = std::find(itr, end, "{"); itr == end) {
    OT_LOGF("can't find library group symbol '{'");
  }

  int stack = 1;
  
  while(stack && ++itr != end) {
    
    if(*itr == "type") {
      std::string name;
      itr = on_next_parentheses(itr, end, [&](auto& v) { name = v; });
      itr = std::find(itr, end, "{");
      OT_LOGF_IF(itr == end, "missing bus type group");
      int depth = 1, first = 0, last = 0;
      while(depth && ++itr != end) {
        if(*itr == "{") ++depth;
        else if(*itr == "}") --depth;
        else if(*itr == "bit_from") {
          OT_LOGF_IF(++itr == end, "missing bit_from");
          first = std::stoi(std::string{*itr});
        }
        else if(*itr == "bit_to") {
          OT_LOGF_IF(++itr == end, "missing bit_to");
          last = std::stoi(std::string{*itr});
        }
      }
      OT_LOGF_IF(itr == end, "unterminated bus type group");
      _bus_ranges[name] = {first, last};
    }
    else if(*itr == "lu_table_template") {
      auto lut = _extract_lut_template(itr, end);
      lut_templates[lut.name] = lut;
    }
    else if(*itr == "power_lut_template") {
      auto lut = _extract_lut_template(itr, end);
      lut_templates[lut.name] = lut;
    }
    else if(*itr == "slew_lower_threshold_pct_rise") {
      OT_LOGF_IF(++itr == end, "missing slew_lower_threshold_pct_rise");
      thresholds.lower[RISE] = std::strtof(itr->data(), nullptr) / 100.0f;
    }
    else if(*itr == "slew_lower_threshold_pct_fall") {
      OT_LOGF_IF(++itr == end, "missing slew_lower_threshold_pct_fall");
      thresholds.lower[FALL] = std::strtof(itr->data(), nullptr) / 100.0f;
    }
    else if(*itr == "slew_upper_threshold_pct_rise") {
      OT_LOGF_IF(++itr == end, "missing slew_upper_threshold_pct_rise");
      thresholds.upper[RISE] = std::strtof(itr->data(), nullptr) / 100.0f;
    }
    else if(*itr == "slew_upper_threshold_pct_fall") {
      OT_LOGF_IF(++itr == end, "missing slew_upper_threshold_pct_fall");
      thresholds.upper[FALL] = std::strtof(itr->data(), nullptr) / 100.0f;
    }
    else if(*itr == "input_threshold_pct_rise") {
      OT_LOGF_IF(++itr == end, "missing input_threshold_pct_rise");
      thresholds.input[RISE] = std::strtof(itr->data(), nullptr) / 100.0f;
    }
    else if(*itr == "input_threshold_pct_fall") {
      OT_LOGF_IF(++itr == end, "missing input_threshold_pct_fall");
      thresholds.input[FALL] = std::strtof(itr->data(), nullptr) / 100.0f;
    }
    else if(*itr == "output_threshold_pct_rise") {
      OT_LOGF_IF(++itr == end, "missing output_threshold_pct_rise");
      thresholds.output[RISE] = std::strtof(itr->data(), nullptr) / 100.0f;
    }
    else if(*itr == "output_threshold_pct_fall") {
      OT_LOGF_IF(++itr == end, "missing output_threshold_pct_fall");
      thresholds.output[FALL] = std::strtof(itr->data(), nullptr) / 100.0f;
    }
    else if(*itr == "slew_derate_from_library") {
      OT_LOGF_IF(++itr == end, "missing slew_derate_from_library");
      thresholds.derate = std::strtof(itr->data(), nullptr);
      OT_LOGF_IF(!(thresholds.derate > 0), "invalid slew derate");
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
      default_cell_leakage_power = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "default_inout_pin_cap") {
      OT_LOGF_IF(++itr == end, "syntax error in default_inout_pin_cap");
      default_inout_pin_cap = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "default_input_pin_cap") {
      OT_LOGF_IF(++itr == end, "syntax error in default_input_pin_cap");
      default_input_pin_cap = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "default_output_pin_cap") {
      OT_LOGF_IF(++itr == end, "syntax error in default_output_pin_cap");
      default_output_pin_cap = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "default_fanout_load") {
      OT_LOGF_IF(++itr == end, "syntax error in default_fanout_load");
      default_fanout_load = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "default_max_fanout") {
      OT_LOGF_IF(++itr == end, "syntax error in default_max_fanout");
      default_max_fanout = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "default_max_transition") {
      OT_LOGF_IF(++itr == end, "syntax error in default_max_transition");
      default_max_transition = std::strtof(itr->data(), nullptr);
    }
    else if(*itr == "operating_conditions") {
      OT_LOGF_IF(++itr == end, "syntax error in operating_conditions");
      voltage = _extract_operating_conditions(itr, end);
    // TODO: Unit field.
    }else if(*itr == "time_unit") {
      OT_LOGF_IF(++itr == end, "time_unit syntax error");
      time_unit = make_time_unit(*itr);
    }
    else if(*itr == "voltage_unit") {
      OT_LOGF_IF(++itr == end, "voltage_unit syntax error");
      voltage_unit = make_voltage_unit(*itr);
    }
    else if(*itr == "current_unit") {
      OT_LOGF_IF(++itr == end, "current_unit syntax error");
      current_unit = make_current_unit(*itr);
    }
    else if(*itr == "pulling_resistance_unit") {
      OT_LOGF_IF(++itr == end, "pulling_resistance_unit syntax error");
      resistance_unit = make_resistance_unit(*itr);
    }
    else if(*itr == "leakage_power_unit") {
      OT_LOGF_IF(++itr == end, "leakage_power_unit syntax error");
      power_unit = make_power_unit(*itr);
    }
    else if(*itr == "capacitive_load_unit") {
      std::string unit;
      if(itr = on_next_parentheses(
        itr, 
        end, 
        [&] (auto& str) mutable { unit += str; }); itr == end) {
        OT_LOGF("capacitive_load_unit syntax error");
      }
      capacitance_unit = make_capacitance_unit(unit);
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

  if(stack != 0 || itr == end || *itr != "}") {
    OT_LOGF("can't find library group brace '}'");
  }

  _apply_default_values();
}
  
// Procedure: _apply_default_values
void Celllib::_apply_default_values() {  

  for(auto& ckvp : cells) {
    
    auto& cell = ckvp.second;
    
    // apply the default leakage power
    if(!cell.leakage_power) {
      cell.leakage_power = default_cell_leakage_power;
    }

    for(auto& pkvp : cell.cellpins) {

      auto& cpin = pkvp.second;
      cpin.thresholds = thresholds;
      for(auto rf : {RISE, FALL}) {
        OT_LOGF_IF(!(thresholds.lower[rf] >= 0 && thresholds.upper[rf] <= 1 && thresholds.span(rf) > 0), "invalid slew thresholds");
      }

      // direction-specific default values
      if(!cpin.direction) {
        OT_LOGW("cellpin ", cell.name, '/', cpin.name, " has no direction defined");
        continue;
      }

      switch(*cpin.direction) {

        case CellpinDirection::INPUT:
          if(!cpin.capacitance) {
            cpin.capacitance = default_input_pin_cap;
          }

          if(!cpin.fanout_load) {
            cpin.fanout_load = default_fanout_load;
          }
        break;

        case CellpinDirection::OUTPUT:
          if(!cpin.capacitance) {
            cpin.capacitance = default_output_pin_cap;
          }

          if(!cpin.max_fanout) {
            cpin.max_fanout = default_max_fanout;
          }

          if(!cpin.max_transition) {
            cpin.max_transition = default_max_transition;
          }
        break;

        case CellpinDirection::INOUT:
          if(!cpin.capacitance) {
            cpin.capacitance = default_inout_pin_cap;
          }
        break;

        case CellpinDirection::INTERNAL:
        break;
      }
      
    }
  }
}

// Procedure: scale_time
// Convert the numerics to the new unit
void Celllib::scale_time(float s) {
  
  if(default_max_transition) {
    default_max_transition = *default_max_transition * s;
  }

  for(auto& c : cells) {
    c.second.scale_time(s);
  }
}

// Procedure: scale_capacitance
void Celllib::scale_capacitance(float s) {
  
  if(default_inout_pin_cap) {
    default_inout_pin_cap = *default_inout_pin_cap * s;
  }
  
  if(default_input_pin_cap) {
    default_input_pin_cap = *default_input_pin_cap * s;
  }
  
  if(default_output_pin_cap) {
    default_output_pin_cap = *default_output_pin_cap * s;
  }
  
  for(auto& c : cells) {
    c.second.scale_capacitance(s);
  }
}

// Procedure: scale_voltage
void Celllib::scale_voltage(float s) {
  // TODO
}

// Procedure: scale_current
void Celllib::scale_current(float s) {

  // TODO
}

// Procedure: scale_resistance
void Celllib::scale_resistance(float s) {
  // TODO
}

// Procedure: scale_power
void Celllib::scale_power(float s) {
  // TODO
}

// Operator: <<
std::ostream& operator << (std::ostream& os, const Celllib& c) {

  // Write the comment.
  os << "/* Generated by OpenTimer " << " */\n";
  
  // Write library name.
  os << "library (\"" << c.name << "\") {\n\n";

  // Delay modeA
  if(c.delay_model) {
    os << "delay_model : " << to_string(*(c.delay_model)) << ";\n";
  }

  // Library units
  if(auto u = c.time_unit; u) {
    os << "time_unit : \"" << u->value() << "s\"\n";
  }

  if(auto u = c.voltage_unit; u) {
    os << "voltage_unit : \"" << u->value() << "V\"\n";
  }
  
  if(auto u = c.current_unit; u) {
    os << "current_unit : \"" << u->value() << "A\"\n";
  }
  
  if(auto u = c.resistance_unit; u) {
    os << "pulling_resistance_unit : \"" << u->value() << "ohm\"\n";
  }

  if(auto u = c.power_unit; u) {
    os << "leakage_power_unit : \"" << u->value() << "W\"\n";
  }

  if(auto u = c.capacitance_unit; u) {
    os << "capacitive_load_unit (" << u->value() << ",F)\"\n";
  }
  
  // default values
  if(c.default_cell_leakage_power) {
    os << *c.default_cell_leakage_power << '\n';
  }

  if(c.default_inout_pin_cap) {
    os << *c.default_inout_pin_cap << '\n';
  }

  if(c.default_input_pin_cap) {
    os << *c.default_input_pin_cap << '\n';
  }

  if(c.default_output_pin_cap) {
    os << *c.default_fanout_load << '\n';
  }
  
  if(c.default_max_fanout) {
    os << *c.default_max_fanout << '\n';
  }
  
  if(c.default_max_transition) {
    os << *c.default_max_transition << '\n';
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





