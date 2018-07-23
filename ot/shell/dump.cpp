#include <ot/shell/shell.hpp>

namespace ot {

// Procedure: dump_version
void dump_version(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {
  os << OT_VERSION << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: dump_license
void dump_license(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  static std::string_view license = "\n\
MIT License\n\
\n\
Copyright (c) 2018 Tsung-Wei Huang and Martin D. F. Wong\n\
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

  os << license << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: dump_contributors
void dump_contributors(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  // contributor list
  static std::string_view contributors = "\n\
OpenTimer contributor list:\n\n\
  Tsung-Wei Huang    <twh760812@gmail.com>\n\
  Martin Wong        <mdfwong@illinois.edu>\n\
  Chun-Xun Lin       <clin99@illinois.edu>\n\
  Kunal Ghosh        <kunalpghosh@gmail.com>\n\
  Pei-Yu Lee         <palacedeforsaken@gmail.com>\n\
  Tin-Yin Lai        <tinyinlai@gmail.com>\n\
  Jin Hu             <jinhu@umich.edu>\n\
  Myung-Chul Kim     <mckima@us.ibm.com>\n\
  George Chen        <george@geochrist.com>\n\
  Pao-I Chen         <poyipenny@gmail.com>\n\
\nPlease let me know <twh760812@gmail.com> if I forgot someone!\n";
  
  os << contributors << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: dump_help
void dump_help(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  // help message
  static std::string_view help = "\n\
List of commonly used commands:\n\
\n[Builder] operations to build the timer\n\n\
  set_units          -[time|power|resistance|capacitance|voltage|current] <unit>\n\
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
  report_slew        -pin name [-early|-late] [-rise|-fall]\n\
  report_at          -pin name [-early|-late] [-rise|-fall]\n\
  report_rat         -pin name [-early|-late] [-rise|-fall]\n\
  report_slack       -pin name [-early|-late] [-rise|-fall]\n\
\n[Dump] operations to inspect the timer\n\n\
  help\n\
  version\n\
  contributors\n\
  dump_graph         [-o <file>]\n\
  dump_celllib       [-o <file>] [-early|-late] [-cell <name>]\n\
  dump_net_load      [-o <file>]\n\
  dump_pin_cap       [-o <file>]\n\
  dump_slack         [-o <file>]\n\
  dump_timer         [-o <file>]\n\n\
For more information, consult the manual at\n\
<https://github.com/OpenTimer/OpenTimer>.\n";

  os << help << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: dump_timer
void dump_timer(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  std::string token;
  std::filesystem::path output;

  while(is >> token) {
    if(token == "-o") {
      if(!(is >> output)) {
        es << "output file not given\n";
        return;
      }
    }
  }

  if(output.empty()) {
    os << timer.dump_timer();
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      ofs << timer.dump_timer();
    }
    else {
      es << "failed to open " << output << '\n';
    }
  }
}

// Procedure: dump_graph
void dump_graph(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  std::string token;
  std::filesystem::path output;

  while(is >> token) {
    if(token == "-o") {
      if(!(is >> output)) {
        es << "output file not given\n";
        return;
      }
    }
  }
  
  if(output.empty()) {
    os << timer.dump_graph();
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      ofs << timer.dump_graph();
    }
    else {
      es << "failed to open " << output << '\n';
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: dump_net_load
void dump_net_load(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  std::string token;
  std::filesystem::path output;

  while(is >> token) {
    if(token == "-o") {
      if(!(is >> output)){
        es << "output file not given\n";
        return;
      }
    }
    else {
      es << "unexpected token " << token << '\n';
    }
  }
  
  if(output.empty()) {
    os << timer.dump_net_load();
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      ofs << timer.dump_net_load();
    }
    else {
      es << "failed to open " << output << '\n';
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: dump_pin_cap
void dump_pin_cap(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  std::string token;
  std::filesystem::path output;

  while(is >> token) {
    if(token == "-o") {
      if(!(is >> output)){
        es << "output file not given\n";
        return;
      }
    }
    else {
      es << "unexpected token " << token << '\n';
    }
  }
  
  if(output.empty()) {
    os << timer.dump_pin_cap();
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      ofs << timer.dump_pin_cap();
    }
    else {
      es << "failed to open " << output << '\n';
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: dump_slack
void dump_slack(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  std::string token;
  std::filesystem::path output;

  while(is >> token) {
    if(token == "-o") {
      if(!(is >> output)){
        es << "output file not given\n";
        return;
      }
    }
    else {
      es << "unexpected token " << token << '\n';
    }
  }
  
  if(output.empty()) {
    os << timer.dump_slack();
  }
  else {
    if(std::ofstream ofs(output); ofs) {
      ofs << timer.dump_slack();
    }
    else {
      es << "failed to open " << output << '\n';
    }
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: dump_celllib
void dump_celllib(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  std::string token;
  std::string cell;
  Split el = EARLY;
  std::filesystem::path output;

  while(is >> token) {
    if(token == "-o") {
      if(!(is >> output)){
        es << "output file not given\n";
        return;
      }
    }
    else if(token == "-cell") {
      if(!(is >> cell)) {
        es << "cell name not given\n";
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
      es << "unexpected token " << token << '\n';
    }
  }

  std::ostream* tgt = &os;
  std::ofstream ofs;
  
  if(!output.empty()) {
    if(ofs.open(output); ofs) {
      tgt = &ofs;
    }
    else {
      es << "failed to open " << output << '\n';
    }
  }

  if(cell.empty()) {
    *tgt << timer.dump_celllib(el);
  }
  else {
    *tgt << timer.dump_cell(cell, el);
  }
}


};  // end of namespace ot. -----------------------------------------------------------------------





