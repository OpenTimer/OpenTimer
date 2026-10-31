// Derived from LiveHD pass/opentimer/tests/celllib_smoke.cpp.
// Distributed under the BSD 3-Clause License; see LICENSE.livehd.
#include <unistd.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "ot/liberty/celllib.hpp"
#include "ot/timer/timer.hpp"

namespace {
class CelllibSmoke : public testing::Test {
protected:
  std::filesystem::path path;

  void SetUp() override {
    const auto* tmpdir = std::getenv("TEST_TMPDIR");
    auto name = ((tmpdir ? std::filesystem::path(tmpdir) : std::filesystem::temp_directory_path()) /
                 "opentimer-celllib-XXXXXX").string();
    int fd = mkstemp(name.data());
    ASSERT_GE(fd, 0);
    close(fd);
    path = name;
  }
  void TearDown() override { std::filesystem::remove(path); }
  void write(const std::string& body) {
    std::ofstream stream(path);
    ASSERT_TRUE(stream);
    stream << body;
    stream.close();
    ASSERT_TRUE(stream);
  }
};

TEST_F(CelllibSmoke, TemplateAxesAndBusAttributes) {
  write(R"lib(library(test) {
    lu_table_template(delay) {
      variable_1 : input_net_transition;
      variable_2 : total_output_net_capacitance;
      index_1("1, 2"); index_2("3, 4, 5");
    }
    type(data) { base_type : array; data_type : bit; bit_width : 4; bit_from : 0; bit_to : 3; }
    cell(RAM) {
      pin(clk) { direction : input; clock : true; }
      bus(A) {
        bus_type : data; direction : input; capacitance : 0.25;
        pin(A[3:0]) {
          timing() { related_pin : clk; timing_type : setup_rising;
            rise_constraint(delay) { values("1, 2, 3", "4, 5, 6"); }
          }
        }
      }
      bus(Q) {
        bus_type : data; direction : output;
        timing() { related_pin : clk; timing_type : rising_edge;
          cell_rise(delay) { index_1("7"); values("8, 9, 10"); }
        }
      }
    }
  })lib");
  ot::Celllib lib;
  lib.read(path);
  const auto* cell = lib.cell("RAM");
  ASSERT_NE(cell, nullptr);
  EXPECT_EQ(cell->cellpins.size(), 9u);
  for (int bit = 0; bit < 4; ++bit) {
    const auto* input = cell->cellpin("A[" + std::to_string(bit) + "]");
    ASSERT_NE(input, nullptr);
    EXPECT_EQ(input->direction, ot::CellpinDirection::INPUT);
    EXPECT_EQ(input->capacitance, 0.25f);
    ASSERT_EQ(input->timings.size(), 1u);
    const auto& lut = input->timings.front().rise_constraint;
    ASSERT_TRUE(lut);
    EXPECT_EQ(lut->indices1, (std::vector<float>{1, 2}));
    EXPECT_EQ(lut->indices2, (std::vector<float>{3, 4, 5}));
    EXPECT_EQ(lut->table, (std::vector<float>{1, 2, 3, 4, 5, 6}));
    const auto* output = cell->cellpin("Q[" + std::to_string(bit) + "]");
    ASSERT_NE(output, nullptr);
    EXPECT_EQ(output->direction, ot::CellpinDirection::OUTPUT);
    ASSERT_EQ(output->timings.size(), 1u);
    const auto& out_lut = output->timings.front().cell_rise;
    ASSERT_TRUE(out_lut);
    EXPECT_EQ(out_lut->indices1, (std::vector<float>{7}));
    EXPECT_EQ(out_lut->indices2, (std::vector<float>{3, 4, 5}));
    EXPECT_EQ(out_lut->table, (std::vector<float>{8, 9, 10}));
  }
}

// The two libraries deliberately reuse a template name. Copies and merges
// must retain the second library's own axis meanings and waveform conventions.
std::string threshold_libraries() {
  return R"lib(
library(logic) {
  time_unit : "1ps"; capacitive_load_unit(1,ff);
  slew_lower_threshold_pct_rise : 10; slew_upper_threshold_pct_rise : 90;
  slew_lower_threshold_pct_fall : 10; slew_upper_threshold_pct_fall : 90;
  lu_table_template(shared) {
    variable_1 : input_net_transition; variable_2 : total_output_net_capacitance;
    index_1("0,100"); index_2("0,100");
  }
  cell(BUF) {
    pin(A) { direction : input; capacitance : 2; }
    pin(Y) { direction : output;
      timing() { related_pin : "A"; timing_sense : positive_unate;
        cell_rise(shared) { values("10,210", "60,260"); }
        cell_fall(shared) { values("10,210", "60,260"); }
        rise_transition(shared) { values("4,104", "54,154"); }
        fall_transition(shared) { values("4,104", "54,154"); }
      }
    }
  }
}
library(ram) {
  time_unit : "1ns"; capacitive_load_unit(1,pf);
  slew_lower_threshold_pct_rise : 20; slew_upper_threshold_pct_rise : 80;
  slew_lower_threshold_pct_fall : 20; slew_upper_threshold_pct_fall : 80;
  lu_table_template(shared) {
    variable_1 : total_output_net_capacitance; variable_2 : input_net_transition;
    index_1("0,0.1"); index_2("0,0.1");
  }
  cell(RAM) {
    pin(A) { direction : input; capacitance : 0.002; }
    pin(Y) { direction : output;
      timing() { related_pin : "A"; timing_sense : positive_unate;
        cell_rise(shared) { values("0.01,0.06", "0.21,0.26"); }
        cell_fall(shared) { values("0.01,0.06", "0.21,0.26"); }
        rise_transition(shared) { values("0.004,0.054", "0.104,0.154"); }
        fall_transition(shared) { values("0.004,0.054", "0.104,0.154"); }
      }
    }
  }
})lib";
}

