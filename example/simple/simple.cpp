// This program demonstrates the basica usage of OpenTimer C++ API
// to get the critical path information of a simple sequential design.
//
// Design : simple.v
// SDC    : simple.sdc
// Library: osu018_stdcells.lib

#include <ot/timer/timer.hpp>

int main(int argc, char *argv[]) {

  // create a timer object
  ot::Timer timer;
  
  // Read design
  timer.read_celllib("osu018_stdcells.lib", ot::MIN)
       .read_celllib("osu018_stdcells.lib", ot::MAX)
       .read_verilog("simple.v")
       .read_sdc("simple.sdc");

  // get the top-5 worst critical paths
  auto paths = timer.report_timing(5);

  for(size_t i=0; i<paths.size(); ++i) {
    std::cout << "----- Critical Path " << i << " -----\n";
    std::cout << paths[i] << '\n';
  }

  // dump the timing graph to dot format for debugging
  timer.dump_graph(std::cout);

  return 0;
}



















