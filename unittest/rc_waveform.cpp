// This file is distributed under the MIT License. See LICENSE for details.
#include <array>
#include <filesystem>
#include <fstream>
#include <limits>
#include <gtest/gtest.h>
#include <ot/timer/rc_waveform.hpp>
#include <ot/timer/timer.hpp>

TEST(RcWaveform, StepUsesVoltageCrossingAndThresholdSlew) {
  ot::SlewThresholds thresholds;
  for (auto edge : {ot::RISE, ot::FALL}) {
    const auto r = ot::rc_wire_response(100, 0, thresholds, thresholds, edge);
    EXPECT_NEAR(r.delay, 100 * std::log(2.), 1e-4);
    EXPECT_NEAR(r.slew, 100 * std::log(4.), 1e-4);
  }
}

TEST(RcWaveform, ZeroWirePreservesLibraryConventions) {
  ot::SlewThresholds from, to;
  from.lower = {.1f, .1f};
  from.upper = {.9f, .9f};
  from.derate = .8f;
  to.input = {.4f, .4f};
  to.derate = .5f;
  const auto rise = ot::rc_wire_response(0, 80, from, to, ot::RISE);
  const auto fall = ot::rc_wire_response(0, 80, from, to, ot::FALL);
  EXPECT_NEAR(rise.delay, -8, 1e-4);
  EXPECT_NEAR(fall.delay, 8, 1e-4);
  EXPECT_NEAR(rise.slew, 96, 1e-4);
  EXPECT_NEAR(fall.slew, 96, 1e-4);
}

TEST(RcWaveform, FiniteRampCrossingSatisfiesCircuitEquation) {
  for (double tau : {1e-8, .1, 1., 10., 100., 1e6}) {
    for (double duration : {1e-8, .1, 1., 10., 100., 1e6}) {
      double previous = -1;
      for (double v : {.1, .2, .5, .8, .9}) {
        const auto t = ot::rc_ramp_crossing(tau, duration, v);
        ASSERT_TRUE(std::isfinite(t));
        EXPECT_GE(t, previous);
        previous = t;
        // Independent convolution: y(t)=(1/T) integral_0^min(t,T)
        // (1-exp(-(t-u)/tau)) du. Stable expressions on either side of T.
        const double y =
            t < duration ? (t + tau * std::expm1(-t / tau)) / duration
                         : 1 - (tau / duration) * -std::expm1(-duration / tau) *
                                   std::exp(-(t - duration) / tau);
        EXPECT_NEAR(y, v, 2e-7) << tau << ' ' << duration << ' ' << v;
      }
    }
  }
}

TEST(RcWaveform, PreparedRampHandlesRepeatedOutOfOrderBoundaryQueries) {
  // Exercise both solver boundaries and the ramp/tail crossing. The circuit
  // equation below is independent of the threshold solver and cached values.
  for (double boundary : {1e-4, 1e8}) {
    for (double ratio : {std::nextafter(boundary, 0.), boundary,
                         std::nextafter(boundary,
                                        std::numeric_limits<double>::infinity())}) {
      const double tau = 100, duration = tau * ratio;
      const double r = duration / tau;
      const double end_voltage =
          r < 1e-4 ? r * (.5 - r / 6 + r * r / 24)
                   : 1 + std::expm1(-r) / r;
      const std::array<double, 6> voltages{
          .8, .2, std::nextafter(end_voltage, 0.), end_voltage,
          std::nextafter(end_voltage, 1.), .5};
      ot::detail::RcRamp ramp(tau, duration);
      std::array<double, 6> crossings{};
      for (size_t i = 0; i < voltages.size(); ++i) {
        const double t = ramp.crossing(voltages[i]);
        ASSERT_TRUE(std::isfinite(t));
        crossings[i] = t;
        const long double x = t, pole = tau, width = duration;
        const long double y =
            x < width ? (x + pole * std::expm1(-x / pole)) / width
                      : 1 - (pole / width) * -std::expm1(-width / pole) *
                                std::exp(-(x - width) / pole);
        EXPECT_NEAR(y, voltages[i], 2e-6) << ratio << ' ' << voltages[i];
      }
      for (size_t i = voltages.size(); i-- > 0;)
        EXPECT_EQ(ramp.crossing(voltages[i]), crossings[i]);
    }
  }
}