TEST_F(CelllibSmoke, ScopedTemplatesSurviveCopiesCollisionsAndMixedUnits) {
  write(threshold_libraries());
  ot::Celllib copy;
  {
    auto libraries = ot::Celllib::read_all(path);
    copy           = libraries.at(1);
  }
  const auto& arc = copy.cell("RAM")->cellpin("Y")->timings.front();
  EXPECT_NEAR(arc.delay(ot::RISE, ot::RISE, .08f, .004f).value(), .058f, 1e-6);
  ot::Timer timer;
  timer.set_num_threads(1);
  timer.read_celllib(path).update_timing();
  for (auto split : {ot::MIN, ot::MAX}) {
    const auto& library = timer.celllib(split);
    const auto& timing  = library->cell("RAM")->cellpin("Y")->timings.front();
    EXPECT_NEAR(timing.delay(ot::RISE, ot::RISE, 80, 4).value(), 58, 1e-4);
    EXPECT_NEAR(timing.slew(ot::RISE, ot::RISE, 80, 4).value(), 48, 1e-4);
    EXPECT_NEAR(library->cell("RAM")->cellpin("A")->capacitance.value(), 2, 1e-5);
  }
}

void build_threshold_chain(ot::Timer& timer) {
  timer.insert_primary_input("a").insert_primary_output("y");
  timer.insert_gate("b", "BUF").insert_gate("m", "RAM");
  for (const auto* net : {"input", "middle", "output"}) {
    timer.insert_net(net);
  }
  timer.disconnect_pin("a").disconnect_pin("y");
  timer.connect_pin("a", "input").connect_pin("b:A", "input");
  timer.connect_pin("b:Y", "middle").connect_pin("m:A", "middle");
  timer.connect_pin("m:Y", "output").connect_pin("y", "output");
  for (auto split : {ot::MIN, ot::MAX}) {
    for (auto tran : {ot::RISE, ot::FALL}) {
      timer.set_at("a", split, tran, 10).set_slew("a", split, tran, 80).set_load("y", split, tran, 4);
    }
  }
}

TEST_F(CelllibSmoke, TimingPathRecoveryAllowsMissingInternalPower) {
  write(threshold_libraries());
  ot::Timer timer;
  timer.set_num_threads(1);
  timer.read_celllib(path).update_timing();
  build_threshold_chain(timer);
  spef::Spef parasitics;
  parasitics.capacitance_unit = "1 FF";
  parasitics.resistance_unit = "1 OHM";
  for (const auto& [name, driver, sink] : std::vector<std::tuple<std::string, std::string, std::string>>{
           {"input", "a", "b:A"}, {"middle", "b:Y", "m:A"}, {"output", "m:Y", "y"}}) {
    spef::Net net(name, 0);
    net.caps.emplace_back(sink, "", 0);
    net.ress.emplace_back(driver, sink, 1);
    parasitics.nets.push_back(std::move(net));
  }
  timer.read_parasitics(std::move(parasitics));
  for (auto split : {ot::MIN, ot::MAX})
    for (auto tran : {ot::RISE, ot::FALL}) timer.set_rat("y", split, tran, 0);
  timer.update_timing();
  ASSERT_TRUE(timer.report_at("y", ot::MAX, ot::RISE));
  ASSERT_TRUE(timer.report_rat("y", ot::MAX, ot::RISE));
  for (size_t count : {1U, 4U}) {
    SCOPED_TRACE(count);
    const auto paths = timer.report_timing(count, ot::MAX);
    ASSERT_FALSE(paths.empty());
    for (const auto& result : paths) {
      EXPECT_FALSE(result.empty());
      for (const auto& point : result) EXPECT_EQ(point.ipower, 0);
    }
  }
}

