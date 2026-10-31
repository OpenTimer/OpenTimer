// This file is distributed under the MIT License. See LICENSE for details.
#include <gtest/gtest.h>
#include <ot/timer/timer.hpp>
#include <unistd.h>

namespace {
class PathReport : public testing::Test {
protected:
  ot::Timer timer;
  std::filesystem::path library;

  void SetUp() override {
    auto name =
        (std::filesystem::temp_directory_path() / "ot-path-report-XXXXXX")
            .string();
    const auto fd = mkstemp(name.data());
    ASSERT_GE(fd, 0);
    close(fd);
    library = name;
    std::ofstream(library) << R"lib(library(paths) {
      time_unit : "1ps"; capacitive_load_unit(1,ff);
      cell(INV) {
        pin(A) { direction : input; capacitance : 1; }
        pin(Y) { direction : output; function : "!A";
          timing() { related_pin : "A"; timing_sense : negative_unate;
            cell_rise(scalar) { values("3"); } cell_fall(scalar) { values("5"); }
            rise_transition(scalar) { values("1"); } fall_transition(scalar) { values("1"); }
          }
        }
      }
      cell(NAND) {
        pin(A) { direction : input; capacitance : 1; }
        pin(B) { direction : input; capacitance : 1; }
        pin(Y) { direction : output; function : "!(A*B)";
          timing() { related_pin : "A"; timing_sense : negative_unate;
            cell_rise(scalar) { values("7"); } cell_fall(scalar) { values("11"); }
            rise_transition(scalar) { values("1"); } fall_transition(scalar) { values("1"); }
          }
          timing() { related_pin : "B"; timing_sense : negative_unate;
            cell_rise(scalar) { values("13"); } cell_fall(scalar) { values("17"); }
            rise_transition(scalar) { values("1"); } fall_transition(scalar) { values("1"); }
          }
        }
      }
      cell(FF) {
        ff(IQ,IQN) { clocked_on : "C"; next_state : "D"; }
        pin(C) { direction : input; clock : true; capacitance : 1; }
        pin(D) { direction : input; capacitance : 1;
          timing() { related_pin : "C"; timing_type : setup_rising;
            rise_constraint(scalar) { values("2"); } fall_constraint(scalar) { values("2"); }
          }
          timing() { related_pin : "C"; timing_type : hold_rising;
            rise_constraint(scalar) { values("1000"); } fall_constraint(scalar) { values("1000"); }
          }
        }
        pin(Q) { direction : output; function : "IQ";
          timing() { related_pin : "C"; timing_type : rising_edge; timing_sense : non_unate;
            cell_rise(scalar) { values("19"); } cell_fall(scalar) { values("23"); }
            rise_transition(scalar) { values("1"); } fall_transition(scalar) { values("1"); }
          }
        }
      }
    })lib";
    timer.set_num_threads(1).read_celllib(library);
    timer.insert_primary_input("a")
        .insert_primary_input("b")
        .insert_primary_output("y");
    timer.insert_gate("inv", "INV").insert_gate("join", "NAND");
    timer.insert_net("mid").insert_net("out");
    timer.connect_pin("inv:A", "a").connect_pin("inv:Y", "mid");
    timer.connect_pin("join:A", "mid")
        .connect_pin("join:B", "b")
        .connect_pin("join:Y", "out");
    timer.disconnect_pin("y").connect_pin("y", "out");
    for (auto split : {ot::MIN, ot::MAX}) {
      for (auto rf : {ot::RISE, ot::FALL}) {
        timer.set_slew("a", split, rf, 1).set_slew("b", split, rf, 1);
      }
    }
  }

  void TearDown() override { std::filesystem::remove(library); }

  void compare(ot::Split split, ot::Tran rf) {
    // K>1 exercises the independent suffix/prefix-tree enumerator, including
    // alternate sources through the reconvergent cell.
    auto single = timer.report_timing(1, split, rf);
    auto reference = timer.report_timing(2, split, rf);
    ASSERT_EQ(single.size(), 1U);
    ASSERT_FALSE(reference.empty());
    const auto &a = single.front();
    const auto &b = reference.front();
    ASSERT_EQ(a.size(), b.size());
    EXPECT_FLOAT_EQ(a.slack, b.slack);
    auto p = a.begin();
    for (const auto &q : b) {
      EXPECT_EQ(p->pin.name(), q.pin.name());
      EXPECT_EQ(p->transition, q.transition);
      EXPECT_FLOAT_EQ(p->at, q.at);
      EXPECT_FLOAT_EQ(p->ipower, q.ipower);
      ++p;
    }
  }

  void arrivals(float a, float b) {
    for (auto split : {ot::MIN, ot::MAX}) {
      for (auto rf : {ot::RISE, ot::FALL}) {
        timer.set_at("a", split, rf, a + 2 * rf);
        timer.set_at("b", split, rf, b + 3 * rf);
      }
    }
  }
};

