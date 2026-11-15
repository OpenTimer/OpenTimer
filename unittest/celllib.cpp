#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include <ot/liberty/celllib.hpp>
#include <filesystem>
#include <fstream>

// Function: write_celllib
// Write a small library to a file. Timing tables are scalars (no template).
static std::filesystem::path write_celllib(const std::string& name, const std::string& body) {
  auto path = std::filesystem::temp_directory_path() / name;
  std::ofstream ofs(path);
  ofs << "library (test) {\n"
      << "  delay_model : table_lookup;\n"
      << "  time_unit : \"1ns\";\n"
      << "  cell (XOR2) {\n"
      << body
      << "  }\n"
      << "}\n";
  return path;
}

static std::string timing(
  const std::string& related, const std::string& sense, const std::string& rise, const std::string& fall
) {
  return "      timing () {\n"
         "        related_pin : \"" + related + "\";\n"
         "        timing_sense : " + sense + ";\n"
         "        cell_rise (scalar) { values (\"" + rise + "\"); }\n"
         "        cell_fall (scalar) { values (\"" + fall + "\"); }\n"
         "      }\n";
}

// Testcase: MultipleTimingGroups
TEST_CASE("MultipleTimingGroups") {

  // Subcase: positive and negative unate groups of the same related pin are both kept.
  SUBCASE("NonUnate") {
    auto path = write_celllib("ot_celllib_non_unate.lib",
      "    pin (A) { direction : input; }\n"
      "    pin (Z) {\n"
      "      direction : output;\n" +
      timing("A", "positive_unate", "0.1", "0.2") +
      timing("A", "negative_unate", "0.3", "0.4") +
      "    }\n"
    );

    ot::Celllib lib;
    lib.read(path);
    std::filesystem::remove(path);

    auto cell = lib.cell("XOR2");
    REQUIRE(cell != nullptr);
    const auto& timings = cell->cellpins.at("Z").timings;
    REQUIRE(timings.size() == 2);
    REQUIRE(timings[0].related_pin == "A");
    REQUIRE(timings[0].sense == ot::TimingSense::POSITIVE_UNATE);
    REQUIRE(timings[0].cell_rise->table[0] == doctest::Approx(0.1f));
    REQUIRE(timings[1].related_pin == "A");
    REQUIRE(timings[1].sense == ot::TimingSense::NEGATIVE_UNATE);
    REQUIRE(timings[1].cell_rise->table[0] == doctest::Approx(0.3f));
  }

  // Subcase: groups of different related pins stay separate and keep their order.
  SUBCASE("TwoRelatedPins") {
    auto path = write_celllib("ot_celllib_two_pins.lib",
      "    pin (A) { direction : input; }\n"
      "    pin (B) { direction : input; }\n"
      "    pin (Z) {\n"
      "      direction : output;\n" +
      timing("A", "positive_unate", "0.1", "0.2") +
      timing("A", "negative_unate", "0.3", "0.4") +
      timing("B", "positive_unate", "0.5", "0.6") +
      timing("B", "negative_unate", "0.7", "0.8") +
      "    }\n"
    );

    ot::Celllib lib;
    lib.read(path);
    std::filesystem::remove(path);

    const auto& timings = lib.cell("XOR2")->cellpins.at("Z").timings;
    REQUIRE(timings.size() == 4);
    REQUIRE(timings[2].related_pin == "B");
    REQUIRE(timings[3].related_pin == "B");
    REQUIRE(timings[3].cell_fall->table[0] == doctest::Approx(0.8f));
  }

  // Subcase: an internal_power group read before the timing groups still gets one timing entry
  // filled in, and the other timing group of the same related pin is added.
  SUBCASE("InternalPowerFirst") {
    auto path = write_celllib("ot_celllib_power_first.lib",
      "    pin (A) { direction : input; }\n"
      "    pin (Z) {\n"
      "      direction : output;\n"
      "      internal_power () {\n"
      "        related_pin : \"A\";\n"
      "        rise_power (scalar) { values (\"0.9\"); }\n"
      "      }\n" +
      timing("A", "positive_unate", "0.1", "0.2") +
      timing("A", "negative_unate", "0.3", "0.4") +
      "    }\n"
    );

    ot::Celllib lib;
    lib.read(path);
    std::filesystem::remove(path);

    const auto& timings = lib.cell("XOR2")->cellpins.at("Z").timings;
    REQUIRE(timings.size() == 2);
    REQUIRE(timings[0].sense == ot::TimingSense::POSITIVE_UNATE);
    REQUIRE(timings[0].internal_power.rise_power.has_value());
    REQUIRE(timings[1].sense == ot::TimingSense::NEGATIVE_UNATE);
  }
}