TEST_F(CelllibSmoke, MixedThresholdChainMatchesIndependentOpenStaCalculation) {
  write(threshold_libraries());
  ot::Timer timer;
  timer.set_num_threads(1);
  timer.read_celllib(path).update_timing();
  build_threshold_chain(timer);
  timer.update_timing();
  for (auto split : {ot::MIN, ot::MAX}) {
    for (auto tran : {ot::RISE, ot::FALL}) {
      EXPECT_NEAR(timer.pins().at("b:Y").at(split, tran).value(), 64, 1e-4);
      EXPECT_NEAR(timer.pins().at("m:A").slew(split, tran).value(), 34.5, 1e-4);
      EXPECT_NEAR(timer.pins().at("y").at(split, tran).value(), 99.25, 1e-4);
      EXPECT_NEAR(timer.pins().at("y").slew(split, tran).value(), 33.666667, 1e-4);
    }
  }
}

TEST_F(CelllibSmoke,
       SerialAndParallelPropagationAgreeAcrossReconvergenceAndCycles) {
  const std::string buffer = R"lib(
    cell(BUF) {
      pin(A) { direction : input; capacitance : 2; }
      pin(Y) { direction : output; function : "A";
        timing() { related_pin : "A"; timing_sense : positive_unate;
          cell_rise(scalar) { values("10"); } cell_fall(scalar) { values("11"); }
          rise_transition(scalar) { values("5"); } fall_transition(scalar) { values("6"); }
        }
      }
    }
  )lib";
  write(R"lib(library(test) {
    time_unit : "1ps"; capacitive_load_unit(1,ff);
  )lib" +
        buffer + R"lib(
    cell(JOIN) {
      pin(A) { direction : input; capacitance : 2; }
      pin(B) { direction : input; capacitance : 2; }
      pin(Y) { direction : output; function : "A | B";
        timing() { related_pin : "A"; timing_sense : positive_unate;
          cell_rise(scalar) { values("20"); } cell_fall(scalar) { values("21"); }
          rise_transition(scalar) { values("5"); } fall_transition(scalar) { values("6"); }
        }
        timing() { related_pin : "B"; timing_sense : positive_unate;
          cell_rise(scalar) { values("30"); } cell_fall(scalar) { values("31"); }
          rise_transition(scalar) { values("5"); } fall_transition(scalar) { values("6"); }
        }
      }
    }
  })lib");
  ot::Timer serial, parallel;
  serial.set_num_threads(1);
  parallel.set_num_threads(2);
  for (auto *timer : {&serial, &parallel}) {
    timer->read_celllib(path).insert_primary_input("a").insert_primary_output(
        "y");
    timer->insert_gate("b", "BUF").insert_gate("c", "BUF");
    timer->insert_gate("join", "JOIN").insert_gate("tail", "BUF");
    for (const auto *net : {"in", "b_out", "c_out", "joined", "out"})
      timer->insert_net(net);
    timer->disconnect_pin("a").disconnect_pin("y");
    for (const auto &[pin, net] :
         std::vector<std::pair<std::string, std::string>>{{"a", "in"},
                                                          {"b:A", "in"},
                                                          {"c:A", "in"},
                                                          {"b:Y", "b_out"},
                                                          {"join:A", "b_out"},
                                                          {"c:Y", "c_out"},
                                                          {"join:B", "c_out"},
                                                          {"join:Y", "joined"},
                                                          {"tail:A", "joined"},
                                                          {"tail:Y", "out"},
                                                          {"y", "out"}}) {
      timer->connect_pin(pin, net);
    }
    for (auto split : {ot::MIN, ot::MAX}) {
      for (auto tran : {ot::RISE, ot::FALL}) {
        timer->set_at("a", split, tran, 3.f + 2.f * tran);
        timer->set_slew("a", split, tran, 10.f);
        timer->set_rat("y", split, tran, split == ot::MIN ? 1000.f : 0.f);
      }
    }
  }
  const auto compare = [&] {
    serial.update_timing();
    parallel.update_timing();
    ASSERT_EQ(serial.num_sccs(), parallel.num_sccs());
    ASSERT_EQ(serial.pins().size(), parallel.pins().size());
    for (const auto &[name, pin] : serial.pins()) {
      const auto &other = parallel.pins().at(name);
      for (auto split : {ot::MIN, ot::MAX}) {
        for (auto tran : {ot::RISE, ot::FALL}) {
          EXPECT_EQ(pin.at(split, tran), other.at(split, tran)) << name;
          EXPECT_EQ(pin.rat(split, tran), other.rat(split, tran)) << name;
          EXPECT_EQ(pin.slew(split, tran), other.slew(split, tran)) << name;
          EXPECT_EQ(pin.slack(split, tran), other.slack(split, tran)) << name;
        }
      }
    }
  };
  compare();
  EXPECT_EQ(serial.num_sccs(), 0U);
  EXPECT_FLOAT_EQ(*serial.report_at("y", ot::MAX, ot::RISE), 53.f);
  for (auto *timer : {&serial, &parallel})
    timer->set_at("a", ot::MAX, ot::RISE, 13.f);
  compare();
  EXPECT_FLOAT_EQ(*serial.report_at("y", ot::MAX, ot::RISE), 63.f);
  // Introduce a feedback loop with an external input and an acyclic tail.
  for (auto *timer : {&serial, &parallel})
    timer->disconnect_pin("c:A").connect_pin("c:A", "joined");
  compare();
  EXPECT_GT(serial.num_sccs(), 0U);
  // Only the sink is a forward candidate; its backward cone still contains
  // the previously discovered SCC and must respect its retained loop breakers.
  for (auto *timer : {&serial, &parallel})
    timer->set_rat("y", ot::MAX, ot::RISE, -20.f);
  compare();
  for (auto *timer : {&serial, &parallel})
    timer->disconnect_pin("c:A").connect_pin("c:A", "in");
  compare();
  EXPECT_EQ(serial.num_sccs(), 0U);
  // Two independent cycles feed the same acyclic output cone. Reordering must
  // honor both sets of loop breakers, including a later RAT-only update.
  for (auto *timer : {&serial, &parallel}) {
    timer->disconnect_pin("b:A").connect_pin("b:A", "b_out");
    timer->disconnect_pin("c:A").connect_pin("c:A", "c_out");
  }
  compare();
  EXPECT_EQ(serial.num_sccs(), 2U);
  for (auto *timer : {&serial, &parallel})
    timer->set_rat("y", ot::MAX, ot::RISE, -30.f);
  compare();
  for (auto *timer : {&serial, &parallel}) {
    timer->disconnect_pin("b:A").connect_pin("b:A", "in");
    timer->disconnect_pin("c:A").connect_pin("c:A", "in");
  }
  compare();
  EXPECT_EQ(serial.num_sccs(), 0U);
}

