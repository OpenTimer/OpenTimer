#include <ot/shell/shell.hpp>

namespace ot {

// Procedure: update_timing
void Shell::_update_timing() {
  _timer.update_timing();
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_at
void Shell::_report_at() {

  std::string token;
  std::string pin;
  auto el = EARLY;
  auto rf = RISE;

  while(_is >> token) {
    if(token == "-pin") _is >> pin;
    else if(token == "-early") el = EARLY;
    else if(token == "-late" ) el = LATE;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      _es << "failed to parse " << std::quoted(token) << '\n';
    }
  }

  if(pin.empty()) {
    _es << "-pin <name> not given\n";
    return;
  }

  auto at = _timer.at(std::move(pin), el, rf);
  _os << (at ? *at : std::numeric_limits<float>::quiet_NaN()) << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_rat
void Shell::_report_rat() {

  std::string token;
  std::string pin;
  auto el = EARLY;
  auto rf = RISE;

  while(_is >> token) {
    if(token == "-pin") _is >> pin;
    else if(token == "-early") el = EARLY;
    else if(token == "-late" ) el = LATE;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      _es << "failed to parse " << std::quoted(token) << '\n';
    }
  }

  if(pin.empty()) {
    _es << "-pin <name> not given\n";
    return;
  }

  auto rat = _timer.rat(std::move(pin), el, rf);
  _os << (rat ? *rat : std::numeric_limits<float>::quiet_NaN()) << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_slack
void Shell::_report_slack() {

  std::string token;
  std::string pin;
  auto el = EARLY;
  auto rf = RISE;

  while(_is >> token) {
    if(token == "-pin") _is >> pin;
    else if(token == "-early") el = EARLY;
    else if(token == "-late" ) el = LATE;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      _es << "failed to parse " << std::quoted(token) << '\n';
    }
  }

  if(pin.empty()) {
    _es << "-pin <name> not given\n";
    return;
  }

  auto slack = _timer.slack(std::move(pin), el, rf);
  _os << (slack ? *slack : std::numeric_limits<float>::quiet_NaN()) << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_slew
void Shell::_report_slew() {

  std::string token;
  std::string pin;
  auto el = EARLY;
  auto rf = RISE;

  while(_is >> token) {
    if(token == "-pin") _is >> pin;
    else if(token == "-early") el = EARLY;
    else if(token == "-late" ) el = LATE;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      _es << "failed to parse " << std::quoted(token) << '\n';
    }
  }

  if(pin.empty()) {
    _es << "-pin <name> not given\n";
    return;
  }

  auto slew = _timer.slew(std::move(pin), el, rf);
  _os << (slew ? *slew : std::numeric_limits<float>::quiet_NaN()) << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_tns
void Shell::_report_tns() {
  if(auto tns = _timer.tns(); tns) {
    _os << *tns << '\n';
  }
  else {
    _os << std::numeric_limits<float>::quiet_NaN() << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_wns
void Shell::_report_wns() {
  if(auto wns = _timer.wns(); wns) {
    _os << *wns << '\n';
  }
  else {
    _os << std::numeric_limits<float>::quiet_NaN() << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_fep
void Shell::_report_fep() {
  if(auto fep = _timer.fep(); fep) {
    _os << *fep << '\n';
  }
  else {
    _os << "no endpoints found\n";
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_timing
void Shell::_report_timing() {
  if(auto paths = _timer.worst_paths(1); !paths.empty()) {
    _os << paths[0];
  }
  else {
    _os << empty_path;
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_path
void Shell::_report_path() {

  std::string token;
  size_t K {1};

  while(_is >> token) {
    if(token == "-num_paths") {
      _is >> K;
    }
    else {
      _es << "failed to parse " << std::quoted(token) << '\n';
    }
  }

  auto paths = _timer.worst_paths(K);

  for(const auto& path : paths) {
    _os << path;
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_area
void Shell::_report_area() {
  if(auto area = _timer.area(); area) {
    _os << *area << '\n';
  }
  else {
    _os << std::numeric_limits<float>::quiet_NaN() << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_leakage_power
void Shell::_report_leakage_power() {
  if(auto lp = _timer.leakage_power(); lp) {
    _os << *lp << '\n';
  }
  else {
    _os << std::numeric_limits<float>::quiet_NaN() << '\n';
  }
}

};  // end of namespace ot. -----------------------------------------------------------------------