TEST(RcWaveform, FastAndSlowDriverLimits) {
  EXPECT_NEAR(ot::rc_ramp_crossing(100, 0, .5), 100 * std::log(2.), 1e-10);
  EXPECT_NEAR(ot::rc_ramp_crossing(1, 1e5, .5) - .5e5, 1, 1e-7);
  EXPECT_NEAR(ot::rc_ramp_crossing(0, 100, .25), 25, 1e-10);
}

TEST(RcWaveform, TimerPropagatesSinglePoleThresholdsAndIncrementalRc) {
  ot::Timer timer;
  timer.set_num_threads(1);
  timer.set_time_unit(ot::second_t(1e-12));
  timer.set_capacitance_unit(ot::farad_t(1e-15));
  timer.set_resistance_unit(ot::ohm_t(1e3));
  timer.insert_primary_input("a").insert_primary_output("y").insert_net("wire");
  timer.disconnect_pin("a")
      .disconnect_pin("y")
      .connect_pin("a", "wire")
      .connect_pin("y", "wire");
  for (auto split : {ot::MIN, ot::MAX}) {
    for (auto edge : {ot::RISE, ot::FALL}) {
      timer.set_at("a", split, edge, 0).set_slew("a", split, edge, 0);
      timer.set_rat("y", split, edge, 1000);
    }
  }
  for (float resistance : {1000.f, 2000.f, 1000.f}) {
    spef::Spef data;
    data.capacitance_unit = "1 FF";
    data.resistance_unit = "1 OHM";
    spef::Net net("wire", 100);
    net.caps.emplace_back("a", "", 0);
    net.caps.emplace_back("y", "", 100);
    net.ress.emplace_back("a", "y", resistance);
    data.nets.push_back(std::move(net));
    timer.read_parasitics(std::move(data));
    for (auto edge : {ot::RISE, ot::FALL}) {
      const double tau = resistance * .1;
      EXPECT_NEAR(timer.report_at("y", ot::MAX, edge).value(),
                  tau * std::log(2.), 1e-4);
      EXPECT_NEAR(timer.report_slew("y", ot::MAX, edge).value(),
                  tau * std::log(4.), 1e-4);
    }
  }
}

