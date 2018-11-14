// This program demonstrates how to use OpenTimer's API to 
// run a simple timing-driven optimization loop plus 
// incremental timing update.

// Design : optimizer.v
// SDC    : optimizer.sdc
// SPEF   : optimizer.spef change_1.spef
// Library: optimizer_Early.lib (early/ min split)
// Library: optimizer_Late.lib  (late / max split)

#include <ot/timer/timer.hpp>

int main(int argc, char *argv[]) {

  ot::Timer timer;
  
  // Read design
  timer.read_celllib("optimizer_Early.lib", ot::MIN)
       .read_celllib("optimizer_Late.lib", ot::MAX)
       .read_verilog("optimizer.v")
       .read_spef("optimizer.spef")
       .read_sdc("optimizer.sdc");

  // Report the TNS and WNS
  if(std::optional<float> tns = timer.report_tns(); tns) {
    std::cout << "TNS: " << *tns << '\n';
  }
  else {
    std::cout << "TNS is not available\n";
  }

  if(std::optional<float> wns = timer.report_wns(); wns) {
    std::cout << "WNS: " << *wns << '\n';
  }
  else {
    std::cout << "WNS is not available\n";
  }

  // repower a gate and insert a buffer
  timer.repower_gate("inst_10", "INV_X16")
       .insert_gate("TAUGATE_1", "BUF_X2")
       .insert_net("TAUNET_1")
       .disconnect_pin("inst_3:ZN")
       .connect_pin("inst_3:ZN", "TAUNET_1")
       .connect_pin("TAUGATE_1:A", "TAUNET_1")
       .connect_pin("TAUGATE_1:Z", "net_14")
       .read_spef("change_1.spef");
  
  // report the slack at a G17
  std::cout << "Late/Fall slack at pin G17: " 
            << *timer.report_slack("G17", ot::MAX, ot::FALL) 
            << '\n';
  
  // report the arrival time at G17
  std::cout << "Late/Fall arrival time at pin G17: "
            << *timer.report_at("G17", ot::MAX, ot::FALL)
            << '\n';
  
  // report the required arrival time at G17
  std::cout << "Late/Fall required arrival time at pin G17: "
            << *timer.report_rat("G17", ot::MAX, ot::FALL)
            << '\n';

  // report the path after optimization
  auto opt_path = timer.report_timing(1);

  // compare
  std::cout << "Critical path after  optimization: \n" << opt_path[0];

  // dump the slack
  timer.dump_slack(std::cout);

  return 0;
}



















