#include <ot/shell/shell.hpp>

namespace ot {

// Constructor
Shell::Shell(const std::string& w, std::ostream& os, std::ostream& es) :
  _os     {os},
  _es     {es},
  _prompt {w, "ot> ", ot::user_home() / ".ot_history"} {
  
  // Add auto-complete
  for(const auto& kvp : _handles) {
    _prompt.autocomplete(kvp.first);
  }

  for(const auto& kvp : ot::syscmds) {
    _prompt.autocomplete(kvp.first);
  }

  _prompt.autocomplete("quit");
  _prompt.autocomplete("exit");

}
  
// Operator: ()
void Shell::operator()() {  

  std::string line;

  bool quit {false};
  
  while(!quit && _prompt.readline(line)) {

    // Remove the comment
    if(auto cpos = line.find('#'); cpos != std::string::npos) {
      line.erase(cpos);
    }

    _is.clear();
    _is.str(line);

    std::string op;
    _is >> op;
    
    // Nothing to do with empty line
    if(op.empty()) {
    }
    // leave
    else if(op == "quit" || op == "exit") {
      quit = true; 
    }
    // built-in command
    else if(auto itr = _handles.find(op); itr != _handles.end()) {
      (this->*(itr->second))();
    } 
    // system command
    else if(auto itr = ot::syscmds.find(op); itr != ot::syscmds.end()) {
      std::invoke(itr->second, line, _is, _os, _es);
    }
    else {
      _es << "undefined command: " << std::quoted(line) << '\n';
    }

    // flush
    _os.flush();
    _es.flush();
  }
}

};  // end of namespace ot. -----------------------------------------------------------------------




