#include <ot/shell/shell.hpp>

namespace ot {

/*// Procedure: set_units
void set_units(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  std::string token;

  while(is >> token) {
    if(token == "-time") {
      is >> token;
      if(auto unit = make_time_unit(token); unit) {
        timer.time_unit(*unit);
      }
      else {
        es << "time unit syntax error " << std::quoted(token) << '\n';
      }
    }
    else if(token == "-capacitance") {
      is >> token;
      if(auto unit = make_capacitance_unit(token); unit) {
        timer.capacitance_unit(*unit);
      }
      else {
        es << "capacitance unit syntax error " << std::quoted(token) << '\n';
      }
    }
    else if(token == "-resistance") {
      is >> token;
      if(auto unit = make_resistance_unit(token); unit) {
        timer.resistance_unit(*unit);
      }
      else {
        es << "resistance unit syntax error " << std::quoted(token) << '\n';
      }
    }
    else if(token == "-voltage") {
      is >> token;
      if(auto unit = make_voltage_unit(token); unit) {
        timer.voltage_unit(*unit);
      }
      else {
        es << "voltage unit syntax error " << std::quoted(token) << '\n';
      }
    }
    else if(token == "-current") {
      is >> token;
      if(auto unit = make_current_unit(token); unit) {
        timer.current_unit(*unit);
      }
      else {
        es << "current unit syntax error " << std::quoted(token) << '\n';
      }
    }
    else if(token == "-power") {
      is >> token;
      if(auto unit = make_power_unit(token); unit) {
        timer.power_unit(*unit);
      }
      else {
        es << "power unit syntax error " << std::quoted(token) << '\n';
      }
    }
    else {
      es << "unexpected token " << std::quoted(token) << '\n';
    }
  }
}*/

// ------------------------------------------------------------------------------------------------

// Procedure: set_num_threads
void set_num_threads(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {
  if(unsigned N = 1; is >> N) {
    timer.num_threads(N);
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: read_verilog
void read_verilog(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {
  if(std::filesystem::path path; is >> path) {
    timer.verilog(std::move(path));
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: read_spef
void read_spef(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {
  if(std::filesystem::path path; is >> path) {
    timer.spef(std::move(path));
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: read_timing
void read_timing(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {
  if(std::filesystem::path path; is >> path) {
    timer.timing(std::move(path));
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: read_celllib
void read_celllib(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  std::string token;
  std::filesystem::path path;
  std::optional<Split> el;

  while(is >> token) {
    if(token == "-early") {
      el = EARLY;
    }
    else if(token == "-late") {
      el = LATE;
    }
    else {
      path = std::move(token);
    }
  }

  timer.celllib(std::move(path), el);
}

// Procedure: read_sdc
void read_sdc(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {
  if(std::filesystem::path path; is >> path) {
    timer.sdc(std::move(path));
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: set_at
void set_at(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  std::string token;
  std::string pin;
  auto el = EARLY;
  auto rf = RISE;
  std::optional<float> value;

  while(is >> token) {
    if(token == "-pin") is >> pin;
    else if(token == "-early") el = EARLY;
    else if(token == "-late" ) el = LATE;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      try {
        value = std::stof(token);
      }
      catch(...) {
        es << "failed to parse " << std::quoted(token) << '\n';
        return;
      }
    }
  }

  if(pin.empty()) {
    es << "-pin <name> not given\n";
    return;
  }

  timer.at(std::move(pin), el, rf, value);
}

// ------------------------------------------------------------------------------------------------

// Procedure: set_slew
void set_slew(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  std::string token;
  std::string pin;
  Split el = EARLY;
  Tran  rf = RISE;
  std::optional<float> value;

  while(is >> token) {
    if(token == "-pin") is >> pin;
    else if(token == "-early") el = EARLY;
    else if(token == "-late" ) el = LATE;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      try {
        value = std::stof(token);
      }
      catch(...) {
        es << "failed to parse " << std::quoted(token) << '\n';
        return;
      }
    }
  }

  if(pin.empty()) {
    es << "-pin <name> not given\n";
    return;
  }

  timer.slew(std::move(pin), el, rf, value);
}

// ------------------------------------------------------------------------------------------------

// Procedure: set_rat
void set_rat(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  std::string token;
  std::string pin;
  Split el = EARLY;
  Tran  rf = RISE;
  std::optional<float> value;

  while(is >> token) {
    if(token == "-pin") is >> pin;
    else if(token == "-early") el = EARLY;
    else if(token == "-late" ) el = LATE;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      try {
        value = std::stof(token);
      }
      catch(...) {
        es << "failed to parse " << std::quoted(token) << '\n';
        return;
      }
    }
  }

  if(pin.empty()) {
    es << "-pin <name> not given\n";
    return;
  }
  
  timer.rat(std::move(pin), el, rf, value);
}

// ------------------------------------------------------------------------------------------------

// Procedure: set_load
void set_load(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  std::string token;
  std::string pin;
  Split el = EARLY;
  Tran  rf = RISE;
  std::optional<float> value;

  while(is >> token) {
    if(token == "-pin") is >> pin;
    else if(token == "-early") el = EARLY;
    else if(token == "-late" ) el = LATE;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      try {
        value = std::stof(token);
      }
      catch(...) {
        es << "failed to parse " << std::quoted(token) << '\n';
        return;
      }
    }
  }

  if(pin.empty()) {
    es << "-pin <name> not given\n";
    return;
  }
  
  timer.load(std::move(pin), el, rf, value ? *value : 0.0f);
}

// ------------------------------------------------------------------------------------------------

// Procedure: insert_gate
void insert_gate(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  if(std::string gate, cell; (is >> gate >> cell)) {
    timer.insert_gate(std::move(gate), std::move(cell));
  }
  else {
    es << "usage: insert_gate <gate> <cell>\n";
  }
}

// Procedure: remove_gate
void remove_gate(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  if(std::string gate; is >> gate) {
    timer.remove_gate(std::move(gate));
  }
  else {
    es << "usage: remove_gate <gate>\n";
  }
}

// Procedure: repower_gate
void repower_gate(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  if(std::string gate, cell; (is >> gate >> cell)) {
    timer.repower_gate(std::move(gate), std::move(cell));
  }
  else {
    es << "usage: repower_gate <gate> <cell>\n";
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: insert_net
void insert_net(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  if(std::string name; (is >> name)) {
    timer.insert_net(std::move(name));
  }
  else {
    es << "usage: insert_net <name>\n";
  }
}

// Procedure: remove_net
void remove_net(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  if(std::string name; (is >> name)) {
    timer.remove_net(std::move(name));
  }
  else {
    es << "usage: remove_net <name>\n";
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: disconnect_pin
void disconnect_pin(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  if(std::string pin; (is >> pin)) {
    timer.disconnect_pin(std::move(pin));
  }
  else {
    es << "usage: disconnect_pin <pin>\n";
  }
}

// Procedure: connect_pin
void connect_pin(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  if(std::string pin, net; (is >> pin >> net)) {
    timer.connect_pin(std::move(pin), std::move(net));
  }
  else {
    es << "usage: connect_pin <pin> <net>\n";
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: cppr
void cppr(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  std::string token;

  if(is >> token; token == "-disable") {
    timer.cppr(false);
  }
  else if(token == "-enable") {
    timer.cppr(true);
  }
  else {
    es << "usage: cppr -disable|-enable\n";
  }

}


};  // end of namespace ot. -----------------------------------------------------------------------