TEST(RcWaveform, MaterializedEditsRetainDirtyTimingUntilTheFinalQuery) {
  ot::Timer deferred, reference;
  const auto build = [](ot::Timer &timer) {
    timer.set_time_unit(ot::second_t(1e-12));
    timer.set_capacitance_unit(ot::farad_t(1e-15));
    timer.set_resistance_unit(ot::ohm_t(1e3));
    timer.insert_primary_input("a").insert_primary_output("y").insert_net(
        "wire");
    timer.disconnect_pin("a")
        .disconnect_pin("y")
        .connect_pin("a", "wire")
        .connect_pin("y", "wire");
    for (auto split : {ot::MIN, ot::MAX}) {
      for (auto edge : {ot::RISE, ot::FALL}) {
        timer.set_at("a", split, edge, 0).set_slew("a", split, edge, 0);
        timer.set_rat("y", split, edge, 1000);
      }
    }
  };
  const auto rc = [](ot::Timer &timer, float resistance) {
    spef::Spef data;
    data.capacitance_unit = "1 FF";
    data.resistance_unit = "1 OHM";
    spef::Net net("wire", 100);
    net.caps.emplace_back("a", "", 0);
    net.caps.emplace_back("y", "", 100);
    net.ress.emplace_back("a", "y", resistance);
    data.nets.push_back(std::move(net));
    timer.read_parasitics(std::move(data));
  };
  build(deferred);
  build(reference);
  deferred.update_netlist();
  ASSERT_TRUE(deferred.pins().contains("y"));
  ASSERT_EQ(deferred.pins().at("y").net()->name(), "wire");
  EXPECT_FALSE(deferred.pins().at("y").at(ot::MAX, ot::RISE));
  // Separate builder barriers must preserve all dirty frontiers, including
  // after the last barrier when no new task is queued before the timing query.
  for (auto model : {ot::RcModel::effective_capacitance, ot::RcModel::elmore,
                     ot::RcModel::effective_capacitance}) {
    deferred.set_rc_model(model);
    reference.set_rc_model(model);
    deferred.update_netlist();
    for (float resistance : {1000.f, 2000.f}) {
      const auto old = deferred.pins().at("y").at(ot::MAX, ot::RISE);
      rc(deferred, resistance);
      rc(reference, resistance);
      deferred.update_netlist();
      deferred.update_netlist(); // idempotent structural barrier
      EXPECT_EQ(deferred.pins().at("y").at(ot::MAX, ot::RISE), old);
      EXPECT_EQ(deferred.report_at("y", ot::MAX, ot::RISE),
                reference.report_at("y", ot::MAX, ot::RISE));
      EXPECT_EQ(deferred.report_slew("y", ot::MAX, ot::RISE),
                reference.report_slew("y", ot::MAX, ot::RISE));
      EXPECT_EQ(deferred.report_slack("y", ot::MAX, ot::RISE),
                reference.report_slack("y", ot::MAX, ot::RISE));
    }
  }
  // Deleting connectivity on a materialized dirty frontier must not leave
  // stale arc tasks behind when propagation is finally requested.
  deferred.set_rat("y", ot::MAX, ot::RISE, 3);
  deferred.update_netlist();
  deferred.remove_net("wire");
  deferred.update_netlist();
  deferred.update_timing();
  EXPECT_EQ(deferred.pins().at("y").net(), nullptr);
}

TEST(RcWaveform, RankingModelTracksRcAndCanReturnToDetailedTiming) {
  ot::Timer timer;
  timer.set_num_threads(1).set_rc_model(ot::RcModel::elmore);
  timer.set_time_unit(ot::second_t(1e-12));
  timer.set_capacitance_unit(ot::farad_t(1e-15));
  timer.set_resistance_unit(ot::ohm_t(1e3));
  timer.insert_primary_input("a").insert_primary_output("y").insert_net("wire");
  timer.disconnect_pin("a")
      .disconnect_pin("y")
      .connect_pin("a", "wire")
      .connect_pin("y", "wire");
  for (auto split : {ot::MIN, ot::MAX})
    for (auto edge : {ot::RISE, ot::FALL})
      timer.set_at("a", split, edge, 0)
          .set_slew("a", split, edge, 0)
          .set_rat("y", split, edge, 1000);
  for (const auto model :
       {ot::RcModel::elmore, ot::RcModel::effective_capacitance,
        ot::RcModel::elmore}) {
    timer.set_rc_model(model);
    for (float resistance : {1000.f, 2000.f, 1000.f}) {
      spef::Spef data;
      data.capacitance_unit = "1 FF";
      data.resistance_unit = "1 OHM";
      spef::Net net("wire", 100);
      net.caps.emplace_back("a", "", 0);
      net.caps.emplace_back("y", "", 100);
      net.ress.emplace_back("a", "y", resistance);
      data.nets.push_back(std::move(net));
      timer.read_parasitics(std::move(data));
      for (auto edge : {ot::RISE, ot::FALL}) {
        const double tau = resistance * .1;
        EXPECT_NEAR(timer.report_at("y", ot::MAX, edge).value(),
                    tau * (model == ot::RcModel::elmore ? 1. : std::log(2.)),
                    1e-4);
        EXPECT_NEAR(timer.report_slew("y", ot::MAX, edge).value(),
                    tau * std::log(4.), 1e-4);
      }
    }
  }
}

