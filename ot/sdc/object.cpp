#include <ot/sdc/object.hpp>

namespace ot::sdc {

//// Operator << (Port)
//std::ostream& operator << (std::ostream& os, const Port& port) {
//  
//  std::visit(Functors{
//    [&] (const AllInputs&) {
//      os << "[all_inputs]";
//    },
//    [&] (const AllOutputs&) {
//      os << "[all_outputs]";
//    },
//    [&] (const GetPorts& get_ports) {
//      if(const auto& ports = get_ports.ports; !ports.empty()) {
//        os << "[get_ports {";
//        for(size_t i=0; i<ports.size(); ++i) {
//          if(i != 0) {
//            os << ' ';
//          }
//          // add another brace for array
//          if(is_array(ports[i])) {
//            os << '{' << ports[i] << '}';
//          }
//          else {
//            os << ports[i];
//          }
//        }
//        os << "}]";
//      }
//    }
//  }, port);
//
//  return os;
//}
//
//// ------------------------------------------------------------------------------------------------
//
//// Operator << (Clock)
//std::ostream& operator << (std::ostream& os, const Clock& clock) {
//
//  std::visit(Functors{
//    [&] (const AllClocks&) {
//      os << "all_clocks\n";
//    }, 
//    [&] (const GetClocks& get_clocks) {
//      if(const auto& clocks = get_clocks.clocks; !clocks.empty()) {
//        os << "[get_clocks {";
//        for(size_t i=0; i<clocks.size(); ++i) {
//          if(i != 0) {
//            os << ' ';
//          }
//          // add another brace for array
//          if(is_array(clocks[i])) {
//            os << '{' << clocks[i] << '}';
//          }
//          else {
//            os << clocks[i];
//          }
//        }
//        os << "}]";
//      }
//    }
//  }, clock);
//
//  return os;
//}

// ------------------------------------------------------------------------------------------------

// Function: parse_port
Object parse_port(const std::string& line) {

  if(line.find("all_inputs") != std::string::npos) {
    return AllInputs{};
  }

  if(line.find("all_outputs") != std::string::npos) {
    return AllOutputs{};
  }

  const static std::regex ws_re("\\s+|\\n+|\\t+"); 
  
  auto itr = std::sregex_token_iterator(line.begin(), line.end(), ws_re, -1);
  auto end = std::sregex_token_iterator();
  auto num = std::distance(itr, end);

  GetPorts get_ports;

  for(; itr != end; ++itr) {
    get_ports.ports.push_back(itr->str());
  }
  
  return get_ports;
}

};  // end of namespace sdc. ----------------------------------------------------------------------
