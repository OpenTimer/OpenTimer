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

  _prompt.autocomplete("quit");
  _prompt.autocomplete("exit");

}
  
// Operator: ()
void Shell::operator()() {  

  bool quit {false};
  
  while(!quit && _prompt.readline(_line)) {

    // Remove the comment
    if(auto cpos = _line.find('#'); cpos != std::string::npos) {
      _line.erase(cpos);
    }

    _is.clear();
    _is.str(_line);

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
    else {
      _es << "undefined command: " << std::quoted(_line) << '\n';
    }

    // flush
    _os.flush();
    _es.flush();
  }
}

};  // end of namespace ot. -----------------------------------------------------------------------




