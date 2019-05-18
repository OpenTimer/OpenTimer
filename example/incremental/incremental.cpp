// This program demonstrates how to use OpenTimer C++ API to 
// construct a timing analysis graph from the ground up.
//
// Library: osu018_stdcells.lib

#include <ot/timer/timer.hpp>

int main(int argc, char *argv[]) {
  
  // create an opentimer object
  ot::Timer timer;
  
  // the first step is to read a library.
  timer.read_celllib("osu018_stdcells.lib", ot::MIN);
  timer.read_celllib("osu018_stdcells.lib", ot::MAX);

  // the second step is to create a netlist, for example:
  // // Start PIs and POs
  // input inp1;
  // input inp2;
  // input tau2015_clk;
  // output out;
  timer.insert_primary_input("inp1");
  timer.insert_primary_input("inp2");
  timer.insert_primary_input("tau2015_clk");
  timer.insert_primary_output("out");

  // // Start wires
  // wire n1;
  // wire n2;
  // wire n3;
  // wire n4;
  // wire inp1;
  // wire inp2;
  // wire tau2015_clk;
  // wire out;
  timer.insert_net("n1");
  timer.insert_net("n2");
  timer.insert_net("n3");
  timer.insert_net("n4");
  timer.insert_net("inp1");
  timer.insert_net("inp2");
  timer.insert_net("tau2015_clk");
  timer.insert_net("out");

  // // Start cells
  // NAND2X1 u1 ( .A(inp1), .B(inp2), .Y(n1) );
  timer.insert_gate("u1", "NAND2X1");
  timer.connect_pin("u1:A", "inp1");
  timer.connect_pin("u1:B", "inp2");
  timer.connect_pin("u1:Y", "n1");
  
  // DFFNEGX1 f1 ( .D(n2), .CLK(tau2015_clk), .Q(n3) );
  timer.insert_gate("f1", "DFFNEGX1");
  timer.connect_pin("f1:D", "n2");
  timer.connect_pin("f1:CLK", "tau2015_clk");
  timer.connect_pin("f1:Q", "n3");
  
  // INVX1 u2 ( .A(n3), .Y(n4) );
  timer.insert_gate("u2", "INVX1");
  timer.connect_pin("u2:A", "n3");
  timer.connect_pin("u2:Y", "n4");
  
  // INVX2 u3 ( .A(n4), .Y(out) );
  timer.insert_gate("u3", "INVX2");
  timer.connect_pin("u3:A", "n4");
  timer.connect_pin("u3:Y", "out");
  
  // NOR2X1 u4 ( .A(n1), .B(n3), .Y(n2) );
  timer.insert_gate("u4", "NOR2X1");
  timer.connect_pin("u4:A", "n1");
  timer.connect_pin("u4:B", "n3");
  timer.connect_pin("u4:Y", "n2");

  // the 3-rd step is to assert timing constraints
  // timer.read_sdc("simple.sdc");  <-- of course you can do this.
  //create_clock -period 50 -name tau2015_clk [get_ports tau2015_clk]
  timer.create_clock("tau2015_clk", "tau2015_clk", 50);

  // assert input arrival time and transition for PIs
  // set_input_delay 0 -min -rise [get_ports tau2015_clk] -clock tau2015_clk
  // set_input_delay 25 -min -fall [get_ports tau2015_clk] -clock tau2015_clk
  // set_input_delay 0 -max -rise [get_ports tau2015_clk] -clock tau2015_clk
  // set_input_delay 25 -max -fall [get_ports tau2015_clk] -clock tau2015_clk
  // set_input_transition 10 -min -rise [get_ports tau2015_clk] -clock tau2015_clk
  // set_input_transition 15 -min -fall [get_ports tau2015_clk] -clock tau2015_clk
  // set_input_transition 10 -max -rise [get_ports tau2015_clk] -clock tau2015_clk
  // set_input_transition 15 -max -fall [get_ports tau2015_clk] -clock tau2015_clk
  timer.set_at("tau2015_clk", ot::MIN, ot::RISE, 0);
  timer.set_at("tau2015_clk", ot::MIN, ot::FALL, 25);
  timer.set_at("tau2015_clk", ot::MAX, ot::RISE, 0);
  timer.set_at("tau2015_clk", ot::MAX, ot::FALL, 25);
  timer.set_slew("tau2015_clk", ot::MIN, ot::RISE, 10);
  timer.set_slew("tau2015_clk", ot::MIN, ot::FALL, 15);
  timer.set_slew("tau2015_clk", ot::MAX, ot::RISE, 10);
  timer.set_slew("tau2015_clk", ot::MAX, ot::FALL, 15);

  // similarly, assert arrival time for inp1 and inp2
  timer.set_at("inp1", ot::MIN, ot::RISE, 0);
  timer.set_at("inp1", ot::MIN, ot::FALL, 0);
  timer.set_at("inp1", ot::MAX, ot::RISE, 5);
  timer.set_at("inp1", ot::MAX, ot::FALL, 5);
  timer.set_slew("inp1", ot::MIN, ot::RISE, 10);
  timer.set_slew("inp1", ot::MIN, ot::FALL, 15);
  timer.set_slew("inp1", ot::MAX, ot::RISE, 20);
  timer.set_slew("inp1", ot::MAX, ot::FALL, 25);
  timer.set_at("inp2", ot::MIN, ot::RISE, 0);
  timer.set_at("inp2", ot::MIN, ot::FALL, 0);
  timer.set_at("inp2", ot::MAX, ot::RISE, 1);
  timer.set_at("inp2", ot::MAX, ot::FALL, 1);
  timer.set_slew("inp2", ot::MIN, ot::RISE, 30);
  timer.set_slew("inp2", ot::MIN, ot::FALL, 30);
  timer.set_slew("inp2", ot::MAX, ot::RISE, 40);
  timer.set_slew("inp2", ot::MAX, ot::FALL, 40);

  // for output, we assert required arrival time and load capacitance
  // notice that sdc has a different definition for required arrival time
  // set_load -pin_load 4 [get_ports out]
  // set_output_delay -10 -min -rise [get_ports out] -clock tau2015_clk
  // set_output_delay -10 -min -fall [get_ports out] -clock tau2015_clk
  // set_output_delay 30 -max -rise [get_ports out] -clock tau2015_clk
  // set_output_delay 30 -max -fall [get_ports out] -clock tau2015_clk
  timer.set_load("out", ot::MIN, ot::RISE, 4);
  timer.set_load("out", ot::MIN, ot::FALL, 4);
  timer.set_load("out", ot::MAX, ot::RISE, 4);
  timer.set_load("out", ot::MAX, ot::FALL, 4);
  timer.set_rat("out", ot::MIN, ot::RISE, 10);
  timer.set_rat("out", ot::MIN, ot::FALL, 10);
  timer.set_rat("out", ot::MAX, ot::RISE, 20);
  timer.set_rat("out", ot::MAX, ot::FALL, 20);

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



















