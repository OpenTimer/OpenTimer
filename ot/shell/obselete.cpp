#include <ot/shell/shell.hpp>

namespace ot {

// Procedure: exec_ops
void exec_ops(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {
  OT_LOGW(std::quoted("exec_ops"), " is obselete and has no effect");
}

// Procedure: init_timer
void init_timer(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {
  OT_LOGW(std::quoted("init_timer"), " is obselete and has no effect");
}

// Procedure: set_early_celllib_fpath
void set_early_celllib_fpath(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {
  OT_LOGW(
    std::quoted("set_early_celllib_fpath"), " is obselete; use ", std::quoted("read_celllib") 
  );
  if(std::filesystem::path path; is >> path) {
    timer.celllib(std::move(path), EARLY);
  }
}

// Procedure: set_late_celllib_fpath
void set_late_celllib_fpath(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {
  OT_LOGW(
    std::quoted("set_late_celllib_fpath"), " is obselete; use ", std::quoted("read_celllib")
  );
  if(std::filesystem::path path; is >> path) {
    timer.celllib(std::move(path), LATE);
  }
}

// Procedure: set_verilog_fpath
void set_verilog_fpath(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {
  OT_LOGW(
    std::quoted("set_verilog_fpath"), " is obselete; use ", std::quoted("read_verilog")
  );
  if(std::filesystem::path path; is >> path) {
    timer.verilog(std::move(path));
  }
}

// Procedure: set_spef_fpath
void set_spef_fpath(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {
  OT_LOGW(
    std::quoted("set_spef_fpath"), " is obselete; use ", std::quoted("read_spef")
  );
  if(std::filesystem::path path; is >> path) {
    timer.spef(std::move(path));
  }
}

// Procedure: set_timing_fpath
void set_timing_fpath(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {
  OT_LOGW(
    std::quoted("set_timing_fpath"), " is obselete; use ", std::quoted("read_timing")
  );
  if(std::filesystem::path path; is >> path) {
    timer.timing(std::move(path));
  }
}

// Procedure: report_timer
void report_timer(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  OT_LOGW(
    std::quoted("report_timer"), " is obselete; use ", std::quoted("dump_timer")
  );
    
  if(auto paths = timer.worst_paths(1).get(); !paths.empty()) {
    os << paths[0];
  }
  else {
    os << Path();
  }
}

};  // end of namespace ot. -----------------------------------------------------------------------