TEST(RcWaveform, RemovingAnAnnotatedGateInvalidatesItsOldRcPins) {
  const auto path =
      std::filesystem::path(std::getenv("TEST_TMPDIR")) / "rc_reconnect.lib";
  std::ofstream(path) << R"lib(library(rc) {
    time_unit : "1ps"; capacitive_load_unit(1,ff);
    pulling_resistance_unit : "1kohm";
    cell(EMPTY) { area : 3; }
    cell(BUF) { area : 1;
      pin(A) { direction : input; capacitance : 1; }
      pin(Y) { direction : output; function : "A";
        timing() { related_pin : "A"; timing_sense : positive_unate;
          cell_rise(scalar) { values("1"); } cell_fall(scalar) { values("1"); }
          rise_transition(scalar) { values("1"); } fall_transition(scalar) { values("1"); }
        }
      }
    }
  })lib";
  ot::Timer timer;
  timer.set_num_threads(1).read_celllib(path);
  timer.insert_primary_input("a").insert_primary_output("y");
  timer.insert_gate("u", "BUF").insert_gate("v", "BUF");
  for (const auto name : {"in", "middle", "out"})
    timer.insert_net(name);
  timer.disconnect_pin("a").connect_pin("a", "in").connect_pin("u:A", "in");
  timer.connect_pin("u:Y", "middle").connect_pin("v:A", "middle");
  timer.connect_pin("v:Y", "out").disconnect_pin("y").connect_pin("y", "out");
  for (auto split : {ot::MIN, ot::MAX})
    for (auto edge : {ot::RISE, ot::FALL})
      timer.set_at("a", split, edge, 0)
          .set_slew("a", split, edge, 1)
          .set_rat("y", split, edge, 1000);
  const auto annotate = [&] {
    spef::Spef data;
    data.capacitance_unit = "1 FF";
    data.resistance_unit = "1 OHM";
    spef::Net net("middle", 10);
    net.caps.emplace_back("u:Y", "", 0);
    net.caps.emplace_back("v:A", "", 10);
    net.ress.emplace_back("u:Y", "v:A", 1000);
    data.nets.push_back(std::move(net));
    timer.read_parasitics(std::move(data));
  };
  annotate();
  for (auto edge : {ot::RISE, ot::FALL}) {
    // The scalar Liberty delay has no load slope: full 10 fF wire + 1 fF
    // sink capacitance, 1 kohm -> 11 ps Elmore, plus two 1 ps gates.
    // Do not infer a waveform or artificially broaden the 1 ps transition.
    EXPECT_NEAR(timer.report_at("y", ot::MAX, edge).value(), 13, 1e-4);
    EXPECT_NEAR(timer.report_slew("v:A", ot::MAX, edge).value(), 1, 1e-5);
  }
  timer.remove_gate("v").disconnect_pin("y").connect_pin("y", "middle");
  timer.update_netlist();
  EXPECT_NEAR(timer.report_at("y", ot::MAX, ot::RISE).value(), 1, 1e-5);
  timer.insert_gate("v", "BUF").connect_pin("v:A", "middle");
  timer.connect_pin("v:Y", "out").disconnect_pin("y").connect_pin("y", "out");
  annotate();
  for (auto edge : {ot::RISE, ot::FALL}) {
    // The scalar Liberty delay has no load slope: full 10 fF wire + 1 fF
    // sink capacitance, 1 kohm -> 11 ps Elmore, plus two 1 ps gates.
    // Do not infer a waveform or artificially broaden the 1 ps transition.
    EXPECT_NEAR(timer.report_at("y", ot::MAX, edge).value(), 13, 1e-4);
    EXPECT_NEAR(timer.report_slew("v:A", ot::MAX, edge).value(), 1, 1e-5);
  }
  EXPECT_EQ(timer.report_area(), 2);
  // A cell with no pins changes area without creating timing frontiers. The
  // materialization barrier must still invalidate aggregate report caches.
  timer.insert_gate("physical_only", "EMPTY");
  timer.update_netlist();
  EXPECT_EQ(timer.report_area(), 5);
  timer.remove_gate("physical_only");
  timer.update_netlist();
  EXPECT_EQ(timer.report_area(), 2);
}
