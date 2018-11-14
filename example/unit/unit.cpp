// This program demonstrates how to change units for 
// dimensional analysis.
//
// Design : unit.v
// SDC    : unit.sdc
// SPEF   : unit.spef
// Library: osu018_stdcells.lib

#include <ot/timer/timer.hpp>

int main(int argc, char *argv[]) {

  ot::Timer timer;
  
  // read the design
  timer.read_celllib("osu018_stdcells.lib")
       .read_verilog("unit.v")
       .read_sdc("unit.sdc")
       .read_spef("unit.spef")
       .update_timing();

  // the default library is at ns and pf scale.
  std::cout << timer.time_unit()->value()        << " second (time unit)\n"
            << timer.capacitance_unit()->value() << " farad  (capacitance unit)\n"
            << "TNS: " << *timer.report_tns() << '\n';
  
  // change to ps and ff.
  timer.set_time_unit(ot::second_t(1e-12))
       .set_capacitance_unit(ot::farad_t(1e-15))
       .update_timing();

  // dump the timer details
  std::cout << timer.time_unit()->value()        << " second (time unit)\n"
            << timer.capacitance_unit()->value() << " farad  (capacitance unit)\n"
            << "TNS: " << *timer.report_tns() << '\n';
  
  // change back to ns and pf
  timer.set_time_unit(ot::second_t(1e-9))
       .set_capacitance_unit(ot::farad_t(1e-12f))
       .update_timing();

  // dump the timer details
  std::cout << timer.time_unit()->value()        << " second (time unit)\n"
            << timer.capacitance_unit()->value() << " farad  (capacitance unit)\n"
            << "TNS: " << *timer.report_tns() << '\n';

  // scale the time unit by half
  timer.set_time_unit(ot::second_t(2e-9))
       .set_capacitance_unit(ot::farad_t(2e-12f))
       .update_timing();

  // dump the timer details
  std::cout << timer.time_unit()->value()        << " second (time unit)\n"
            << timer.capacitance_unit()->value() << " farad  (capacitance unit)\n"
            << "TNS: " << *timer.report_tns() << '\n';

  return 0;
}



















