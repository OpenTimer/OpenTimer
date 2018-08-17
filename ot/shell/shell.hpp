#ifndef OT_SHELL_SHELL_HPP_
#define OT_SHELL_SHELL_HPP_

#include <ot/timer/timer.hpp>
#include <ot/shell/prompt.hpp>

namespace ot {

// Builder
void set_units              (Timer&, std::istream&, std::ostream&, std::ostream&);
void set_num_threads        (Timer&, std::istream&, std::ostream&, std::ostream&);
void read_verilog           (Timer&, std::istream&, std::ostream&, std::ostream&);      
void read_spef              (Timer&, std::istream&, std::ostream&, std::ostream&);         
void read_celllib           (Timer&, std::istream&, std::ostream&, std::ostream&);
void read_sdc               (Timer&, std::istream&, std::ostream&, std::ostream&);
void read_timing            (Timer&, std::istream&, std::ostream&, std::ostream&);
void set_at                 (Timer&, std::istream&, std::ostream&, std::ostream&); 
void set_slew               (Timer&, std::istream&, std::ostream&, std::ostream&);
void set_load               (Timer&, std::istream&, std::ostream&, std::ostream&);
void set_rat                (Timer&, std::istream&, std::ostream&, std::ostream&); 
void insert_gate            (Timer&, std::istream&, std::ostream&, std::ostream&);
void remove_gate            (Timer&, std::istream&, std::ostream&, std::ostream&);
void repower_gate           (Timer&, std::istream&, std::ostream&, std::ostream&);
void insert_net             (Timer&, std::istream&, std::ostream&, std::ostream&);
void remove_net             (Timer&, std::istream&, std::ostream&, std::ostream&);
void disconnect_pin         (Timer&, std::istream&, std::ostream&, std::ostream&);
void connect_pin            (Timer&, std::istream&, std::ostream&, std::ostream&);
void cppr                   (Timer&, std::istream&, std::ostream&, std::ostream&);

// Action
void update_timing          (Timer&, std::istream&, std::ostream&, std::ostream&);
void report_timing          (Timer&, std::istream&, std::ostream&, std::ostream&);
void report_path            (Timer&, std::istream&, std::ostream&, std::ostream&);
void report_at              (Timer&, std::istream&, std::ostream&, std::ostream&);
void report_slew            (Timer&, std::istream&, std::ostream&, std::ostream&);
void report_rat             (Timer&, std::istream&, std::ostream&, std::ostream&);
void report_slack           (Timer&, std::istream&, std::ostream&, std::ostream&);
void report_tns             (Timer&, std::istream&, std::ostream&, std::ostream&);
void report_wns             (Timer&, std::istream&, std::ostream&, std::ostream&);
void report_fep             (Timer&, std::istream&, std::ostream&, std::ostream&);
void report_area            (Timer&, std::istream&, std::ostream&, std::ostream&);
void report_leakage_power   (Timer&, std::istream&, std::ostream&, std::ostream&);

// Dump
void dump_version           (Timer&, std::istream&, std::ostream&, std::ostream&);
void dump_help              (Timer&, std::istream&, std::ostream&, std::ostream&);
void dump_license           (Timer&, std::istream&, std::ostream&, std::ostream&);
void dump_graph             (Timer&, std::istream&, std::ostream&, std::ostream&);
void dump_celllib           (Timer&, std::istream&, std::ostream&, std::ostream&);
void dump_net_load          (Timer&, std::istream&, std::ostream&, std::ostream&);
void dump_pin_cap           (Timer&, std::istream&, std::ostream&, std::ostream&);
void dump_at                (Timer&, std::istream&, std::ostream&, std::ostream&);
void dump_rat               (Timer&, std::istream&, std::ostream&, std::ostream&);
void dump_slew              (Timer&, std::istream&, std::ostream&, std::ostream&);
void dump_slack             (Timer&, std::istream&, std::ostream&, std::ostream&);
void dump_timer             (Timer&, std::istream&, std::ostream&, std::ostream&);

// Obselete
void exec_ops               (Timer&, std::istream&, std::ostream&, std::ostream&);
void init_timer             (Timer&, std::istream&, std::ostream&, std::ostream&);
void set_early_celllib_fpath(Timer&, std::istream&, std::ostream&, std::ostream&);
void set_late_celllib_fpath (Timer&, std::istream&, std::ostream&, std::ostream&); 
void set_verilog_fpath      (Timer&, std::istream&, std::ostream&, std::ostream&);
void set_spef_fpath         (Timer&, std::istream&, std::ostream&, std::ostream&);
void set_timing_fpath       (Timer&, std::istream&, std::ostream&, std::ostream&);
void report_timer           (Timer&, std::istream&, std::ostream&, std::ostream&);

// Operation Map
inline const std::unordered_map<
  std::string, 
  void(*)(Timer&, std::istream&, std::ostream&, std::ostream&)
> commands {

  // Builder
  {"set_units",               set_units},
  {"set_num_threads",         set_num_threads},
  {"read_verilog",            read_verilog},
  {"read_spef",               read_spef},
  {"read_celllib",            read_celllib},
  {"read_sdc",                read_sdc},
  {"read_timing",             read_timing},
  {"set_at",                  set_at},
  {"set_slew",                set_slew},
  {"set_load",                set_load},
  {"set_rat",                 set_rat},
  {"insert_gate",             insert_gate},
  {"remove_gate",             remove_gate},
  {"repower_gate",            repower_gate},
  {"insert_net",              insert_net},
  {"remove_net",              remove_net},
  {"disconnect_pin",          disconnect_pin},
  {"connect_pin",             connect_pin},
  {"cppr",                    cppr},

  // Action
  {"update_timing",           update_timing},
  {"report_timing",           report_timing},
  {"report_path",             report_path},
  {"report_at",               report_at},
  {"report_slew",             report_slew},
  {"report_rat",              report_rat},
  {"report_slack",            report_slack},
  {"report_tns",              report_tns},
  {"report_wns",              report_wns},
  {"report_fep",              report_fep},
  {"report_area",             report_area},
  {"report_leakage_power",    report_leakage_power},

  // Dump
  {"help",                    dump_help},
  {"version",                 dump_version},
  {"license",                 dump_license},
  {"dump_graph",              dump_graph},
  {"dump_celllib",            dump_celllib},
  {"dump_net_load",           dump_net_load},
  {"dump_pin_cap",            dump_pin_cap},
  {"dump_slew",               dump_slew},
  {"dump_at",                 dump_at},
  {"dump_rat",                dump_rat},
  {"dump_slack",              dump_slack},
  {"dump_timer",              dump_timer},

  // obselete
  {"exec_ops",                exec_ops},
  {"init_timer",              init_timer},
  {"set_early_celllib_fpath", set_early_celllib_fpath},
  {"set_late_celllib_fpath",  set_late_celllib_fpath},
  {"set_verilog_fpath",       set_verilog_fpath},
  {"set_spef_fpath",          set_spef_fpath},
  {"set_timing_fpath",        set_timing_fpath},
  {"report_timer",            report_timer}
};

//-------------------------------------------------------------------------------------------------

// Bash
void ls    (std::string_view, std::istream&, std::ostream&, std::ostream&);
void cd    (std::string_view, std::istream&, std::ostream&, std::ostream&);
void pwd   (std::string_view, std::istream&, std::ostream&, std::ostream&);
void rm    (std::string_view, std::istream&, std::ostream&, std::ostream&);
void cat   (std::string_view, std::istream&, std::ostream&, std::ostream&);
void head  (std::string_view, std::istream&, std::ostream&, std::ostream&);
void tail  (std::string_view, std::istream&, std::ostream&, std::ostream&);
void clear (std::string_view, std::istream&, std::ostream&, std::ostream&);
void echo  (std::string_view, std::istream&, std::ostream&, std::ostream&);
void which (std::string_view, std::istream&, std::ostream&, std::ostream&);

inline std::unordered_map<
  std::string, 
  void(*)(std::string_view, std::istream&, std::ostream&, std::ostream&)
> syscmds {
  // Misc
  {"ls",                      ls},
  {"pwd",                     pwd},
  {"cd",                      cd},
  {"rm",                      rm},
  {"cat",                     cat},
  {"head",                    head},
  {"tail",                    tail},
  {"clear",                   clear},
  {"echo",                    echo},
  {"which",                   which}
};

// ------------------------------------------------------------------------------------------------
  
// Procedure: shell
// Main procedure to enter into a shell.
int shell(int, char*[]);

};  // end of namespace ot ------------------------------------------------------------------------

#endif




