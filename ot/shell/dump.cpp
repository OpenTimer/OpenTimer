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
  read_celllib       [-min|-max] <file>\n\
  read_verilog       <file>\n\
  read_spef          <file>\n\
  read_sdc           <file>\n\
  read_timing        <file>\n\
  set_slew           -pin name [-min|-max] [-rise|-fall] <value>\n\
  set_at             -pin name [-min|-max] [-rise|-fall] <value>\n\
  set_rat            -pin name [-min|-max] [-rise|-fall] <value>\n\
  set_load           -pin name [-min|-max] [-rise|-fall] <value>\n\
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
  report_slew        -pin name [-min|-max] [-rise|-fall]\n\
  report_at          -pin name [-min|-max] [-rise|-fall]\n\
  report_rat         -pin name [-min|-max] [-rise|-fall]\n\
  report_slack       -pin name [-min|-max] [-rise|-fall]\n\
  report_wns\n\
  report_tns\n\
  report_fep\n\
\n[Dump] operations to inspect the timer\n\n\
  help\n\
  version\n\
  dump_graph         [-o <file>]\n\
  dump_celllib       [-o <file>] [-min|-max] [-cell <name>]\n\
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
    _timer.dump_timer(_os);
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      _timer.dump_timer(ofs);
    }
    else {
      _es << "failed to open " << output << '\n';
    }
  }
}

// Procedure: dump_taskflow
void Shell::_dump_taskflow() {

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
    _timer.dump_taskflow(_os);
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      _timer.dump_taskflow(ofs);
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
    _timer.dump_graph(_os);
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      _timer.dump_graph(ofs);
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
    _timer.dump_net_load(_os);
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      _timer.dump_net_load(ofs);
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
    _timer.dump_pin_cap(_os);
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      _timer.dump_pin_cap(ofs);
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
    _timer.dump_slack(_os);
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      _timer.dump_slack(ofs);
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
    _timer.dump_rat(_os);
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      _timer.dump_rat(ofs);
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
    _timer.dump_slew(_os);
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      _timer.dump_slew(ofs);
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
    _timer.dump_at(_os);
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      _timer.dump_at(ofs);
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
  Split el = MIN;
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
    else if(token == "-min" || token == "-early") {
      el = MIN;
    }
    else if(token == "-max" || token == "-late") {
      el = MAX;
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
    _timer.dump_celllib(*tgt, el);
  }
  else {
    _timer.dump_cell(*tgt, cell, el);
  }
}

// ----------------------------------------------------------------------------

// Procedure: _dump_verilog
void Shell::_dump_verilog() {

  std::string token;
  std::string name;
  std::filesystem::path output;

  while(_is >> token) {
    if(token == "-o") {
      if(!(_is >> output)){
        _es << "output file not given\n";
        return;
      }
    }
    else if(token == "-name") {
      if(!(_is >> name)) {
        _es << "module name not given\n";
        return;
      }
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

  _timer.dump_verilog(*tgt, name);
}

// ----------------------------------------------------------------------------

// Procedure: _dump_spef
void Shell::_dump_spef() {

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

  _timer.dump_spef(*tgt);
}

// ----------------------------------------------------------------------------

// Procedure: _dump_rctree
void Shell::_dump_rctree() {

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

  _timer.dump_rctree(*tgt);
}

};  // end of namespace ot. ---------------------------------------------------





