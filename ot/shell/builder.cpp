#include <ot/shell/shell.hpp>

namespace ot {

// Procedure: set_units
void Shell::_set_units() {

  std::string token;

  while(_is >> token) {
    if(token == "-time") {
      _is >> token;
      if(auto unit = make_time_unit(token); unit) {
        _timer.set_time_unit(*unit);
      }
      else {
        _es << "time unit syntax error " << std::quoted(token) << '\n';
      }
    }
    else if(token == "-capacitance") {
      _is >> token;
      if(auto unit = make_capacitance_unit(token); unit) {
        _timer.set_capacitance_unit(*unit);
      }
      else {
        _es << "capacitance unit syntax error " << std::quoted(token) << '\n';
      }
    }
    else if(token == "-resistance") {
      _is >> token;
      if(auto unit = make_resistance_unit(token); unit) {
        _timer.set_resistance_unit(*unit);
      }
      else {
        _es << "resistance unit syntax error " << std::quoted(token) << '\n';
      }
    }
    else if(token == "-voltage") {
      _is >> token;
      if(auto unit = make_voltage_unit(token); unit) {
        _timer.set_voltage_unit(*unit);
      }
      else {
        _es << "voltage unit syntax error " << std::quoted(token) << '\n';
      }
    }
    else if(token == "-current") {
      _is >> token;
      if(auto unit = make_current_unit(token); unit) {
        _timer.set_current_unit(*unit);
      }
      else {
        _es << "current unit syntax error " << std::quoted(token) << '\n';
      }
    }
    else if(token == "-power") {
      _is >> token;
      if(auto unit = make_power_unit(token); unit) {
        _timer.set_power_unit(*unit);
      }
      else {
        _es << "power unit syntax error " << std::quoted(token) << '\n';
      }
    }
    else {
      _es << "unexpected token " << std::quoted(token) << '\n';
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: set_num_threads
void Shell::_set_num_threads() {
  if(unsigned N = 1; _is >> N) {
    _timer.set_num_threads(N);
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: read_verilog
void Shell::_read_verilog() {
  if(std::filesystem::path path; _is >> path) {
    _timer.read_verilog(std::move(path));
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: read_spef
void Shell::_read_spef() {
  if(std::filesystem::path path; _is >> path) {
    _timer.read_spef(std::move(path));
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: read_timing
void Shell::_read_timing() {
  if(std::filesystem::path path; _is >> path) {
    _timer.read_timing(std::move(path));
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: read_celllib
void Shell::_read_celllib() {

  std::string token;
  std::filesystem::path path;
  std::optional<Split> el;

  while(_is >> token) {
    if(token == "-min" || token == "-early") {
      el = MIN;
    }
    else if(token == "-max" || token == "-late" || token == "-max") {
      el = MAX;
    }
    else {
      path = std::move(token);
    }
  }

  _timer.read_celllib(std::move(path), el);
}

// Procedure: read_sdc
void Shell::_read_sdc() {
  if(std::filesystem::path path; _is >> path) {
    _timer.read_sdc(std::move(path));
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: set_at
void Shell::_set_at() {

  std::string token;
  std::string pin;
  auto el = MIN;
  auto rf = RISE;
  std::optional<float> value;

  while(_is >> token) {
    if(token == "-pin") _is >> pin;
    else if(token == "-early" || token == "-min") el = MIN;
    else if(token == "-late" || token == "-max" ) el = MAX;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      try {
        value = std::stof(token);
      }
      catch(...) {
        _es << "failed to parse " << std::quoted(token) << '\n';
        return;
      }
    }
  }

  if(pin.empty()) {
    _es << "-pin <name> not given\n";
    return;
  }

  _timer.set_at(std::move(pin), el, rf, value);
}

// ------------------------------------------------------------------------------------------------

// Procedure: set_slew
void Shell::_set_slew() {

  std::string token;
  std::string pin;
  Split el = MIN;
  Tran  rf = RISE;
  std::optional<float> value;

  while(_is >> token) {
    if(token == "-pin") _is >> pin;
    else if(token == "-early" || token == "-min") el = MIN;
    else if(token == "-late" || token == "-max" ) el = MAX;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      try {
        value = std::stof(token);
      }
      catch(...) {
        _es << "failed to parse " << std::quoted(token) << '\n';
        return;
      }
    }
  }

  if(pin.empty()) {
    _es << "-pin <name> not given\n";
    return;
  }

  _timer.set_slew(std::move(pin), el, rf, value);
}

// ------------------------------------------------------------------------------------------------

// Procedure: set_rat
void Shell::_set_rat() {

  std::string token;
  std::string pin;
  Split el = MIN;
  Tran  rf = RISE;
  std::optional<float> value;

  while(_is >> token) {
    if(token == "-pin") _is >> pin;
    else if(token == "-early" || token == "-min") el = MIN;
    else if(token == "-late" || token == "-max" ) el = MAX;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      try {
        value = std::stof(token);
      }
      catch(...) {
        _es << "failed to parse " << std::quoted(token) << '\n';
        return;
      }
    }
  }

  if(pin.empty()) {
    _es << "-pin <name> not given\n";
    return;
  }
  
  _timer.set_rat(std::move(pin), el, rf, value);
}

// ------------------------------------------------------------------------------------------------

// Procedure: set_load
void Shell::_set_load() {

  std::string token;
  std::string pin;
  Split el = MIN;
  Tran  rf = RISE;
  std::optional<float> value;

  while(_is >> token) {
    if(token == "-pin") _is >> pin;
    else if(token == "-early" || token == "-min") el = MIN;
    else if(token == "-late"  || token == "-max") el = MAX;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      try {
        value = std::stof(token);
      }
      catch(...) {
        _es << "failed to parse load " << std::quoted(token) << '\n';
        return;
      }
    }
  }

  if(pin.empty()) {
    _es << "-pin <name> not given\n";
    return;
  }
  
  _timer.set_load(std::move(pin), el, rf, value ? *value : 0.0f);
}

// ------------------------------------------------------------------------------------------------

// Procedure: insert_gate
void Shell::_insert_gate() {

  if(std::string gate, cell; (_is >> gate >> cell)) {
    _timer.insert_gate(std::move(gate), std::move(cell));
  }
  else {
    _es << "usage: insert_gate <gate> <cell>\n";
  }
}

// Procedure: remove_gate
void Shell::_remove_gate() {

  if(std::string gate; _is >> gate) {
    _timer.remove_gate(std::move(gate));
  }
  else {
    _es << "usage: remove_gate <gate>\n";
  }
}

// Procedure: repower_gate
void Shell::_repower_gate() {

  if(std::string gate, cell; (_is >> gate >> cell)) {
    _timer.repower_gate(std::move(gate), std::move(cell));
  }
  else {
    _es << "usage: repower_gate <gate> <cell>\n";
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: insert_net
void Shell::_insert_net() {

  if(std::string name; (_is >> name)) {
    _timer.insert_net(std::move(name));
  }
  else {
    _es << "usage: insert_net <name>\n";
  }
}

// Procedure: remove_net
void Shell::_remove_net() {

  if(std::string name; (_is >> name)) {
    _timer.remove_net(std::move(name));
  }
  else {
    _es << "usage: remove_net <name>\n";
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: disconnect_pin
void Shell::_disconnect_pin() {

  if(std::string pin; (_is >> pin)) {
    _timer.disconnect_pin(std::move(pin));
  }
  else {
    _es << "usage: disconnect_pin <pin>\n";
  }
}

// Procedure: connect_pin
void Shell::_connect_pin() {

  if(std::string pin, net; (_is >> pin >> net)) {
    _timer.connect_pin(std::move(pin), std::move(net));
  }
  else {
    _es << "usage: connect_pin <pin> <net>\n";
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: cppr
void Shell::_cppr() {

  std::string token;

  if(_is >> token; token == "-disable") {
    _timer.cppr(false);
  }
  else if(token == "-enable") {
    _timer.cppr(true);
  }
  else {
    _es << "usage: cppr -disable|-enable\n";
  }
}

// Procedure: _enable_cppr
void Shell::_enable_cppr() {
  _timer.cppr(true);
}

// Procedure: _disable_cppr
void Shell::_disable_cppr() {
  _timer.cppr(false);
}


};  // end of namespace ot. -----------------------------------------------------------------------