TEST_F(CelllibSmoke, InputDriverTracksLoadWithoutDoubleCountingIntrinsicDelay) {
  write(threshold_libraries());
  ot::Timer timer;
  timer.set_num_threads(1);
  timer.read_celllib(path).update_timing();
  build_threshold_chain(timer);
  for (auto split : {ot::MIN, ot::MAX}) {
    for (auto tran : {ot::RISE, ot::FALL}) {
      timer.set_input_driver("a", "BUF", "A", "Y", split, tran, {20, 20});
    }
  }
  timer.update_timing();
  for (auto split : {ot::MIN, ot::MAX}) {
    for (auto tran : {ot::RISE, ot::FALL}) {
      EXPECT_NEAR(timer.pins().at("a").at(split, tran).value(), 14, 1e-4);
      EXPECT_NEAR(timer.pins().at("a").slew(split, tran).value(), 16, 1e-4);
      EXPECT_NEAR(timer.pins().at("y").at(split, tran).value(), 59.25, 1e-4);
    }
  }
  timer.insert_gate("extra", "BUF").connect_pin("extra:A", "input").update_timing();
  EXPECT_NEAR(timer.pins().at("a").at(ot::MAX, ot::RISE).value(), 18, 1e-4);
  EXPECT_NEAR(timer.pins().at("a").slew(ot::MAX, ot::RISE).value(), 18, 1e-4);
}

