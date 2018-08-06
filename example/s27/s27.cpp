// This program demonstrates the basica usage of OpenTimer API
// to modify a design and report the timing.

#include <ot/timer/timer.hpp>

int main(int argc, char *argv[]) {
  
  ot::Timer timer;
  
  // Read design
  timer.num_threads(std::thread::hardware_concurrency())
       .celllib("s27_Early.lib", ot::EARLY)
       .celllib("s27_Late.lib", ot::LATE)
       .verilog("s27.v")
       .spef("s27.spef")
       .sdc("s27.sdc");

  // Report the TNS and WNS
  if(auto tns = timer.tns(); tns) {
    std::cout << "TNS: " << *tns << '\n';
  }
  else {
    std::cout << "TNS is not available\n";
  }

  if(auto wns = timer.wns(); wns) {
    std::cout << "WNS: " << *wns << '\n';
  }
  else {
    std::cout << "WNS is not available\n";
  }

  auto raw_path = timer.worst_paths(1);

  // apply design modifiers
  timer.repower_gate("inst_10", "INV_X16")
       .insert_gate("TAUGATE_1", "BUF_X2")
       .insert_net("TAUNET_1")
       .disconnect_pin("inst_3:ZN")
       .connect_pin("inst_3:ZN", "TAUNET_1")
       .connect_pin("TAUGATE_1:A", "TAUNET_1")
       .connect_pin("TAUGATE_1:Z", "net_14")
       .spef("change_1.spef");
  
  // report the slack
  std::cout << "Late/Fall slack at pin G17: " 
            << *timer.slack("G17", ot::LATE, ot::FALL) 
            << '\n';
  
  // report the arrival time
  std::cout << "Late/Fall arrival time at pin G17: "
            << *timer.at("G17", ot::LATE, ot::FALL)
            << '\n';
  
  // report the required arrival time
  std::cout << "Late/Fall required arrival time at pin G17: "
            << *timer.rat("G17", ot::LATE, ot::FALL)
            << '\n';

  // Dump the timer
  std::cout << timer.dump_timer();

  // Dump the slack
  std::cout << timer.dump_slack();

  // change from pico seconds to nano seconds
  timer.time_unit(ot::nanoseconds(1))
       .capacitance_unit(ot::picofarads(1))
       .update_timing();
  
  // Dump the slack
  std::cout << timer.dump_slack();
  
  // report the path after optimization
  auto opt_path = timer.worst_paths(1);

  // compare
  std::cout << "Path before optimization: \n" << raw_path.get()[0];
  std::cout << "Path after  optimization: \n" << opt_path.get()[0];

  return 0;
}



















