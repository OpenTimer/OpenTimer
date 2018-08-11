#include <ot/shell/shell.hpp>

namespace ot {

// Procedure: update_timing
void update_timing(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {
  timer.update_timing();
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_at
void report_at(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  std::string token;
  std::string pin;
  auto el = EARLY;
  auto rf = RISE;
  std::optional<float> value;

  while(is >> token) {
    if(token == "-pin") is >> pin;
    else if(token == "-early") el = EARLY;
    else if(token == "-late" ) el = LATE;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      es << "failed to parse " << std::quoted(token) << '\n';
    }
  }

  if(pin.empty()) {
    es << "-pin <name> not given\n";
    return;
  }

  auto at = timer.at(std::move(pin), el, rf);
  os << (at ? *at : std::numeric_limits<float>::quiet_NaN()) << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_rat
void report_rat(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  std::string token;
  std::string pin;
  auto el = EARLY;
  auto rf = RISE;
  std::optional<float> value;

  while(is >> token) {
    if(token == "-pin") is >> pin;
    else if(token == "-early") el = EARLY;
    else if(token == "-late" ) el = LATE;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      es << "failed to parse " << std::quoted(token) << '\n';
    }
  }

  if(pin.empty()) {
    es << "-pin <name> not given\n";
    return;
  }

  auto rat = timer.rat(std::move(pin), el, rf);
  os << (rat ? *rat : std::numeric_limits<float>::quiet_NaN()) << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_slack
void report_slack(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  std::string token;
  std::string pin;
  auto el = EARLY;
  auto rf = RISE;
  std::optional<float> value;

  while(is >> token) {
    if(token == "-pin") is >> pin;
    else if(token == "-early") el = EARLY;
    else if(token == "-late" ) el = LATE;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      es << "failed to parse " << std::quoted(token) << '\n';
    }
  }

  if(pin.empty()) {
    es << "-pin <name> not given\n";
    return;
  }

  auto slack = timer.slack(std::move(pin), el, rf);
  os << (slack ? *slack : std::numeric_limits<float>::quiet_NaN()) << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_slew
void report_slew(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {

  std::string token;
  std::string pin;
  auto el = EARLY;
  auto rf = RISE;
  std::optional<float> value;

  while(is >> token) {
    if(token == "-pin") is >> pin;
    else if(token == "-early") el = EARLY;
    else if(token == "-late" ) el = LATE;
    else if(token == "-rise" ) rf = RISE;
    else if(token == "-fall" ) rf = FALL;
    else {
      es << "failed to parse " << std::quoted(token) << '\n';
    }
  }

  if(pin.empty()) {
    es << "-pin <name> not given\n";
    return;
  }

  auto slew = timer.slew(std::move(pin), el, rf);
  os << (slew ? *slew : std::numeric_limits<float>::quiet_NaN()) << '\n';
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_tns
void report_tns(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {
  if(auto tns = timer.tns(); tns) {
    os << *tns << '\n';
  }
  else {
    os << std::numeric_limits<float>::quiet_NaN() << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_wns
void report_wns(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {
  if(auto wns = timer.wns(); wns) {
    os << *wns << '\n';
  }
  else {
    os << std::numeric_limits<float>::quiet_NaN() << '\n';
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_fep
void report_fep(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {
  if(auto fep = timer.fep(); fep) {
    os << *fep << '\n';
  }
  else {
    os << "no endpoints found\n";
  }
}

// ------------------------------------------------------------------------------------------------

// Procedure: report_timing
void report_timing(Timer& timer, std::istream& is, std::ostream& os, std::ostream& es) {
  if(auto paths = timer.worst_paths(1); !paths.empty()) {
    os << paths[0];
  }
  else {
    os << empty_path;
  }
}

};  // end of namespace ot. -----------------------------------------------------------------------





