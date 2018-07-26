#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include <ot/sdc/sdc.hpp>

/*// Operator << tokens
std::ostream& operator << (std::ostream& os, const std::vector<std::string>& tokens) {
  for(size_t i=0; i<tokens.size(); ++i) {
    if(i != 0) {
      os << ' ';
    }
    os << tokens[i];
  }
  return os;
}

// Testcase: Tokenize.Comment
TEST_CASE("Tokenize.Comment") {

  std::string l1 = "# this is a comment"; 
  std::string l2 = "this is not a comment "; 
  std::string l3 = "line with comment # starting from here";
  std::string l4 = "quoted comment \"-------\" ends";

  auto t1 = ot::sdc::tokenize(l1);
  REQUIRE(t1.empty());
  
  auto t2 = ot::sdc::tokenize(l2);
  REQUIRE(t2.size() == 5);

  auto t3 = ot::sdc::tokenize(l3);
  REQUIRE(t3.size() == 3);

  auto t4 = ot::sdc::tokenize(l4);
  REQUIRE(t4.size() == 4);
}

// Testcase:: Tokenize.Command
TEST_CASE("Tokenize.Command") {
  
  std::string cmd = "w1 [w2 {w3 {w4[0]}}] {{w5[8]} w6} w7 # comments";
  auto tokens = ot::sdc::tokenize(cmd);

  REQUIRE(tokens.size() == 11);
  REQUIRE(tokens[0] == "w1");
  REQUIRE(tokens[1] == "[");
  REQUIRE(tokens[2] == "w2");
  REQUIRE(tokens[3] == "w3");
  REQUIRE(tokens[4] == "w4[0]");
  REQUIRE(tokens[5] == "]");
  REQUIRE(tokens[6] == "{");
  REQUIRE(tokens[7] == "w5[8]");
  REQUIRE(tokens[8] == "w6");
  REQUIRE(tokens[9] == "}");
  REQUIRE(tokens[10] == "w7");
}

// Testcase: Port
TEST_CASE("Port") {
  
  SUBCASE("all_inputs") {
    auto p = ot::sdc::extract_port("[all_inputs]");
    REQUIRE(std::holds_alternative<ot::sdc::AllInputs>(p));
  }
  
  SUBCASE("all_inputs") {
    auto p = ot::sdc::extract_port("[all_outputs]");
    REQUIRE(std::holds_alternative<ot::sdc::AllOutputs>(p));
  }
  
  SUBCASE("get_ports") {

    auto p = ot::sdc::extract_port("[get_ports {a {b[0]}}]");

    std::visit(Functors{
      [] (ot::sdc::GetPorts& get_ports) {
        REQUIRE(get_ports.ports.size() == 2);
        REQUIRE(get_ports.ports[0] == "a");
        REQUIRE(get_ports.ports[1] == "b[0]");
      },
      [] (auto&&) {
        REQUIRE(false);
      }
    }, p);
  }

  SUBCASE("implicit") {

    auto p = ot::sdc::extract_port("{a b {c[0]}}");

    std::visit(Functors{
      [] (ot::sdc::GetPorts& get_ports) {
        REQUIRE(get_ports.ports.size() == 3);
        REQUIRE(get_ports.ports[0] == "a");
        REQUIRE(get_ports.ports[1] == "b");
        REQUIRE(get_ports.ports[2] == "c[0]");
      },
      [] (auto&&) {
        REQUIRE(false);
      }
    }, p);
  }
}

// Testcase: Clock
TEST_CASE("Clock") {
  
  SUBCASE("all_clocks") {
    auto p = ot::sdc::extract_clock("[all_clocks]");
    REQUIRE(std::holds_alternative<ot::sdc::AllClocks>(p));
  }
  
  SUBCASE("get_clocks") {

    auto p = ot::sdc::extract_clock("[get_clocks ck1 ck2 {ck3}]");

    std::visit(Functors{
      [] (ot::sdc::GetClocks& get_clocks) {
        REQUIRE(get_clocks.clocks.size() == 3);
        REQUIRE(get_clocks.clocks[0] == "ck1");
        REQUIRE(get_clocks.clocks[1] == "ck2");
        REQUIRE(get_clocks.clocks[2] == "ck3");
      },
      [] (auto&&) {
        REQUIRE(false);
      }
    }, p);
  }

  SUBCASE("implicit") {

    auto p = ot::sdc::extract_clock("{ck1 ck2 {ck[0]}}");

    std::visit(Functors{
      [] (ot::sdc::GetClocks& get_clocks) {
        REQUIRE(get_clocks.clocks.size() == 3);
        REQUIRE(get_clocks.clocks[0] == "ck1");
        REQUIRE(get_clocks.clocks[1] == "ck2");
        REQUIRE(get_clocks.clocks[2] == "ck[0]");
      },
      [] (auto&&) {
        REQUIRE(false);
      }
    }, p);
  }
}

// Testcase: TimingMask
TEST_CASE("TimingMask") {
  
  auto m1 = ot::sdc::TimingMask(std::nullopt, std::nullopt, std::nullopt, std::nullopt);
  REQUIRE(((m1 | ot::EARLY) && (m1 | ot::LATE)));
  REQUIRE(((m1 | ot::RISE ) && (m1 | ot::FALL)));

  auto m2 = ot::sdc::TimingMask(std::nullopt, std::nullopt, std::nullopt, std::byte{});
  REQUIRE(((m2 | ot::EARLY) && (m2 | ot::LATE)));
  REQUIRE((!(m2 | ot::RISE) && (m2 | ot::FALL)));
  
  auto m3 = ot::sdc::TimingMask(std::nullopt, std::nullopt, std::byte{}, std::nullopt);
  REQUIRE(((m3 | ot::EARLY) &&  (m3 | ot::LATE)));
  REQUIRE(((m3 | ot::RISE)  && !(m3 | ot::FALL)));
  
  auto m4 = ot::sdc::TimingMask(std::nullopt, std::nullopt, std::byte{}, std::byte{});
  REQUIRE(((m4 | ot::EARLY) &&  (m4 | ot::LATE)));
  REQUIRE(((m4 | ot::RISE)  &&  (m4 | ot::FALL)));
  
  auto m5 = ot::sdc::TimingMask(std::nullopt, std::byte{}, std::nullopt, std::nullopt);
  REQUIRE((!(m5 | ot::EARLY) &&  (m5 | ot::LATE)));
  REQUIRE(((m5 | ot::RISE)   &&  (m5 | ot::FALL)));
  
  auto m6 = ot::sdc::TimingMask(std::byte{}, std::nullopt, std::nullopt, std::nullopt);
  REQUIRE(((m6 | ot::EARLY) && !(m6 | ot::LATE)));
  REQUIRE(((m6 | ot::RISE)  &&  (m6 | ot::FALL)));
  
  auto m7 = ot::sdc::TimingMask(std::byte{}, std::nullopt, std::nullopt, std::byte{});
  REQUIRE(((m7 | ot::EARLY) && !(m7 | ot::LATE)));
  REQUIRE((!(m7 | ot::RISE) &&  (m7 | ot::FALL)));
  
  auto m8 = ot::sdc::TimingMask(std::byte{}, std::byte{}, std::nullopt, std::nullopt);
  REQUIRE(((m8 | ot::EARLY) && (m8 | ot::LATE)));
  REQUIRE(((m8 | ot::RISE)  && (m8 | ot::FALL)));
}

// Testcase: SetInputDelay
TEST_CASE("SetInputDelay") {

  SUBCASE("command1") { 

    auto tokens = ot::sdc::tokenize(
      "set_input_delay 3.000000\
      -rise -min\
      -clock [get_clocks {{clk}}]\
      [get_ports {{bus[31]} pi1 {pi2}}]"
    );

    auto beg = tokens.begin();
    auto end = tokens.end();

    REQUIRE(beg != end);
    auto obj = ot::sdc::extract_set_input_delay(beg, end);
    REQUIRE(beg == end);

    REQUIRE((obj.value && std::fabs(*obj.value - 3.0f) < 1e-3));
    REQUIRE(obj.rise);
    REQUIRE(obj.min);
    REQUIRE(obj.clock);
    REQUIRE(obj.port);

    std::visit(Functors{
      [] (ot::sdc::GetClocks& get_clocks) {
        REQUIRE(get_clocks.clocks.size() == 1);
        REQUIRE(get_clocks.clocks[0] == "clk");
      },
      [] (auto&&) {
        REQUIRE(false);
      }
    }, *obj.clock);

    std::visit(Functors{
      [] (ot::sdc::GetPorts& get_ports) {
        REQUIRE(get_ports.ports.size() == 3);
        REQUIRE(get_ports.ports[0] == "bus[31]");
        REQUIRE(get_ports.ports[1] == "pi1");
        REQUIRE(get_ports.ports[2] == "pi2");
      },
      [] (auto&&) {
        REQUIRE(false);
      }
    }, *obj.port);
  }
  
  
  SUBCASE("command2") {

    auto tokens = ot::sdc::tokenize("set_input_delay -1 -fall -max port");

    auto beg = tokens.begin();
    auto end = tokens.end();

    REQUIRE(beg != end);
    auto obj = ot::sdc::extract_set_input_delay(beg, end);
    REQUIRE(beg == end);

    REQUIRE((obj.value && std::fabs(*obj.value + 1.0f) < 1e-3));
    REQUIRE(obj.fall);
    REQUIRE(obj.max);
    REQUIRE(obj.port);

    std::visit(Functors{
      [] (ot::sdc::GetPorts& get_ports) {
        REQUIRE(get_ports.ports.size() == 1);
        REQUIRE(get_ports.ports[0] == "port");
      },
      [] (auto&&) {
        REQUIRE(false);
      }
    }, *obj.port);
  }
}

// Testcase: SetInputTransition
TEST_CASE("SetInputTransition") {
  
  auto tokens = ot::sdc::tokenize(
    "set_input_transition -fall -max 0.5 [get_ports abs {bus[0]}] -clock clk"
  ); 
    
  auto beg = tokens.begin();
  auto end = tokens.end();
  REQUIRE(beg != end);
  auto obj = ot::sdc::extract_set_input_transition(beg, end);
  REQUIRE(beg == end);
    
  REQUIRE((obj.value && std::fabs(*obj.value - 0.5f) < 1e-3));
  REQUIRE(obj.fall);
  REQUIRE(obj.max);
  REQUIRE(obj.clock);
  REQUIRE(obj.port);
    
  std::visit(Functors{
    [] (ot::sdc::GetClocks& get_clocks) {
      REQUIRE(get_clocks.clocks.size() == 1);
      REQUIRE(get_clocks.clocks[0] == "clk");
    },
    [] (auto&&) {
      REQUIRE(false);
    }
  }, *obj.clock);

  std::visit(Functors{
    [] (ot::sdc::GetPorts& get_ports) {
      REQUIRE(get_ports.ports.size() == 2);
      REQUIRE(get_ports.ports[0] == "abs");
      REQUIRE(get_ports.ports[1] == "bus[0]");
    },
    [] (auto&&) {
      REQUIRE(false);
    }
  }, *obj.port);
}

// Testcase: SetOutputDelay
TEST_CASE("SetOutputDelay") {

  SUBCASE("command1") {

    auto tokens = ot::sdc::tokenize(
      "set_output_delay 3.000000 -rise -min -clock {{clk[0]}} po"
    );

    auto beg = tokens.begin();
    auto end = tokens.end();
    REQUIRE(beg != end);
    auto obj = ot::sdc::extract_set_output_delay(beg, end);
    REQUIRE(beg == end);

    REQUIRE((obj.value && std::fabs(*obj.value - 3.0f) < 1e-3));
    REQUIRE(obj.rise);
    REQUIRE(obj.min);
    REQUIRE(obj.clock);
    REQUIRE(obj.port);

    std::visit(Functors{
      [] (ot::sdc::GetClocks& get_clocks) {
        REQUIRE(get_clocks.clocks.size() == 1);
        REQUIRE(get_clocks.clocks[0] == "clk[0]");
      },
      [] (auto&&) {
        REQUIRE(false);
      }
    }, *obj.clock);

    std::visit(Functors{
      [] (ot::sdc::GetPorts& get_ports) {
        REQUIRE(get_ports.ports.size() == 1);
        REQUIRE(get_ports.ports[0] == "po");
      },
      [] (auto&&) {
        REQUIRE(false);
      }
    }, *obj.port);
  }
  
  
  SUBCASE("command2") {

    auto tokens = ot::sdc::tokenize(
      "set_output_delay -1 -fall -max {po {bus[0]}}\
       -network_latency_included -source_latency_included"
    );

    auto beg = tokens.begin();
    auto end = tokens.end();
    REQUIRE(beg != end);
    auto obj = ot::sdc::extract_set_output_delay(beg, end);
    REQUIRE(beg == end);

    REQUIRE((obj.value && std::fabs(*obj.value + 1.0f) < 1e-3));
    REQUIRE(obj.fall);
    REQUIRE(obj.max);
    REQUIRE(obj.network_latency_included);
    REQUIRE(obj.source_latency_included);
    REQUIRE(obj.port);

    std::visit(Functors{
      [] (ot::sdc::GetPorts& get_ports) {
        REQUIRE(get_ports.ports.size() == 2);
        REQUIRE(get_ports.ports[0] == "po");
        REQUIRE(get_ports.ports[1] == "bus[0]");
      },
      [] (auto&&) {
        REQUIRE(false);
      }
    }, *obj.port);
  }
}

// Testcase: SetLoad
TEST_CASE("SetLoad") {
  
  auto tokens = ot::sdc::tokenize("set_load 3 -min -pin_load port");
  
  auto beg = tokens.begin();
  auto end = tokens.end();
  
  REQUIRE(beg != end);
  auto obj = ot::sdc::extract_set_load(beg, end);
  REQUIRE(beg == end);

  REQUIRE((obj.value && std::fabs(*obj.value - 3.0f) < 1e-3));
  REQUIRE(obj.min);
  REQUIRE(obj.pin_load);
  REQUIRE(obj.port);

  std::visit(Functors{
    [] (ot::sdc::GetPorts& get_ports) {
      REQUIRE(get_ports.ports.size() == 1);
      REQUIRE(get_ports.ports[0] == "port");
    },
    [] (auto&&) {
      REQUIRE(false);
    }
  }, *obj.port);
}

// Testcase: CreateClock
TEST_CASE("CreateClock") {
  
  auto tokens = ot::sdc::tokenize(
    "create_clock -name clk -waveform {0 9} -add -comment \"n/a\" [get_ports port]"
  );
  auto beg = tokens.begin();
  auto end = tokens.end();
  
  REQUIRE(beg != end);
  auto obj = ot::sdc::extract_create_clock(beg, end);
  REQUIRE(beg == end);
  
  REQUIRE(obj.name == "clk");
  REQUIRE(obj.waveform);
  REQUIRE(obj.add);
  REQUIRE(obj.comment == "n/a");
  REQUIRE(obj.port);
    
  std::visit(Functors{
    [] (ot::sdc::GetPorts& get_ports) {
      REQUIRE(get_ports.ports.size() == 1);
      REQUIRE(get_ports.ports[0] == "port");
    },
    [] (auto&&) {
      REQUIRE(false);
    }
  }, *obj.port);
}*/