TEST_F(PathReport, MatchesEnumerationAfterArrivalAndCaseUpdates) {
  for (unsigned workers : {1U, 3U}) {
    timer.set_num_threads(workers);
    for (auto split : {ot::MIN, ot::MAX}) {
      for (auto rf : {ot::RISE, ot::FALL}) {
        timer.set_rat("y", split, rf, split == ot::MIN ? 10000.f : -10000.f);
      }
    }
    for (int edit = 0; edit < 4; ++edit) {
      arrivals(edit % 2 ? 90.f : 0.f, edit % 2 ? 0.f : 90.f);
      timer.set_case_masks(
          edit < 2
              ? std::vector<std::tuple<std::string, std::string, uint8_t>>{}
              : std::vector<std::tuple<std::string, std::string, uint8_t>>{
                    {"join:B", "join:Y", 0}});
      for (auto split : {ot::MIN, ot::MAX}) {
        for (auto rf : {ot::RISE, ot::FALL})
          compare(split, rf);
      }
    }
  }
}

TEST_F(PathReport, SequentialEndpointsAndClockSourcesMatchWithAndWithoutCppr) {
  timer.insert_primary_input("clk")
      .insert_gate("ff", "FF")
      .insert_primary_output("q")
      .insert_net("qout");
  timer.connect_pin("ff:C", "clk")
      .connect_pin("ff:D", "out")
      .connect_pin("ff:Q", "qout");
  timer.disconnect_pin("q").connect_pin("q", "qout").create_clock("clk", 1.f);
  for (auto split : {ot::MIN, ot::MAX}) {
    for (auto rf : {ot::RISE, ot::FALL}) {
      timer.set_slew("clk", split, rf, 1)
          .set_at("clk", split, rf, rf == ot::RISE ? 0.f : 0.5f);
    }
  }
  arrivals(20, 40);
  for (bool cppr : {false, true, false}) {
    timer.cppr(cppr);
    for (auto split : {ot::MIN, ot::MAX}) {
      for (auto rf : {ot::RISE, ot::FALL}) {
        compare(split, rf);
        const auto paths = timer.report_timing(1, split, rf);
        ASSERT_EQ(paths.size(), 1U);
        ASSERT_NE(paths[0].endpoint->test(), nullptr);
        EXPECT_EQ(paths[0].back().pin.name(), "ff:D");
      }
    }
  }
  // A launched path starts at the clock cell pin, not at the upstream clock
  // port. This is the same boundary used by suffix-tree recovery.
  timer.set_rat("q", ot::MAX, ot::RISE, -10000.f);
  compare(ot::MAX, ot::RISE);
  const auto paths = timer.report_timing(1, ot::MAX, ot::RISE);
  ASSERT_EQ(paths.size(), 1U);
  ASSERT_FALSE(paths[0].empty());
  EXPECT_EQ(paths[0].front().pin.name(), "ff:C");
  EXPECT_EQ(paths[0].back().pin.name(), "q");
}
} // namespace
