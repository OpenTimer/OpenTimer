#include <ot/unit/unit.hpp>

namespace ot {

// Function: make_time_unit
std::optional<second_t> make_time_unit(std::string_view str) {

  using namespace units::literals;

  static const std::regex unit_regex(
    "([\\+-]?\\d*\\.?\\d+)\\s*([fpnumkM]?)\\s*(s)", 
    std::regex::icase
  );

  if(std::cmatch pieces; std::regex_match(str.begin(), str.end(), pieces, unit_regex)) {

    if(pieces.size() != 4) {
      return std::nullopt;
    }
    
    auto s = std::stof(pieces[1].str());

    if(const auto& b = pieces[2].str(); b.empty()) {
      return s* 1_s;
    }
    else if(b == "f") {
      return s * 1_fs;
    }
    else if(b == "p") {
      return s * 1_ps;
    }
    else if(b == "n") {
      return s * 1_ns;
    }
    else if(b == "u") {
      return s * 1_us;
    }
    else if(b == "m") {
      return s * 1_ms;
    }
    else if(b == "k" || b == "K") {
      return s * 1_ks;
    }
    else if(b == "M") {
      return s * 1_Ms;
    }
    else return std::nullopt;
  }
  else return std::nullopt;
}

// Function: make_power_unit
std::optional<watt_t> make_power_unit(std::string_view str) {

  using namespace units::literals;

  static const std::regex unit_regex(
    "([\\+-]?\\d*\\.?\\d+)\\s*([fpnumkM]?)\\s*(W)", 
    std::regex::icase
  );

  if(std::cmatch pieces; std::regex_match(str.begin(), str.end(), pieces, unit_regex)) {

    if(pieces.size() != 4) {
      return std::nullopt;
    }

    auto s = std::stof(pieces[1].str());

    if(const auto& b = pieces[2].str(); b.empty()) {
      return s * 1_W;
    }
    else if(b == "f") {
      return s * 1_fW;
    }
    else if(b == "p") {
      return s * 1_pW;
    }
    else if(b == "n") {
      return s * 1_nW;
    }
    else if(b == "u") {
      return s * 1_uW;
    }
    else if(b == "m") {
      return s * 1_mW;
    }
    else if(b == "k" || b == "K") {
      return s * 1_kW;
    }
    else if(b == "M") {
      return s * 1_MW;
    }
    else {
      return std::nullopt;
    }
  }
  else return std::nullopt;
}

// Function: make_capacitance_unit
std::optional<farad_t> make_capacitance_unit(std::string_view str) {

  using namespace units::literals;

  static const std::regex unit_regex(
    "([\\+-]?\\d*\\.?\\d+)\\s*([fpnumkM]?)\\s*(F)", 
    std::regex::icase
  );

  if(std::cmatch pieces; std::regex_match(str.begin(), str.end(), pieces, unit_regex)) {

    if(pieces.size() != 4) {
      return std::nullopt;
    }

    auto s = std::stof(pieces[1].str());

    if(const auto& b = pieces[2].str(); b.empty()) {
      return s * 1_F;
    }
    else if(b == "f") {
      return s * 1_fF;
    }
    else if(b == "p") {
      return s * 1_pF;
    }
    else if(b == "n") {
      return s * 1_nF;
    }
    else if(b == "u") {
      return s * 1_uF;
    }
    else if(b == "m") {
      return s * 1_mF;
    }
    else if(b == "k" || b == "K") {
      return s * 1_kF;
    }
    else if(b == "M") {
      return s * 1_MF;
    }
    else {
      return std::nullopt;
    }
  }
  else return std::nullopt;
}

// Function: make_voltage_unit
std::optional<volt_t> make_voltage_unit(std::string_view str) {

  using namespace units::literals;

  static const std::regex unit_regex(
    "([\\+-]?\\d*\\.?\\d+)\\s*([fpnumkM]?)\\s*(V)",
    std::regex::icase
  );

  if(std::cmatch pieces; std::regex_match(str.begin(), str.end(), pieces, unit_regex)) {

    if(pieces.size() != 4) {
      return std::nullopt;
    }

    auto s = std::stof(pieces[1].str());

    if(const auto& b = pieces[2].str(); b.empty()) {
      return s * 1_V;
    }
    else if(b == "f") {
      return s * 1_fV;
    }
    else if(b == "p") {
      return s * 1_pV;
    }
    else if(b == "n") {
      return s * 1_nV;
    }
    else if(b == "u") {
      return s * 1_uV;
    }
    else if(b == "m") {
      return s * 1_mV;
    }
    else if(b == "k" || b == "K") {
      return s * 1_kV;
    }
    else if(b == "M") {
      return s * 1_MV;
    }
    else {
      return std::nullopt;
    }
  }
  else return std::nullopt;
}

// Function: make_current_unit
std::optional<ampere_t> make_current_unit(std::string_view str) {

  using namespace units::literals;

  static const std::regex unit_regex(
    "([\\+-]?\\d*\\.?\\d+)\\s*([fpnumkM]?)\\s*(A)",
    std::regex::icase
  );

  if(std::cmatch pieces; std::regex_match(str.begin(), str.end(), pieces, unit_regex)) {

    if(pieces.size() != 4) {
      return std::nullopt;
    }

    auto s = std::stof(pieces[1].str());

    if(const auto& b = pieces[2].str(); b.empty()) {
      return s * 1_A;
    }
    else if(b == "f") {
      return s * 1_fA;
    }
    else if(b == "p") {
      return s * 1_pA;
    }
    else if(b == "n") {
      return s * 1_nA;
    }
    else if(b == "u") {
      return s * 1_uA;
    }
    else if(b == "m") {
      return s * 1_mA;
    }
    else if(b == "k" || b == "K") {
      return s * 1_kA;
    }
    else if(b == "M") {
      return s * 1_MA;
    }
    else {
      return std::nullopt;
    }
  }
  else return std::nullopt;
}

// Function: make_resistance_unit
std::optional<ohm_t> make_resistance_unit(std::string_view str) {

  using namespace units::literals;

  static const std::regex unit_regex(
    "([\\+-]?\\d*\\.?\\d+)\\s*([fpnumkM]?)\\s*(ohm)",
    std::regex::icase
  );

  if(std::cmatch pieces; std::regex_match(str.begin(), str.end(), pieces, unit_regex)) {

    if(pieces.size() != 4) {
      return std::nullopt;
    }

    auto s = std::stof(pieces[1].str());

    if(const auto& b = pieces[2].str(); b.empty()) {
      return s * 1_Ohm;
    }
    else if(b == "f") {
      return s * 1_fOhm;
    }
    else if(b == "p") {
      return s * 1_pOhm;
    }
    else if(b == "n") {
      return s * 1_nOhm;
    }
    else if(b == "u") {
      return s * 1_uOhm;
    }
    else if(b == "m") {
      return s * 1_mOhm;
    }
    else if(b == "k" || b == "K") {
      return s * 1_kOhm;
    }
    else if(b == "M") {
      return s * 1_MOhm;
    }
    else {
      return std::nullopt;
    }
  }
  else return std::nullopt;
}

};  // end of namespace ot. -----------------------------------------------------------------------





