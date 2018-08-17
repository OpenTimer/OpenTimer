#include <ot/shell/shell.hpp>

namespace ot {

// Procedure: dump_version
void Shell::_dump_version() {
  _os << OT_VERSION << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: dump_license
void Shell::_dump_license() {

  static std::string_view license = "\n\
MIT License\n\
\n\
Copyright (c) 2018 Dr. Tsung-Wei Huang and Dr. Martin Wong\n\
\n\
The University of Illinois at Urbana-Champaign, IL, USA\n\
\n\
Permission is hereby granted, free of charge, to any person obtaining a copy\n\
of this software and associated documentation files (the \"Software\"), to deal\n\
in the Software without restriction, including without limitation the rights\n\
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n\
copies of the Software, and to permit persons to whom the Software is\n\
furnished to do so, subject to the following conditions:\n\
\n\
The above copyright notice and this permission notice shall be included in all\n\
copies or substantial portions of the Software.\n\
\n\
THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n\
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n\
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n\
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n\
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n\
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n\
SOFTWARE.\n";

  _os << license << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: dump_help
void Shell::_dump_help() {

  // help message
  static std::string_view help = "\n\
List of commonly used commands:\n\
\n[Builder] operations to build the timer\n\n\
  set_num_threads    <N>\n\
  read_celllib       [-early|-late] <file>\n\
  read_verilog       <file>\n\
  read_spef          <file>\n\
  read_sdc           <file>\n\
  read_timing        <file>\n\
  set_slew           -pin name [-early|-late] [-rise|-fall] <value>\n\
  set_at             -pin name [-early|-late] [-rise|-fall] <value>\n\
  set_rat            -pin name [-early|-late] [-rise|-fall] <value>\n\
  set_load           -pin name [-early|-late] [-rise|-fall] <value>\n\
  insert_gate        <gate> <cell>\n\
  repower_gate       <gate> <cell>\n\
  remove_gate        <gate>\n\
  insert_net         <net>\n\
  remove_net         <net>\n\
  disconnect_pin     <pin>\n\
  connect_pin        <pin> <net>\n\
  cppr               -disable|-enable\n\
\n[Action] operations to update the timer\n\n\
  update_timing\n\
  report_timing\n\
  report_path        [-num_paths <N>]\n\
  report_slew        -pin name [-early|-late] [-rise|-fall]\n\
  report_at          -pin name [-early|-late] [-rise|-fall]\n\
  report_rat         -pin name [-early|-late] [-rise|-fall]\n\
  report_slack       -pin name [-early|-late] [-rise|-fall]\n\
  report_wns\n\
  report_tns\n\
  report_fep\n\
\n[Dump] operations to inspect the timer\n\n\
  help\n\
  version\n\
  dump_graph         [-o <file>]\n\
  dump_celllib       [-o <file>] [-early|-late] [-cell <name>]\n\
  dump_net_load      [-o <file>]\n\
  dump_pin_cap       [-o <file>]\n\
  dump_slew          [-o <file>]\n\
  dump_at            [-o <file>]\n\
  dump_rat           [-o <file>]\n\
  dump_slack         [-o <file>]\n\
  dump_timer         [-o <file>]\n\n\
For more information, consult the manual at\n\
<https://github.com/OpenTimer/OpenTimer>.\n";

  _os << help << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: dump_timer
void Shell::_dump_timer() {

  std::string token;
  std::filesystem::path output;

  while(_is >> token) {
    if(token == "-o") {
      if(!(_is >> output)) {
        _es << "output file not given\n";
        return;
      }
    }
  }

  if(output.empty()) {
    _os << _timer.dump_timer();
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      ofs << _timer.dump_timer();
    }
    else {
      _es << "failed to open " << output << '\n';
    }
  }
}

// Procedure: dump_graph
void Shell::_dump_graph() {

  std::string token;
  std::filesystem::path output;

  while(_is >> token) {
    if(token == "-o") {
      if(!(_is >> output)) {
        _es << "output file not given\n";
        return;
      }
    }
  }
  
  if(output.empty()) {
    _os << _timer.dump_graph();
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      ofs << _timer.dump_graph();
    }
    else {
      _es << "failed to open " << output << '\n';
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: dump_net_load
void Shell::_dump_net_load() {

  std::string token;
  std::filesystem::path output;

  while(_is >> token) {
    if(token == "-o") {
      if(!(_is >> output)){
        _es << "output file not given\n";
        return;
      }
    }
    else {
      _es << "unexpected token " << token << '\n';
    }
  }
  
  if(output.empty()) {
    _os << _timer.dump_net_load();
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      ofs << _timer.dump_net_load();
    }
    else {
      _es << "failed to open " << output << '\n';
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: dump_pin_cap
void Shell::_dump_pin_cap() {

  std::string token;
  std::filesystem::path output;

  while(_is >> token) {
    if(token == "-o") {
      if(!(_is >> output)){
        _es << "output file not given\n";
        return;
      }
    }
    else {
      _es << "unexpected token " << token << '\n';
    }
  }
  
  if(output.empty()) {
    _os << _timer.dump_pin_cap();
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      ofs << _timer.dump_pin_cap();
    }
    else {
      _es << "failed to open " << output << '\n';
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: dump_slack
void Shell::_dump_slack() {

  std::string token;
  std::filesystem::path output;

  while(_is >> token) {
    if(token == "-o") {
      if(!(_is >> output)){
        _es << "output file not given\n";
        return;
      }
    }
    else {
      _es << "unexpected token " << token << '\n';
    }
  }
  
  if(output.empty()) {
    _os << _timer.dump_slack();
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      ofs << _timer.dump_slack();
    }
    else {
      _es << "failed to open " << output << '\n';
    }
  }
}

// Procedure: dump_rat
void Shell::_dump_rat() {

  std::string token;
  std::filesystem::path output;

  while(_is >> token) {
    if(token == "-o") {
      if(!(_is >> output)){
        _es << "output file not given\n";
        return;
      }
    }
    else {
      _es << "unexpected token " << token << '\n';
    }
  }
  
  if(output.empty()) {
    _os << _timer.dump_rat();
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      ofs << _timer.dump_rat();
    }
    else {
      _es << "failed to open " << output << '\n';
    }
  }
}

// Procedure: dump_slew
void Shell::_dump_slew() {

  std::string token;
  std::filesystem::path output;

  while(_is >> token) {
    if(token == "-o") {
      if(!(_is >> output)){
        _es << "output file not given\n";
        return;
      }
    }
    else {
      _es << "unexpected token " << token << '\n';
    }
  }
  
  if(output.empty()) {
    _os << _timer.dump_slew();
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      ofs << _timer.dump_slew();
    }
    else {
      _es << "failed to open " << output << '\n';
    }
  }
}

// Procedure: dump_at
void Shell::_dump_at() {

  std::string token;
  std::filesystem::path output;

  while(_is >> token) {
    if(token == "-o") {
      if(!(_is >> output)){
        _es << "output file not given\n";
        return;
      }
    }
    else {
      _es << "unexpected token " << token << '\n';
    }
  }
  
  if(output.empty()) {
    _os << _timer.dump_at();
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      ofs << _timer.dump_at();
    }
    else {
      _es << "failed to open " << output << '\n';
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: dump_celllib
void Shell::_dump_celllib() {

  std::string token;
  std::string cell;
  Split el = EARLY;
  std::filesystem::path output;

  while(_is >> token) {
    if(token == "-o") {
      if(!(_is >> output)){
        _es << "output file not given\n";
        return;
      }
    }
    else if(token == "-cell") {
      if(!(_is >> cell)) {
        _es << "cell name not given\n";
        return;
      }
    }
    else if(token == "-early") {
      el = EARLY;
    }
    else if(token == "-late") {
      el = LATE;
    }
    else {
      _es << "unexpected token " << token << '\n';
    }
  }

  std::ostream* tgt = &_os;
  std::ofstream ofs;
  
  if(!output.empty()) {
    if(ofs.open(output); ofs) {
      tgt = &ofs;
    }
    else {
      _es << "failed to open " << output << '\n';
    }
  }

  if(cell.empty()) {
    *tgt << _timer.dump_celllib(el);
  }
  else {
    *tgt << _timer.dump_cell(cell, el);
  }
}


};  // end of namespace ot. -----------------------------------------------------------------------





