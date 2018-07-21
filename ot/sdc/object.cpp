#include <ot/sdc/object.hpp>

namespace ot::sdc {

// Operator << (Port)
std::ostream& operator << (std::ostream& os, const Port& port) {
  
  std::visit(Functors{
    [&] (const AllInputs&) {
      os << "[all_inputs]";
    },
    [&] (const AllOutputs&) {
      os << "[all_outputs]";
    },
    [&] (const GetPorts& get_ports) {
      if(const auto& ports = get_ports.ports; !ports.empty()) {
        os << "[get_ports {";
        for(size_t i=0; i<ports.size(); ++i) {
          if(i != 0) {
            os << ' ';
          }
          // add another brace for array
          if(is_array(ports[i])) {
            os << '{' << ports[i] << '}';
          }
          else {
            os << ports[i];
          }
        }
        os << "}]";
      }
    }
  }, port);

  return os;
}

// ------------------------------------------------------------------------------------------------

// Operator << (Clock)
std::ostream& operator << (std::ostream& os, const Clock& clock) {

  std::visit(Functors{
    [&] (const AllClocks&) {
      os << "all_clocks\n";
    }, 
    [&] (const GetClocks& get_clocks) {
      if(const auto& clocks = get_clocks.clocks; !clocks.empty()) {
        os << "[get_clocks {";
        for(size_t i=0; i<clocks.size(); ++i) {
          if(i != 0) {
            os << ' ';
          }
          // add another brace for array
          if(is_array(clocks[i])) {
            os << '{' << clocks[i] << '}';
          }
          else {
            os << clocks[i];
          }
        }
        os << "}]";
      }
    }
  }, clock);

  return os;
}

// ------------------------------------------------------------------------------------------------

// Function: extract_clock
Clock extract_clock(const std::string& line) {
  auto tokens = tokenize(line);
  auto beg = tokens.begin();
  return extract_clock(beg, tokens.end());
}

// Function: extract_clock
// [ get_clocks clk1 clk2 ] or clk or {clk1 clk2}
Clock extract_clock(token_iter_t& beg, token_iter_t end) {

  Clock clock;
  
  // Implicit design access
  if(*beg == "{") {
    auto& clocks = clock.emplace<GetClocks>().clocks;
    for(++beg; beg != end && *beg != "}"; ++beg) {
      clocks.push_back(std::move(*beg));
    }
    OT_LOGF_IF(beg==end || *beg != "}", "can't find brace pair { }");
    return clock;
  }

  if(*beg != "[") {
    clock.emplace<GetClocks>().clocks.push_back(std::move(*beg));
    return clock;
  }

  // Explicit design access

  // all_clocks
  if(auto& token = *(++beg); token == "all_clocks") {
    clock.emplace<AllClocks>();
    ++beg;
  }
  // get_clocks
  else if(token == "get_clocks") {
    auto& clocks = clock.emplace<GetClocks>().clocks;
    while(++beg < end && *beg != "]") {
      clocks.push_back(std::move(*beg));
    }
  }
  else {
    OT_LOGF("unexpected port access token ", token);
  }
  
  OT_LOGF_IF(beg == end || *beg != "]", "can't find bracket pair [ ]");

  return clock;
}

// ------------------------------------------------------------------------------------------------

// Function: extract_port
Port extract_port(const std::string& line) {
  auto tokens = tokenize(line);
  auto beg = tokens.begin();
  return extract_port(beg, tokens.end());
}

// Function: extract_port
// {pin1 pin2} or pin1 or [ get_port pin1 pin2 ] 
Port extract_port(token_iter_t& beg, token_iter_t end) {

  Port port;
  
  // Implicit design access
  if(*beg == "{") {
    auto& ports = port.emplace<GetPorts>().ports;
    for(++beg; beg != end && *beg != "}"; ++beg) {
      ports.push_back(std::move(*beg));
    }
    OT_LOGF_IF(beg==end || *beg != "}", "can't find brace pair { }");
    return port;
  }

  if(*beg != "[") {
    port.emplace<GetPorts>().ports.push_back(std::move(*beg));
    return port;
  }
  
  // Explicit design access command []

  // all inputs
  if(auto& token = *(++beg); token == "all_inputs") {
    port.emplace<AllInputs>();
    ++beg;
  }
  // all outputs
  else if(token == "all_outputs") {
    port.emplace<AllOutputs>();
    ++beg;
  }
  // get_ports
  else if(token == "get_ports") {
    auto& ports = port.emplace<GetPorts>().ports;
    while(++beg < end && *beg != "]") {
      ports.push_back(std::move(*beg));
    }
  }
  else {
    OT_LOGF("unexpected port access token ", token);
  }

  OT_LOGF_IF(beg == end || *beg != "]", "can't find bracket pair [ ]");

  return port;
}

};  // end of namespace sdc. ----------------------------------------------------------------------
