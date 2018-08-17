#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include <ot/unit/unit.hpp>
/*
// Testcase: TimeUnit
TEST_CASE("TimeUnit") {

  REQUIRE(!ot::make_time_unit("123"));
  REQUIRE(!ot::make_time_unit("abc"));

  auto usunit = ot::make_time_unit("+200us");
  auto nsunit = ot::make_time_unit("1ns");
  auto psunit = ot::make_time_unit("1.5ps");

  REQUIRE(usunit.has_value());
  REQUIRE(nsunit.has_value());
  REQUIRE(psunit.has_value());

  auto us = std::get<ot::microseconds>(*usunit);
  auto ns = std::get<ot::nanoseconds>(*nsunit);
  auto ps = std::get<ot::picoseconds>(*psunit);

  REQUIRE(std::fabs(us.count() - 200.0f) < 1e-6);
  REQUIRE(std::fabs(ns.count() - 1.0f) < 1e-6);
  REQUIRE(std::fabs(ps.count() - 1.5f) < 1e-6);
  REQUIRE(std::fabs(us/ns - 200000.0f) < 1e-6);
  REQUIRE(std::fabs(ps/ns - 0.0015f) < 1e-6);

  REQUIRE(std::fabs(ot::divide_time_unit(*usunit, *nsunit) - 200000.0f) < 1e-6);
  REQUIRE(std::fabs(ot::divide_time_unit(*psunit, *nsunit) - 0.0015f) < 1e-6);
}

// Testcase: VoltageUnit
TEST_CASE("VoltageUnit") {

  REQUIRE(!ot::make_voltage_unit("123"));
  REQUIRE(!ot::make_voltage_unit("abc"));
  
  auto mVunit = ot::make_voltage_unit("200mV");
  auto Vunit  = ot::make_voltage_unit("1V");

  REQUIRE(mVunit.has_value());
  REQUIRE(Vunit.has_value());

  auto mV = std::get<ot::millivolts>(*mVunit);
  auto V = std::get<ot::volts>(*Vunit);

  REQUIRE(std::fabs(mV.count() - 200.0f) < 1e-6);
  REQUIRE(std::fabs(V.count() - 1.0f) < 1e-6);
  REQUIRE(std::fabs(V/mV - 5.0f) < 1e-6);
  REQUIRE(std::fabs(mV/V - 0.2f) < 1e-6);

  REQUIRE(std::fabs(ot::divide_voltage_unit(*Vunit, *mVunit) - 5.0f) < 1e-6);
  REQUIRE(std::fabs(ot::divide_voltage_unit(*mVunit, *Vunit) - 0.2f) < 1e-6);
}

// Testcase: CapacitanceUnit
TEST_CASE("CapacitanceUnit") {

  REQUIRE(!ot::make_capacitance_unit("123"));
  REQUIRE(!ot::make_capacitance_unit("abc"));
  
  auto pfunit = ot::make_capacitance_unit("200pf");
  auto ffunit = ot::make_capacitance_unit("1ff");

  REQUIRE(pfunit.has_value());
  REQUIRE(ffunit.has_value());

  auto pf = std::get<ot::picofarads>(*pfunit);
  auto ff = std::get<ot::femtofarads>(*ffunit);

  REQUIRE(std::fabs(pf.count() - 200.0f) < 1e-6);
  REQUIRE(std::fabs(ff.count() - 1.0f) < 1e-6);
  REQUIRE(std::fabs(pf/ff - 200000.0f) < 1e-6);
  REQUIRE(std::fabs(ff/pf - 0.000005f) < 1e-6);

  REQUIRE(std::fabs(ot::divide_capacitance_unit(*pfunit, *ffunit) - 200000.0f) < 1e-6);
  REQUIRE(std::fabs(ot::divide_capacitance_unit(*ffunit, *pfunit) - 0.000005f) < 1e-6);
}

// Testcase: CurrentUnit
TEST_CASE("CurrentUnit") {
  
  REQUIRE(!ot::make_current_unit("123"));
  REQUIRE(!ot::make_current_unit("abc"));
  
  auto uAunit = ot::make_current_unit("200uA");
  auto mAunit = ot::make_current_unit("200mA");
  auto Aunit  = ot::make_current_unit("200A");
  auto kAunit = ot::make_current_unit("200kA");
  
  REQUIRE(uAunit.has_value());
  REQUIRE(mAunit.has_value());
  REQUIRE(Aunit.has_value());
  REQUIRE(kAunit.has_value());

  auto uA = std::get<ot::microamps>(*uAunit);
  auto mA = std::get<ot::milliamps>(*mAunit);
  auto  A = std::get<ot::amps>(*Aunit);
  auto kA = std::get<ot::kiloamps>(*kAunit);

  REQUIRE(std::fabs(A/uA - 1000000.0f) < 1e-6);
  REQUIRE(std::fabs(A/mA - 1000.0f) < 1e-6);
  REQUIRE(std::fabs(A/A  - 1.0f) < 1e-6);
  REQUIRE(std::fabs(A/kA - 0.001) < 1e-6);
  
  REQUIRE(std::fabs(ot::divide_current_unit(A, A)  - 1.0f) < 1e-6);
  REQUIRE(std::fabs(ot::divide_current_unit(A, kA) - 0.001) < 1e-6);
}

// Testcase:: ResistanceUnit
TEST_CASE("ResistanceUnit") {
  
  REQUIRE(!ot::make_resistance_unit("123"));
  REQUIRE(!ot::make_resistance_unit("abc"));

  auto ohmunit  = ot::make_resistance_unit("1ohm");
  auto kohmunit = ot::make_resistance_unit("1kohm"); 

  REQUIRE(ohmunit.has_value());
  REQUIRE(kohmunit.has_value());

  auto ohm  = std::get<ot::ohms>(*ohmunit);
  auto kohm = std::get<ot::kiloohms>(*kohmunit);
  
  REQUIRE(std::fabs(ohm / kohm - 0.001f < 1e-6));
  REQUIRE(std::fabs(kohm / ohm - 1000.0f < 1e-6));
  
  REQUIRE(std::fabs(ot::divide_resistance_unit(ohm, kohm) - 0.001f < 1e-6));
  REQUIRE(std::fabs(ot::divide_resistance_unit(kohm, ohm) - 1000.0f < 1e-6));
}

// Testcase: PowerUnit
TEST_CASE("PowerUnit") {
  
  REQUIRE(!ot::make_power_unit("123"));
  REQUIRE(!ot::make_power_unit("abc"));
  
  auto pWunit = ot::make_power_unit("1pW");
  auto nWunit = ot::make_power_unit("1nW");
  auto uWunit = ot::make_power_unit("1uW");
  auto mWunit = ot::make_power_unit("1mW");
  
  REQUIRE(pWunit.has_value());
  REQUIRE(nWunit.has_value());
  REQUIRE(uWunit.has_value());
  REQUIRE(mWunit.has_value());

  auto pW = std::get<ot::picowatts>(*pWunit);
  auto nW = std::get<ot::nanowatts>(*nWunit);
  auto uW = std::get<ot::microwatts>(*uWunit);
  auto mW = std::get<ot::milliwatts>(*mWunit);

  REQUIRE(std::fabs(pW/nW - 0.001f) < 1e-6);
  REQUIRE(std::fabs(nW/uW - 0.001f) < 1e-6);
  REQUIRE(std::fabs(uW/mW - 0.001f) < 1e-6);
  
  REQUIRE(std::fabs(ot::divide_power_unit(pW, nW) - 0.001f) < 1e-6);
  REQUIRE(std::fabs(ot::divide_power_unit(nW, uW) - 0.001f) < 1e-6);
  REQUIRE(std::fabs(ot::divide_power_unit(uW, mW) - 0.001f) < 1e-6);
}
*/

