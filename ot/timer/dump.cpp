#include <ot/timer/timer.hpp>

namespace ot {

// Function: dump_graph
void Timer::dump_graph(std::ostream& os) const {
  std::shared_lock lock(_mutex);
  _dump_graph(os);
}

// Function: _dump_graph
void Timer::_dump_graph(std::ostream& os) const {
  
  os << "digraph TimingGraph {\n";
  for(const auto& pin : _pins) {
    os << "  \"" << pin.second._name << "\";\n";
  }

  for(const auto& arc : _arcs) {
    os << "  \"" << arc._from._name << "\" -> \"" << arc._to._name << "\";\n";  
  }
  os << "}\n";
}

// Function: dump_taskflow
void Timer::dump_taskflow(std::ostream& os) const {
  std::shared_lock lock(_mutex);
  _dump_taskflow(os);
}

// Function: _dump_taskflow
void Timer::_dump_taskflow(std::ostream& os) const {
  _taskflow.dump(os);
}

// Function: dump_timer
void Timer::dump_timer(std::ostream& os) const {
  std::shared_lock lock(_mutex);
  _dump_timer(os);
}

// Function: _dump_timer
void Timer::_dump_timer(std::ostream& os) const {

  os << "OpenTimer " << OT_VERSION << '\n';
  
  // units
  if(_time_unit) {
    os << "Time unit        : " << *_time_unit << '\n';
  }
  
  if(_capacitance_unit) {
    os << "Capacitance unit : " << *_capacitance_unit << '\n';
  }

  if(_voltage_unit) {
    os << "Voltage unit     : " << *_voltage_unit << '\n';
  }

  if(_resistance_unit) {
    os << "Resistance unit  : " << *_resistance_unit << '\n';
  }

  if(_current_unit) {
    os << "Current unit     : " << *_current_unit << '\n';
  }

  if(_power_unit) {
    os << "Power unit       : "  << *_power_unit << '\n';
  }

  size_t num_cells = 0;

  FOR_EACH_EL_IF(el, _celllib[el]) {
    num_cells = std::max(num_cells, _celllib[el]->cells.size());
  }

  // design statistics
  os << "# Pins           : " << _pins.size()  << '\n'
      << "# POs            : " << _pos.size()   << '\n'
      << "# PIs            : " << _pis.size()   << '\n'
      << "# Gates          : " << _gates.size() << '\n'
      << "# Nets           : " << _nets.size()  << '\n'
      << "# Arcs           : " << _arcs.size()  << '\n'
      << "# SCCs           : " << _sccs.size()  << '\n'
      << "# Tests          : " << _tests.size() << '\n'
      << "# Cells          : " << num_cells     << '\n';
}

// Function: dump_net_load
void Timer::dump_net_load(std::ostream& os) const {
  std::shared_lock lock(_mutex);
  _dump_net_load(os);
}

// Function: _dump_net_load
void Timer::_dump_net_load(std::ostream& os) const {
  
  os << "Net Load [nets:" << _nets.size() << "]\n";

  if(!_nets.empty())  {

    // find the maximum net name
    auto nlen = _max_net_name_size();

    os << std::setfill('-') << std::setw(49 + nlen) << '\n'
        << std::setfill(' ') << std::setw(10) << "E/R"  
                             << std::setw(12) << "E/F"
                             << std::setw(12) << "L/R"
                             << std::setw(12) << "L/F" 
                             << std::setw(2 + nlen)  << "Net"   << '\n'
        << std::setfill('-') << std::setw(49 + nlen) << '\n';

    os << std::setfill(' ') << std::fixed << std::setprecision(3);
    for(const auto& kvp : _nets) {

      const auto& net = kvp.second;
      
      FOR_EACH_EL_RF(el, rf) {
        os << std::setw(10) << net._load(el, rf) << "  ";
      }

      os << std::setw(nlen) << net._name << '\n';
    }
    os << std::setfill('-') << std::setw(49 + nlen) << '\n';
  }
}

// Function: dump_pin_cap
void Timer::dump_pin_cap(std::ostream& os) const {
  std::shared_lock lock(_mutex);
  _dump_pin_cap(os);
}

// Function: _dump_pin_cap
void Timer::_dump_pin_cap(std::ostream& os) const {
  
  os << "Pin Capacitance [pins:" << _pins.size() 
      << "]\n";

  if(!_pins.empty())  {

    // find the maximum pin name
    auto plen = _max_pin_name_size();

    os << std::setfill('-') << std::setw(49 + plen) << '\n'
        << std::setfill(' ') << std::setw(10) << "E/R"  
                             << std::setw(12) << "E/F"
                             << std::setw(12) << "L/R"
                             << std::setw(12) << "L/F" 
                             << std::setw(2 + plen)  << "Pin"   << '\n'
        << std::setfill('-') << std::setw(49 + plen) << '\n';

    os << std::setfill(' ') << std::fixed << std::setprecision(3);
    for(const auto& kvp : _pins) {

      const auto& pin = kvp.second;
      
      FOR_EACH_EL_RF(el, rf) {
        os << std::setw(10) << pin.cap(el, rf) << "  ";
      }

      os << std::setw(plen) << pin._name << '\n';
    }
    os << std::setfill('-') << std::setw(49 + plen) << '\n';
  }
}

// Function: dump_slew
void Timer::dump_slew(std::ostream& os) const {
  std::shared_lock lock(_mutex);
  _dump_slew(os);
}

// Function: _dump_slew
void Timer::_dump_slew(std::ostream& os) const {
  
  os << "Slew [pins:" << _pins.size() << "]\n";

  if(!_pins.empty())  {

    // find the maximum pin name
    auto plen = _max_pin_name_size();

    os << std::setfill('-') << std::setw(49 + plen) << '\n'
        << std::setfill(' ') << std::setw(10) << "E/R"  
                             << std::setw(12) << "E/F"
                             << std::setw(12) << "L/R"
                             << std::setw(12) << "L/F" 
                             << std::setw(2 + plen)  << "Pin"   << '\n'
        << std::setfill('-') << std::setw(49 + plen) << '\n';

    os << std::setfill(' ') << std::fixed << std::setprecision(3);
    for(const auto& kvp : _pins) {

      const auto& pin = kvp.second;
      
      FOR_EACH_EL_RF(el, rf) {
        os << std::setw(10);
        if(auto slew = pin.slew(el, rf); slew) os << *slew;
        else os << "n/a";
        os << "  ";
      }

      os << std::setw(plen) << pin._name << '\n';
    }
    os << std::setfill('-') << std::setw(49 + plen) << '\n';
  }
}

// Function: dump_slack
void Timer::dump_slack(std::ostream& os) const {
  std::shared_lock lock(_mutex);
  _dump_slack(os);
}

// Function: _dump_slack
void Timer::_dump_slack(std::ostream& os) const {

  os << "Slack [pins:" << _pins.size() << "]\n";

  if(!_pins.empty())  {

    // find the maximum pin name
    auto plen = _max_pin_name_size();

    os << std::setfill('-') << std::setw(49 + plen) << '\n'
        << std::setfill(' ') << std::setw(10) << "E/R"  
                             << std::setw(12) << "E/F"
                             << std::setw(12) << "L/R"
                             << std::setw(12) << "L/F" 
                             << std::setw(2 + plen)  << "Pin"   << '\n'
        << std::setfill('-') << std::setw(49 + plen) << '\n';

    os << std::setfill(' ') << std::fixed << std::setprecision(3);
    for(const auto& kvp : _pins) {

      const auto& pin = kvp.second;
      
      FOR_EACH_EL_RF(el, rf) {
        os << std::setw(10);
        if(auto slack = pin.slack(el, rf); slack) os << *slack;
        else os << "n/a";
        os << "  ";
      }

      os << std::setw(plen) << pin._name << '\n';
    }
    os << std::setfill('-') << std::setw(49 + plen) << '\n';
  }
}

// Function: dump_at
void Timer::dump_at(std::ostream& os) const {
  std::shared_lock lock(_mutex);
  _dump_at(os);
}

// Function: _dump_at
void Timer::_dump_at(std::ostream& os) const {

  os << "Arrival time [pins:" << _pins.size() << "]\n";

  if(!_pins.empty())  {

    // find the maximum pin name
    auto plen = _max_pin_name_size();

    os << std::setfill('-') << std::setw(49 + plen) << '\n'
        << std::setfill(' ') << std::setw(10) << "E/R"  
                             << std::setw(12) << "E/F"
                             << std::setw(12) << "L/R"
                             << std::setw(12) << "L/F" 
                             << std::setw(2 + plen)  << "Pin"   << '\n'
        << std::setfill('-') << std::setw(49 + plen) << '\n';

    os << std::setfill(' ') << std::fixed << std::setprecision(3);
    for(const auto& kvp : _pins) {

      const auto& pin = kvp.second;
      
      FOR_EACH_EL_RF(el, rf) {
        os << std::setw(10);
        if(auto at = pin.at(el, rf); at) os << *at;
        else os << "n/a";
        os << "  ";
      }

      os << std::setw(plen) << pin._name << '\n';
    }
    os << std::setfill('-') << std::setw(49 + plen) << '\n';
  }
}

// Function: dump_rat
void Timer::dump_rat(std::ostream& os) const {
  std::shared_lock lock(_mutex);
  _dump_rat(os);
}

// Function: _dump_rat
void Timer::_dump_rat(std::ostream& os) const {
  
  os << "Required arrival time [pins:" << _pins.size() 
      << "]\n";

  if(!_pins.empty())  {

    // find the maximum pin name
    auto plen = _max_pin_name_size();

    os << std::setfill('-') << std::setw(49 + plen) << '\n'
        << std::setfill(' ') << std::setw(10) << "E/R"  
                             << std::setw(12) << "E/F"
                             << std::setw(12) << "L/R"
                             << std::setw(12) << "L/F" 
                             << std::setw(2 + plen)  << "Pin"   << '\n'
        << std::setfill('-') << std::setw(49 + plen) << '\n';

    os << std::setfill(' ') << std::fixed << std::setprecision(3);
    for(const auto& kvp : _pins) {

      const auto& pin = kvp.second;
      
      FOR_EACH_EL_RF(el, rf) {
        os << std::setw(10);
        if(auto rat = pin.rat(el, rf); rat) os << *rat;
        else os << "n/a";
        os << "  ";
      }

      os << std::setw(plen) << pin._name << '\n';
    }
    os << std::setfill('-') << std::setw(49 + plen) << '\n';
  }
}

// Function: dump_cell
void Timer::dump_cell(std::ostream& os, const std::string& name, Split el) const {
  std::shared_lock lock(_mutex);
  _dump_cell(os, name, el);
}

// Function: _dump_cell
void Timer::_dump_cell(std::ostream& os, const std::string& name, Split el) const {
  if(_celllib[el]) {
    if(auto ptr = _celllib[el]->cell(name); ptr) {
      os << *ptr;
    }
    else {
      os << "cell not found\n";
    }
  }
  else {
    os << "celllib not found\n";
  }
}

// Function: dump_celllib
void Timer::dump_celllib(std::ostream& os, Split el) const {
  std::shared_lock lock(_mutex);
  _dump_celllib(os, el);
}

// Function: _dump_celllib
void Timer::_dump_celllib(std::ostream& os, Split el) const {
  if(_celllib[el]) {
    os << *_celllib[el];
  }
  else {
    os << "celllib not found\n";
  }
}
    
};  // end of namespace ot. -----------------------------------------------------------------------



