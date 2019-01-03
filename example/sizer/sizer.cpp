// This program demonstrates how to resize a design of six NAND2 cells
// and reports timing/power/area values using OpenTimer C++ API.
// 
// Design : sizer.v
// SDC    : sizer.sdc
// SPEF   : sizer.spef
// Library: NangateOpenCellLibrary

#include <ot/timer/timer.hpp>

int main(int argc, char* argv[]) {
  
  ot::Timer timer;

  // Builder
  //   - read cell library for both early/late mode (nullopt)
  //   - read the verilog
  //   - read the parasitics
  //   - read the design constraints
  timer.read_celllib("NangateOpenCellLibrary_typical.lib", std::nullopt)
       .read_verilog("sizer.v")
       .read_spef   ("sizer.spef")
       .read_sdc    ("sizer.sdc");

  // Report the timing/power/area
  std::optional<float> tns  = timer.report_tns();
  std::optional<float> wns  = timer.report_wns();
  std::optional<float> area = timer.report_area();
  std::optional<float> lkp  = timer.report_leakage_power();
  
  std::cout << "\nBefore sizing\n"
            << "TNS          : " << *tns  << '\n'
            << "WNS          : " << *wns  << '\n'
            << "AREA         : " << *area << '\n'
            << "LEAKAGE_POWER: " << *lkp  << '\n';

  // Downsize all gates from X2 to X1
  timer.repower_gate("inst_0", "NAND2_X1")
       .repower_gate("inst_1", "NAND2_X1")
       .repower_gate("inst_2", "NAND2_X1")
       .repower_gate("inst_3", "NAND2_X1")
       .repower_gate("inst_4", "NAND2_X1")
       .repower_gate("inst_5", "NAND2_X1");
  
  tns  = timer.report_tns();
  wns  = timer.report_wns();
  area = timer.report_area();
  lkp  = timer.report_leakage_power();
  
  std::cout << "\nDownsize all to NAND2_X1\n"
            << "TNS          : " << *tns  << '\n'
            << "WNS          : " << *wns  << '\n'
            << "AREA         : " << *area << '\n'
            << "LEAKAGE_POWER: " << *lkp  << '\n';

  // Upsize all gates from X1 to X4
  timer.repower_gate("inst_0", "NAND2_X4")
       .repower_gate("inst_1", "NAND2_X4")
       .repower_gate("inst_2", "NAND2_X4")
       .repower_gate("inst_3", "NAND2_X4")
       .repower_gate("inst_4", "NAND2_X4")
       .repower_gate("inst_5", "NAND2_X4");
  
  tns  = timer.report_tns();
  wns  = timer.report_wns();
  area = timer.report_area();
  lkp  = timer.report_leakage_power();
  
  std::cout << "\nUpsize all to NAND2_X4\n"
            << "TNS          : " << *tns  << '\n'
            << "WNS          : " << *wns  << '\n'
            << "AREA         : " << *area << '\n'
            << "LEAKAGE_POWER: " << *lkp  << '\n';

  // Output the top-3 critical paths
  auto paths = timer.report_timing(3);
  
  std::cout << "# critical paths: " << paths.size() << '\n';

  for(size_t i=0; i<paths.size(); ++i) {

    std::cout << "Path : " << i+1 << '\n'
              << "Slack: " << paths[i].slack << '\n'
              << "Split: " << (paths[i].endpoint->split() == ot::MIN ? "MIN" : "MAX") << '\n';
    
    for(const auto& point : paths[i]) {

      std::cout << "  "
                << point.pin.name() << ' '
                << (point.transition == ot::RISE ? 'R' : 'F') << ' '
                << point.at;

      if(auto gate = point.pin.gate()) {
        std::cout << " (" << gate->name() << ' ' << gate->cell_name() << ')';
      }
      std::cout << '\n';
    }

    std::cout << '\n';
  }

  return 0;
}





