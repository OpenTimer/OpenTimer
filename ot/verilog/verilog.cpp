#include <ot/verilog/verilog.hpp>

namespace ot::vlog {

// Operator <<
std::ostream& operator << (std::ostream& os, const Gate& inst) {
  os << inst.cell << ' ' << inst.name << " ( ";
  size_t i {0};
  for(const auto& [pin, net] : inst.cellpin2net) {
    if(i++ != 0) {
      os << ", ";
    }
    os << '.' << pin << '(' << net << ')';
  }
  os << " );";
  return os;
}

// ------------------------------------------------------------------------------------------------

// Operator: <<
std::ostream& operator << (std::ostream& os, const Module& m) {

  os << "module " << m.name << " (\n";  

  for(size_t i=0; i<m.ports.size(); ++i) {
    os << m.ports[i];
    if(i != m.ports.size() - 1) {
      os << ",\n";
    }
  }
  os << ");\n\n";

  os << "// Start PIs\n";
  for(const auto& pi : m.inputs) {
    os << "input " << pi << ";\n";
  }
  os << '\n';
  
  os << "// Start POs\n";
  for(const auto& po : m.outputs) {
    os << "output " << po << ";\n";
  }
  os << '\n';
  
  os << "// Start wires\n"; 
  for(const auto& wire : m.wires) {
    os << "wire " << wire << ";\n";
  }
  os << '\n';

  os << "// Start cells\n";
  for(const auto& gate : m.gates) {
    os << gate << '\n';
  }
  os << '\n';

  os << "endmodule";
  
  return os;
}

// ------------------------------------------------------------------------------------------------

// Function: info
std::string Module::info() const {
  return "verilog module \""s + name + "\" " + 
         "[gates:" + std::to_string(gates.size()) + "]";
}

// Procedure: read_verilog
Module read_verilog(const std::filesystem::path& path) {

  Module module;
  
  static std::string_view delimiters = "(),:;/#[]{}*\"\\";
  static std::string_view exceptions = "().;";
  
  auto tokens = tokenize(path, delimiters, exceptions);

  //for(const auto& token : tokens) {
  //  std::cout << token << std::endl;
  //}

  // Set up the iterator
  auto itr = tokens.begin();
  auto end = tokens.end();

  // Read the module name
  if(itr = std::find(itr, end, "module"); itr == end) {
    OT_LOGF("can't find keyword 'module'");
  }
  else {
    if(++itr == end) {
      OT_LOGF("syntax error in module name");
    }
    module.name = std::move(*itr);
  }

  while(++itr != end && *itr != ";") {
    if(*itr != "(" && *itr != ")") {
      module.ports.push_back(std::move(*itr));
    }
  }

  // Parse the content.
  while(++itr != end) {
    
    if(*itr == "endmodule") {
      break;
    }
    else if(*itr == "input") {
      while(++itr != end && *itr != ";") {
        module.inputs.push_back(std::move(*itr));
      }
    }
    else if(*itr == "output") {
      while(++itr != end && *itr != ";") {
        module.outputs.push_back(std::move(*itr));
      }
    }
    else if(*itr == "wire") {
      while(++itr != end && *itr != ";") {
        module.wires.push_back(std::move(*itr));
      }
    }
    else {
      
      Gate inst;
      inst.cell = std::move(*itr);

      if(++itr == end) {
        OT_LOGF("syntax error in cell ", inst.cell, ")");
      }
      inst.name = std::move(*itr);

      // Read the mapping
      std::string cellpin;
      std::string net;

      itr = on_next_parentheses(itr, end, [&] (auto& str) mutable { 
        if(str == ")" || str == "(") {
          return;
        }
        else if(str[0] == '.') {
          cellpin = str.substr(1);
        }
        else {
          net = str; 
          inst.cellpin2net[cellpin] = net;
          inst.net2cellpin[net] = cellpin;
        }
      });

      if(itr == end) {
        OT_LOGF("syntax error in gate pin-net mapping");
      }

      if(*(++itr) != ";") {
        OT_LOGF("missing ; in instance declaration");
      }
      
      module.gates.push_back(std::move(inst));
    }
  }
  
  return module;
}



};  // end of namespace ot. -----------------------------------------------------------------------
