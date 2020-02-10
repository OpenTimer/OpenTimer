#ifndef OT_SHELL_SHELL_HPP_
#define OT_SHELL_SHELL_HPP_

#include <ot/timer/timer.hpp>
#include <ot/shell/prompt.hpp>

namespace ot {

// welcome message
inline const std::string welcome = "\
  ____              _______              \n\
 / __ \\___  ___ ___/_  __(_)_ _  ___ ____\n\
/ /_/ / _ \\/ -_) _ \\/ / / /  ' \\/ -_) __/\n\
\\____/ .__/\\__/_//_/_/ /_/_/_/_/\\__/_/       v"s + OT_VERSION + "\n" + "\
    /_/                                     \n\
MIT License: type \"license\" to see more details.\n\
For help, type \"help\".\n\
For bug reports, issues, and manual, please see:\n\
<https://github.com/OpenTimer/OpenTimer>.\n";

// ------------------------------------------------------------------------------------------------

// Class: Shell
class Shell {
  
  public:

    Shell(const std::string&, FILE*, std::ostream&, std::ostream&);

    void operator ()();

  private:

    Timer _timer;
    
    std::ostream& _os;
    std::ostream& _es;
  
    prompt::Prompt _prompt;
  
    std::string _line;
    std::string _op;
    std::istringstream _is;
    
    // builder
    void _set_units              ();
    void _set_num_threads        ();
    void _read_verilog           ();      
    void _read_spef              ();         
    void _read_celllib           ();
    void _read_sdc               ();
    void _read_timing            ();
    void _set_at                 (); 
    void _set_slew               ();
    void _set_load               ();
    void _set_rat                (); 
    void _insert_gate            ();
    void _remove_gate            ();
    void _repower_gate           ();
    void _insert_net             ();
    void _remove_net             ();
    void _disconnect_pin         ();
    void _connect_pin            ();
    void _cppr                   ();
    void _enable_cppr            ();
    void _disable_cppr           ();

    // action
    void _update_timing          ();
    void _report_timing          ();
    void _report_path            ();
    void _report_at              ();
    void _report_slew            ();
    void _report_rat             ();
    void _report_slack           ();
    void _report_tns             ();
    void _report_wns             ();
    void _report_fep             ();
    void _report_area            ();
    void _report_leakage_power   ();

    // Dump
    void _dump_version           ();
    void _dump_help              ();
    void _dump_license           ();
    void _dump_graph             ();
    void _dump_taskflow           ();
    void _dump_celllib           ();
    void _dump_net_load          ();
    void _dump_pin_cap           ();
    void _dump_at                ();
    void _dump_rat               ();
    void _dump_slew              ();
    void _dump_slack             ();
    void _dump_timer             ();
    void _dump_verilog           ();
    void _dump_spef              ();
    void _dump_rctree            ();
    
    // Obselete
    void _exec_ops               ();
    void _init_timer             ();
    void _set_early_celllib_fpath();
    void _set_late_celllib_fpath (); 
    void _set_verilog_fpath      ();
    void _set_spef_fpath         ();
    void _set_timing_fpath       ();
    void _report_timer           ();

    // System
    void _ls                     ();
    void _cd                     ();
    void _pwd                    ();
    void _rm                     ();
    void _cat                    ();
    void _head                   ();
    void _tail                   ();
    void _clear                  ();
    void _echo                   ();
    void _which                  ();
    
    std::unordered_map<std::string, void(Shell::*)()> _handles {
      // Builder
      {"set_units",               &Shell::_set_units},
      {"set_num_threads",         &Shell::_set_num_threads},
      {"read_verilog",            &Shell::_read_verilog},
      {"read_spef",               &Shell::_read_spef},
      {"read_celllib",            &Shell::_read_celllib},
      {"read_sdc",                &Shell::_read_sdc},
      {"read_timing",             &Shell::_read_timing},
      {"set_at",                  &Shell::_set_at},
      {"set_slew",                &Shell::_set_slew},
      {"set_load",                &Shell::_set_load},
      {"set_rat",                 &Shell::_set_rat},
      {"insert_gate",             &Shell::_insert_gate},
      {"remove_gate",             &Shell::_remove_gate},
      {"repower_gate",            &Shell::_repower_gate},
      {"insert_net",              &Shell::_insert_net},
      {"remove_net",              &Shell::_remove_net},
      {"disconnect_pin",          &Shell::_disconnect_pin},
      {"connect_pin",             &Shell::_connect_pin},
      {"cppr",                    &Shell::_cppr},
      {"enable_cppr",             &Shell::_enable_cppr},
      {"disable_cppr",            &Shell::_disable_cppr},

      // Action
      {"update_timing",           &Shell::_update_timing},
      {"report_timing",           &Shell::_report_timing},
      {"report_path",             &Shell::_report_path},
      {"report_at",               &Shell::_report_at},
      {"report_slew",             &Shell::_report_slew},
      {"report_rat",              &Shell::_report_rat},
      {"report_slack",            &Shell::_report_slack},
      {"report_tns",              &Shell::_report_tns},
      {"report_wns",              &Shell::_report_wns},
      {"report_fep",              &Shell::_report_fep},
      {"report_area",             &Shell::_report_area},
      {"report_leakage_power",    &Shell::_report_leakage_power},

      // Dump
      {"help",                    &Shell::_dump_help},
      {"version",                 &Shell::_dump_version},
      {"license",                 &Shell::_dump_license},
      {"dump_graph",              &Shell::_dump_graph},
      {"dump_taskflow",           &Shell::_dump_taskflow},
      {"dump_celllib",            &Shell::_dump_celllib},
      {"dump_net_load",           &Shell::_dump_net_load},
      {"dump_pin_cap",            &Shell::_dump_pin_cap},
      {"dump_slew",               &Shell::_dump_slew},
      {"dump_at",                 &Shell::_dump_at},
      {"dump_rat",                &Shell::_dump_rat},
      {"dump_slack",              &Shell::_dump_slack},
      {"dump_timer",              &Shell::_dump_timer},
      {"dump_verilog",            &Shell::_dump_verilog},
      {"dump_spef",               &Shell::_dump_spef},
      {"dump_rctree",             &Shell::_dump_rctree},

      // obselete
      {"exec_ops",                &Shell::_exec_ops},
      {"init_timer",              &Shell::_init_timer},
      {"set_early_celllib_fpath", &Shell::_set_early_celllib_fpath},
      {"set_late_celllib_fpath",  &Shell::_set_late_celllib_fpath},
      {"set_verilog_fpath",       &Shell::_set_verilog_fpath},
      {"set_spef_fpath",          &Shell::_set_spef_fpath},
      {"set_timing_fpath",        &Shell::_set_timing_fpath},
      {"report_timer",            &Shell::_report_timer},

      // system commands
      {"ls",                      &Shell::_ls},
      {"cd",                      &Shell::_cd},
      {"pwd",                     &Shell::_pwd},
      {"rm",                      &Shell::_rm},
      {"cat",                     &Shell::_cat},
      {"head",                    &Shell::_head},
      {"tail",                    &Shell::_tail},
      {"clear",                   &Shell::_clear},
      {"echo",                    &Shell::_echo},
      {"which",                   &Shell::_which}
    };
};

//-------------------------------------------------------------------------------------------------

};  // end of namespace ot ------------------------------------------------------------------------

#endif