TEST_F(CelllibSmoke, CapacitanceRangesUseEarlyAndLateValuesAndRebaseUnits) {
  auto              text = threshold_libraries();
  const std::string old  = "pin(A) { direction : input; capacitance : 0.002; }";
  const auto        pos  = text.find(old);
  ASSERT_NE(pos, std::string::npos);
  text.replace(pos, old.size(), R"lib(pin(A) { direction : input; capacitance : 0.002;
    rise_capacitance_range (0.001, 0.003);
    fall_capacitance_range (0.0005, 0.004);
  })lib");
  write(text);
  ot::Timer timer;
  timer.set_num_threads(1);
  timer.read_celllib(path).insert_gate("m", "RAM").update_timing();
  const auto& pin = timer.pins().at("m:A");
  EXPECT_NEAR(pin.cap(ot::MIN, ot::RISE), 1, 1e-5);
  EXPECT_NEAR(pin.cap(ot::MAX, ot::RISE), 3, 1e-5);
  EXPECT_NEAR(pin.cap(ot::MIN, ot::FALL), .5, 1e-5);
  EXPECT_NEAR(pin.cap(ot::MAX, ot::FALL), 4, 1e-5);
}
TEST_F(CelllibSmoke, ConcatenatedLibrariesRebaseUnitsAndKeepScopedDefaults) {
  write(R"lib(
    library(logic) {
      time_unit : "1ps"; capacitive_load_unit(1,ff);
      default_input_pin_cap : 2;
      lu_table_template(delay) { variable_1 : input_net_transition; index_1("1,2"); }
      cell(BUF) { area : 1;
        pin(A) { direction : input; }
        pin(Y) { direction : output; function : "A";
          timing() { related_pin : "A";
            cell_rise(delay) { values("3,4"); }
          }
        }
      }
    }
    library(ram) {
      time_unit : "1ns"; capacitive_load_unit(1,pf);
      default_input_pin_cap : 0.025;
      lu_table_template(mem_delay) { variable_1 : input_net_transition; index_1("0.1,0.2"); }
      cell(RAM) { area : 10;
        pin(CLK) { direction : input; clock : true; }
        pin(Q) { direction : output;
          timing() { related_pin : "CLK"; timing_type : rising_edge;
            cell_rise(mem_delay) { values("0.3,0.4"); }
          }
        }
      }
    }
  )lib");
  const auto libraries = ot::Celllib::read_all(path);
  ASSERT_EQ(libraries.size(), 2u);
  EXPECT_FLOAT_EQ(libraries[0].cell("BUF")->cellpin("A")->capacitance.value(), 2);
  EXPECT_FLOAT_EQ(libraries[1].cell("RAM")->cellpin("CLK")->capacitance.value(), .025);
  ot::Timer timer;
  timer.set_num_threads(1);
  timer.read_celllib(path).update_timing();
  for (const auto corner : {ot::MIN, ot::MAX}) {
    const auto& lib = timer.celllib(corner);
    ASSERT_TRUE(lib);
    ASSERT_NE(lib->cell("BUF"), nullptr);
    ASSERT_NE(lib->cell("RAM"), nullptr);
    EXPECT_NEAR(lib->cell("RAM")->cellpin("CLK")->capacitance.value(), 25, 1e-4);
    const auto& table = lib->cell("RAM")->cellpin("Q")->timings.front().cell_rise;
    ASSERT_TRUE(table);
    EXPECT_NEAR(table->table[0], 300, 1e-3);
    EXPECT_NEAR(table->indices1[0], 100, 1e-3);
    EXPECT_NEAR(lib->cell("BUF")->cellpin("Y")->timings.front().cell_rise->table[0], 3, 1e-5);
  }
}
TEST_F(CelllibSmoke, IdealClockPreservesInversionDataRcAndCanRestorePropagation) {
  write(R"lib(library(test) {
    time_unit: "1ps"; capacitive_load_unit(1,ff); pulling_resistance_unit: "1ohm";
    cell(BUF) { area: 1;
      pin(A) { direction: input; capacitance: 1; }
      pin(Y) { direction: output; function: "A";
        timing() { related_pin: "A"; timing_sense: positive_unate;
          cell_rise(scalar) { values("10"); } cell_fall(scalar) { values("10"); }
          rise_transition(scalar) { values("2"); } fall_transition(scalar) { values("2"); }
        }
      }
    }
    cell(INV) { area: 1;
      pin(A) { direction: input; capacitance: 1; }
      pin(Y) { direction: output; function: "!A";
        timing() { related_pin: "A"; timing_sense: negative_unate;
          cell_rise(scalar) { values("10"); } cell_fall(scalar) { values("10"); }
          rise_transition(scalar) { values("2"); } fall_transition(scalar) { values("2"); }
        }
      }
    }
    cell(ANDCLK) { area: 1;
      pin(A) {direction: input; capacitance: 1;}
      pin(EN) {direction: input; capacitance: 1;}
      pin(Y) {direction: output; function: "A & EN";
        timing() { related_pin: "A"; timing_sense: positive_unate;
          cell_rise(scalar) {values("10");} cell_fall(scalar) {values("10");}
          rise_transition(scalar) {values("2");} fall_transition(scalar) {values("2");}
        }
        timing() { related_pin: "EN"; timing_sense: positive_unate;
          cell_rise(scalar) {values("10");} cell_fall(scalar) {values("10");}
          rise_transition(scalar) {values("2");} fall_transition(scalar) {values("2");}
        }
      }
    }
    cell(FF) { area: 2;
      pin(CLK) { direction: input; clock: true; capacitance: 1; }
      pin(D) { direction: input; capacitance: 1;
        timing() { related_pin: "CLK"; timing_type: setup_rising;
          rise_constraint(scalar) { values("5"); } fall_constraint(scalar) { values("5"); }
        }
        timing() { related_pin: "CLK"; timing_type: hold_rising;
          rise_constraint(scalar) { values("2"); } fall_constraint(scalar) { values("2"); }
        }
      }
      pin(Q) { direction: output;
        timing() { related_pin: "CLK"; timing_type: rising_edge; timing_sense: non_unate;
          cell_rise(scalar) { values("3"); } cell_fall(scalar) { values("3"); }
          rise_transition(scalar) { values("2"); } fall_transition(scalar) { values("2"); }
        }
      }
    }
  })lib");
  ot::Timer timer;
  timer.set_num_threads(1);
  timer.read_celllib(path);
  for (const auto& name : {"clk", "data", "buf_clk", "inv_clk", "data_out", "gated_clk", "q0", "q1"}) {
    timer.insert_net(name);
  }
  timer.insert_primary_input("clk").insert_primary_input("data").insert_primary_output("q0").insert_primary_output("q1");
  timer.insert_gate("cb", "BUF")
      .insert_gate("ci", "INV")
      .insert_gate("db", "BUF")
      .insert_gate("cg", "ANDCLK")
      .insert_gate("f0", "FF")
      .insert_gate("f1", "FF");
  for (const auto& [pin, net] : std::vector<std::pair<std::string, std::string>>{
           {   "clk",       "clk"},
           {  "data",      "data"},
           {  "cb:A",       "clk"},
           {  "cb:Y",   "buf_clk"},
           {  "ci:A",   "buf_clk"},
           {  "ci:Y",   "inv_clk"},
           {  "db:A",      "data"},
           {  "db:Y",  "data_out"},
           {  "cg:A",   "buf_clk"},
           { "cg:EN",  "data_out"},
           {  "cg:Y", "gated_clk"},
           {"f0:CLK", "gated_clk"},
           {"f1:CLK",   "inv_clk"},
           {  "f0:D",  "data_out"},
           {  "f1:D",  "data_out"},
           {  "f0:Q",        "q0"},
           {  "f1:Q",        "q1"},
           {    "q0",        "q0"},
           {    "q1",        "q1"}
  }) {
    timer.connect_pin(pin, net);
  }
  timer.create_clock("clock", "clk", 1000);
  for (auto el : {ot::MIN, ot::MAX}) {
    for (auto rf : {ot::RISE, ot::FALL}) {
      timer.set_at("clk", el, rf, 0).set_slew("clk", el, rf, 0);
      timer.set_at("data", el, rf, 20).set_slew("data", el, rf, 2);
    }
  }
  spef::Spef parasitics;
  parasitics.capacitance_unit = "1 FF";
  parasitics.resistance_unit  = "1 OHM";
  for (const auto& [net, driver, sink] : std::vector<std::tuple<std::string, std::string, std::string>>{
           { "buf_clk", "cb:Y", "cg:A"},
           {"data_out", "db:Y", "f0:D"}
  }) {
    spef::Net n(net, 100);
    n.caps.emplace_back(sink, "", 100);
    n.ress.emplace_back(driver, sink, 1000);
    const auto other = net == "buf_clk" ? "ci:A" : "f1:D";
    n.caps.emplace_back(other, "", 0);
    n.ress.emplace_back(driver, other, 1000);
    if (net == "data_out") {
      n.caps.emplace_back("cg:EN", "", 0);
      n.ress.emplace_back(driver, "cg:EN", 1000);
    }
    parasitics.nets.push_back(std::move(n));
  }
  timer.read_parasitics(std::move(parasitics)).update_timing();
  const auto propagated = timer.pins().at("f0:CLK").at(ot::MAX, ot::RISE);
  const auto data_at    = timer.pins().at("f0:D").at(ot::MAX, ot::RISE);
  ASSERT_TRUE(propagated);
  ASSERT_TRUE(data_at);
  EXPECT_GT(*propagated, 10);
  EXPECT_GT(*data_at, 30);  // data RC was actually applied
  timer.set_ideal_clocks({"clock"}).update_timing();
  EXPECT_TRUE(timer.pins().at("cb:Y").is_ideal_clock());
  EXPECT_FALSE(timer.pins().at("db:Y").is_ideal_clock());
  EXPECT_FALSE(timer.pins().at("cg:EN").is_ideal_clock());
  EXPECT_TRUE(timer.pins().at("cg:Y").is_ideal_clock());
  EXPECT_EQ(timer.pins().at("f0:CLK").at(ot::MAX, ot::RISE), 0);
  EXPECT_EQ(timer.pins().at("f0:CLK").slew(ot::MAX, ot::RISE), 0);
  EXPECT_EQ(timer.pins().at("f1:CLK").at(ot::MAX, ot::RISE), 500);
  EXPECT_EQ(timer.pins().at("f1:CLK").at(ot::MAX, ot::FALL), 0);
  EXPECT_EQ(timer.pins().at("f0:D").at(ot::MAX, ot::RISE), data_at);
  EXPECT_FALSE(timer.pins().at("f0:Q").is_ideal_clock());  // never idealize clock-to-Q
  timer.set_ideal_clocks({}).update_timing();
  EXPECT_EQ(timer.pins().at("f0:CLK").at(ot::MAX, ot::RISE), propagated);
  EXPECT_FALSE(timer.pins().at("f0:CLK").is_ideal_clock());
}

