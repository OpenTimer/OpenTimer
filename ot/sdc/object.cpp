#include <ot/sdc/object.hpp>

namespace ot::sdc {

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
  //auto num = std::distance(itr, end);

  GetPorts get_ports;

  for(; itr != end; ++itr) {
    get_ports.ports.push_back(itr->str());
  }
  
  return get_ports;
}

};  // end of namespace sdc. ----------------------------------------------------------------------