TEST_F(CelllibSmoke, NamesSurviveCallerMutationRehashAndUnrelatedRemoval) {
  std::string source(160, 'x');
  const auto expected = source;
  ot::Pin standalone(source);
  source.assign(160, 'y');
  EXPECT_EQ(standalone.name(), expected);
  ot::Pin temporary(std::string(160, 'z'));
  EXPECT_EQ(temporary.name(), std::string(160, 'z'));

  write(R"lib(library(names) { cell(BUF) {
    pin(A) { direction : input; }
    pin(Y) { direction : output; function : "A"; }
  } })lib");
  ot::Timer timer;
  timer.read_celllib(path);
  timer.set_num_threads(1).insert_primary_input(expected).update_netlist();
  const auto *retained = &timer.pins().at(expected);
  const auto &name = retained->name();
  for (int i = 0; i < 512; ++i)
    timer.insert_gate("input_" + std::to_string(i), "BUF");
  timer.update_netlist();
  EXPECT_EQ(retained->name(), expected);
  EXPECT_EQ(name, expected);
  EXPECT_EQ(&timer.pins().at(expected), retained);
  for (int i = 0; i < 512; ++i)
    timer.remove_gate("input_" + std::to_string(i));
  timer.update_netlist();
  EXPECT_EQ(name, expected);
  timer.insert_gate(expected, "BUF").update_netlist();
  EXPECT_EQ(timer.pins().at(expected + ":A").name(), expected + ":A");
  timer.remove_gate(expected).update_netlist();
  timer.insert_gate(expected, "BUF").update_netlist();
  EXPECT_EQ(timer.pins().at(expected + ":A").name(), expected + ":A");
}

TEST_F(CelllibSmoke, ArcDelaysRefreshLoadsValidityAndIdealClockTransitions) {
  write(R"lib(library(delay_storage) {
    time_unit : "1ps"; capacitive_load_unit(1,ff);
    lu_table_template(load) {
      variable_1 : total_output_net_capacitance; index_1("0, 10");
    }
    cell(BUF) {
      pin(A) { direction : input; capacitance : 1; }
      pin(Y) { direction : output; function : "A";
        timing() { related_pin : "A"; timing_sense : positive_unate;
          cell_rise(load) { values("10, 20"); }
          cell_fall(load) { values("20, 30"); }
          rise_transition(scalar) { values("3"); }
          fall_transition(scalar) { values("4"); }
        }
      }
    }
  })lib");
  for (unsigned workers : {1U, 3U}) {
    ot::Timer timer;
    timer.set_num_threads(workers).read_celllib(path);
    timer.insert_primary_input("a").insert_primary_output("y");
    timer.insert_gate("buf", "BUF").insert_net("out");
    timer.connect_pin("buf:A", "a").connect_pin("buf:Y", "out");
    timer.disconnect_pin("y").connect_pin("y", "out");
    for (auto split : {ot::MIN, ot::MAX}) {
      for (auto tran : {ot::RISE, ot::FALL}) {
        timer.set_at("a", split, tran, 0).set_slew("a", split, tran, 2);
      }
    }
    timer.update_timing();
    std::array<const ot::Arc *, 2> gate{};
    for (const auto &arc : timer.arcs()) {
      if (arc.is_cell_arc()) {
        for (auto split : {ot::MIN, ot::MAX}) {
          if (arc.timing_view()[split])
            gate[split] = &arc;
        }
      }
    }
    ASSERT_NE(gate[ot::MIN], nullptr);
    ASSERT_NE(gate[ot::MAX], nullptr);
    for (auto model :
         {ot::RcModel::elmore, ot::RcModel::effective_capacitance}) {
      timer.set_rc_model(model);
      for (float load : {2.f, 7.f, 2.f}) {
        for (auto split : {ot::MIN, ot::MAX}) {
          for (auto tran : {ot::RISE, ot::FALL})
            timer.set_load("y", split, tran, load);
        }
        for (uint8_t mask : {15, 1, 0, 8, 15}) {
          timer.set_case_masks({{"buf:A", "buf:Y", mask}}).update_timing();
          for (auto split : {ot::MIN, ot::MAX}) {
            for (auto from : {ot::RISE, ot::FALL}) {
              for (auto to : {ot::RISE, ot::FALL}) {
                const auto expected =
                    from == to && (mask & (1U << (2 * from + to)))
                        ? std::optional<float>((to == ot::RISE ? 10 : 20) +
                                               load)
                        : std::nullopt;
                EXPECT_EQ(gate[split]->delay(split, from, to), expected);
              }
              const auto expected =
                  (mask & (1U << (3 * from)))
                      ? std::optional<float>((from == ot::RISE ? 10 : 20) +
                                             load)
                      : std::nullopt;
              EXPECT_EQ(timer.report_at("y", split, from), expected);
            }
          }
        }
      }
    }
    timer.create_clock("clk", "a", 100)
        .set_ideal_clocks({"clk"}, {"y"})
        .update_timing();
    for (auto split : {ot::MIN, ot::MAX}) {
      EXPECT_EQ(gate[split]->delay(split, ot::RISE, ot::RISE), 0);
      EXPECT_EQ(gate[split]->delay(split, ot::FALL, ot::FALL), 0);
      EXPECT_FALSE(gate[split]->delay(split, ot::RISE, ot::FALL));
      EXPECT_FALSE(gate[split]->delay(split, ot::FALL, ot::RISE));
      EXPECT_EQ(timer.report_at("y", split, ot::RISE), 0);
      EXPECT_EQ(timer.report_at("y", split, ot::FALL), 50);
    }
    timer.set_ideal_clocks({}).update_timing();
    for (auto split : {ot::MIN, ot::MAX}) {
      EXPECT_EQ(gate[split]->delay(split, ot::RISE, ot::RISE), 12);
      EXPECT_EQ(gate[split]->delay(split, ot::FALL, ot::FALL), 22);
      timer.set_slew("a", split, ot::RISE, std::nullopt);
    }
    timer.update_timing();
    for (auto split : {ot::MIN, ot::MAX}) {
      EXPECT_FALSE(gate[split]->delay(split, ot::RISE, ot::RISE));
      EXPECT_EQ(gate[split]->delay(split, ot::FALL, ot::FALL), 22);
    }
  }
}

TEST_F(CelllibSmoke, SharedCornerArcsPreserveTablesMasksAndReplacement) {
  const auto timing = [](std::string pin, int delay, bool condition) {
    return "timing() { related_pin : \"" + pin +
           "\"; timing_sense : positive_unate; " +
           (condition ? "when : \"A\"; " : "") +
           "cell_rise(scalar) { values(\"" + std::to_string(delay) + "\"); } " +
           "cell_fall(scalar) { values(\"" + std::to_string(delay + 1) +
           "\"); } " +
           "rise_transition(scalar) { values(\"2\"); } fall_transition(scalar) "
           "{ values(\"3\"); } }";
  };
  const auto library = [&](int offset, bool reverse, bool condition) {
    std::string body =
        "library(corners) { time_unit : \"1ps\"; capacitive_load_unit(1,ff); ";
    for (auto name : {std::string("JOIN"), std::string("JOIN2")}) {
      const int bias = offset + (name == "JOIN2" ? 7 : 0);
      const auto a = timing("A", bias + 10, false),
                 b = timing("B", bias + 20, condition);
      body += "cell(" + name +
              ") { pin(A) { direction : input; capacitance : 1; } "
              "pin(B) { direction : input; capacitance : 1; } "
              "pin(Y) { direction : output; function : \"A | B\"; " +
              (reverse ? b + a : a + b) + "} } ";
    }
    return body + "}";
  };
  struct ExtraFile {
    std::filesystem::path path;
    ~ExtraFile() { std::filesystem::remove(path); }
  } late{path.string() + ".late"};
  // Different tables can share an arc. Different authored order or conditions
  // must retain separate arcs even when both connect the same pins.
  for (int mismatch : {0, 1, 2}) {
    write(library(0, false, false));
    std::ofstream(late.path) << library(100, mismatch == 1, mismatch == 2);
    for (unsigned workers : {1U, 3U}) {
      ot::Timer timer;
      timer.set_num_threads(workers)
          .read_celllib(path, ot::MIN)
          .read_celllib(late.path, ot::MAX);
      timer.insert_primary_input("a")
          .insert_primary_input("b")
          .insert_primary_output("y");
      timer.insert_gate("g", "JOIN").insert_net("out");
      timer.connect_pin("g:A", "a")
          .connect_pin("g:B", "b")
          .connect_pin("g:Y", "out");
      timer.disconnect_pin("y").connect_pin("y", "out");
      for (auto split : {ot::MIN, ot::MAX}) {
        for (auto tran : {ot::RISE, ot::FALL}) {
          for (auto input : {"a", "b"})
            timer.set_at(input, split, tran, 0).set_slew(input, split, tran, 2);
        }
      }
      timer.update_timing();
      size_t cell_arcs = 0, paired = 0;
      for (const auto &arc : timer.arcs()) {
        if (!arc.is_cell_arc())
          continue;
        ++cell_arcs;
        paired += arc.timing_view()[ot::MIN] && arc.timing_view()[ot::MAX];
      }
      EXPECT_EQ(cell_arcs, mismatch ? 4u : 2u);
      EXPECT_EQ(paired, mismatch ? 0u : 2u);
      for (auto tran : {ot::RISE, ot::FALL}) {
        EXPECT_EQ(timer.report_at("y", ot::MIN, tran), 10 + tran);
        EXPECT_EQ(timer.report_at("y", ot::MAX, tran), 120 + tran);
      }
      timer.set_case_masks({{"g:B", "g:Y", 0}}).update_timing();
      timer.repower_gate("g", "JOIN2").update_timing();
      for (auto tran : {ot::RISE, ot::FALL}) {
        EXPECT_EQ(timer.report_at("y", ot::MIN, tran), 17 + tran);
        EXPECT_EQ(timer.report_at("y", ot::MAX, tran), 117 + tran);
      }
      timer.set_case_masks({}).update_timing();
      for (auto tran : {ot::RISE, ot::FALL}) {
        EXPECT_EQ(timer.report_at("y", ot::MIN, tran), 17 + tran);
        EXPECT_EQ(timer.report_at("y", ot::MAX, tran), 127 + tran);
      }
      timer.remove_gate("g").update_timing();
      EXPECT_TRUE(timer.arcs().empty());
    }
  }
}
}  // namespace
